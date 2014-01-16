#include "StdAfx.h"
#include "INetClientImp.h"

#include "ICombiner.h"
#include "INetConnection.h"
#include "INetworkImp.h"
#include "ICombinerImp.h"
#include "INetPacketMgrImp.h"
#include "IDispatchHandler.h"
#include "INetConnectionMgrImp.h"

INetTcpClientImp::INetTcpClientImp(void)
{
    m_pICombiner = NULL;
    m_pINetwork = NULL;
    m_pIDispatcher = NULL;
    m_pNetPacketMgr = NULL;
    m_pINetConnMgr = NULL;
    m_bInitSucc  =    false;

}

INetTcpClientImp::~INetTcpClientImp(void)
{
}

bool INetTcpClientImp::SetICombiner(ICombiner* pCombiner) 
{
    _ASSERT(pCombiner);
    auto_lock helper(m_cs);

    m_pICombiner = pCombiner;
    return m_pICombiner != NULL;
}

bool INetTcpClientImp::SetIDispatchHandler(IDispatchHandler* pHandler) 
{
    _ASSERT(pHandler);
    auto_lock helper(m_cs);

    m_pIDispatcher = pHandler;
    return m_pIDispatcher != NULL;
}

bool INetTcpClientImp::SetINetConnectionMgr(INetConnectionMgr* pMgr) 
{
    _ASSERT(pMgr);
    auto_lock helper(m_cs);

    m_pINetConnMgr = pMgr;
    return m_pINetConnMgr != NULL;
}

bool INetTcpClientImp::InitClient()
{
    if (m_bInitSucc)
    {
        return m_bInitSucc;
    }

    m_pINetwork = new INetNetworkImp;
    _ASSERT(m_pINetwork);
    m_pINetwork->InitNetwork();
    m_pINetwork->SetNetEventHandler(this);
    m_pINetwork->RunNetwork();

    // ��ʼ��Ĭ�ϵĴ�����
    m_pICombiner = new ICombinerImp;
    m_pNetPacketMgr = new INetPacketMgrImp;
    m_pINetConnMgr = new INetConnectionMgrImp;

    m_bInitSucc = (m_pICombiner && m_pNetPacketMgr && m_pINetConnMgr);

    return m_bInitSucc;
}

bool INetTcpClientImp::AddEventHandler(INetEventHandler* handler) 
{
    auto_lock helper(m_cs);

#ifdef _DEBUG
    INetEventHandlerListType::const_iterator itr;
    itr = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) 
    {
        if ((*itr) == handler) {
            _ASSERT(FALSE);
        }
    }
#endif

    m_INetEventList.push_back(handler);
    return true;
}

bool INetTcpClientImp::RemoveEventHandler(INetEventHandler* handler) 
{
    auto_lock helper(m_cs);

    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr)
    {
        if (handler == (*itr)) {
            m_INetEventList.erase(itr);
            return true;
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
bool INetTcpClientImp::OnConnect(INetConnection* pConn, bool bConnected/* = true*/) 
{
    if (bConnected) {
        //const char *msg = "GET / HTTP/1.1\r\nAccept: */*\r\nHost: www.baidu.com\r\n\r\n";
        //m_pINetwork->PostWrite(pConn, msg, strlen(msg));
    }
    else {
        //TRACE(TEXT("����ʧ��!\r\n"));
    }

#ifdef _DEBUG
    if (m_pINetConnMgr) {
        if (m_pINetConnMgr->is_exist_conn(pConn)) {
            _ASSERT(FALSE && "������Ӵ�����ƴ���");
        }
    }
#endif

    //��������ӵ����ӹ�������
    if (m_pINetConnMgr) {
        m_pINetConnMgr->add_conn(pConn);
    }

    //֪ͨ�����¼�
    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) {
        (*itr)->OnConnect(pConn, bConnected);
    }

    return true;
}

bool INetTcpClientImp::OnDisConnect(INetConnection* pConn) 
{
    //MyPrtLog("���ӶϿ���...\r\n");

#ifdef _DEBUG
    if (m_pINetConnMgr) {
        if (!m_pINetConnMgr->is_exist_conn(pConn)) {
            _ASSERT(FALSE && "���ӹرմ�����ƴ���");
            // ������������ڸղ������ϻ�û����ӵ�mgr���棬����Ͷ�ݶ�����ͻ����;
            return true;
        }
    }
#endif

    //�������Ƚ����ϲ㴦�����ͷ�
    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) {
        (*itr)->OnDisConnect(pConn);
    }

    if (m_pINetConnMgr) {
        m_pINetConnMgr->rmv_conn(pConn);
    }
    if (m_pNetPacketMgr) {
        m_pNetPacketMgr->FreeConnPacket(pConn);
    }

    return true;
}

bool INetTcpClientImp::OnRead(INetConnection* pConn, const char* buff, size_t len)
{
    //MyPrtLog("����������Len: %d", len);
    _ASSERT(buff);
    int iPacketLen = 0;
    bool bIsCombined = false;

    pConn->AddStream(buff, len);
    bIsCombined = m_pICombiner->IsIntactPacket(pConn->GetBuff(),
        pConn->GetDataLen(),
        iPacketLen);

    while (bIsCombined) 
    {
        //��ӷ��
        //MyPrtLog("�ѹ����������....\r\n");

        NetPacket* pPacket = m_pNetPacketMgr->AllocNetPacket();
        if (NULL == pPacket) 
        {
            _ASSERT(FALSE);
            return false;
        }

        //TRACE(TEXT("�������..."));
        pPacket->pConn = pConn;
        pPacket->datalen = iPacketLen;
        pConn->GetStream(pPacket->buff, iPacketLen);

        if (m_pIDispatcher) 
        {
            m_pIDispatcher->DispatchPacket(pPacket);
            m_pNetPacketMgr->FreeNetPacket(pPacket);
        }
        else 
        {
            m_pNetPacketMgr->AddPacket(pPacket);
        }

        if (pConn->GetDataLen() <= 0)
        {
            break;
        }

        bIsCombined = m_pICombiner->IsIntactPacket(pConn->GetBuff(), 
            pConn->GetDataLen(), 
            iPacketLen);
    }

#ifdef _DEBUG
    pConn->GetBuff()[pConn->GetDataLen()] = '\0';
    //MyPrtLog("ʣ������>>���� = %d \r\n", pConn->GetDataLen());
#endif  

    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) {
        (*itr)->OnRead(pConn, buff, len);
    }

    return true;
}

bool INetTcpClientImp::OnWrite(INetConnection* pConn) 
{
    //MyPrtLog("д�������..\r\n");

    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) {
        (*itr)->OnWrite(pConn);
    }

    return true;
}

bool INetTcpClientImp::OnAccept(INetConnection* pListen, INetConnection* pNewConn, bool bSuccess/* = true*/) 
{
    if (m_pINetConnMgr && bSuccess) {
#ifdef _DEBUG
        if (m_pINetConnMgr->is_exist_conn(pNewConn)) {
            _ASSERT(FALSE && "�ظ�������ӣ�������Ƴ���!");
        }
#endif
        m_pINetConnMgr->add_conn(pNewConn);
    }

    if (bSuccess) {
        //MyPrtLog("��������...");
    }
    else {
        //MyPrtLog("��������ʧ��...");
    }

    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) {
        (*itr)->OnAccept(pListen, pNewConn, bSuccess);
    }
    return true;
}