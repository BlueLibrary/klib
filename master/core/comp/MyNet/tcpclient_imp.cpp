#include "StdAfx.h"
#include "tcpclient_imp.h"

#include "icombiner.h"
#include "net_conn.h"
#include "INetworkImp.h"
#include "icombiner_imp.h"
#include "inetpacket_mgr_imp.h"
#include "dispatcher_handler.h"
#include "inet_conn_mgr_imp.h"

tcpclient_imp::tcpclient_imp(void)
{
    m_pICombiner = NULL;
    m_pINetwork = NULL;
    m_pIDispatcher = NULL;
    m_pNetPacketMgr = NULL;
    m_pINetConnMgr = NULL;
    m_bInitSucc  =    false;

}

tcpclient_imp::~tcpclient_imp(void)
{
}

bool tcpclient_imp::SetICombiner(icombiner* pCombiner) 
{
    _ASSERT(pCombiner);
    auto_lock helper(m_cs);

    m_pICombiner = pCombiner;
    return m_pICombiner != NULL;
}

bool tcpclient_imp::SetIDispatchHandler(dispatcher_handler* pHandler) 
{
    _ASSERT(pHandler);
    auto_lock helper(m_cs);

    m_pIDispatcher = pHandler;
    return m_pIDispatcher != NULL;
}

bool tcpclient_imp::SetINetConnectionMgr(inet_conn_mgr* pMgr) 
{
    _ASSERT(pMgr);
    auto_lock helper(m_cs);

    m_pINetConnMgr = pMgr;
    return m_pINetConnMgr != NULL;
}

bool tcpclient_imp::InitClient()
{
    if (m_bInitSucc)
    {
        return m_bInitSucc;
    }

    m_pINetwork = new inetwork_imp;
    _ASSERT(m_pINetwork);
    m_pINetwork->init_network();
    m_pINetwork->set_net_event_handler(this);
    m_pINetwork->run_network();

    // ��ʼ��Ĭ�ϵĴ�����
    m_pICombiner = new icombiner_imp;
    m_pNetPacketMgr = new inetpacket_mgr_imp;
    m_pINetConnMgr = new inet_conn_mgr_imp;

    m_bInitSucc = (m_pICombiner && m_pNetPacketMgr && m_pINetConnMgr);

    return m_bInitSucc;
}

bool tcpclient_imp::AddEventHandler(inet_event_handler* handler) 
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

bool tcpclient_imp::RemoveEventHandler(inet_event_handler* handler) 
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
bool tcpclient_imp::OnConnect(net_conn* pConn, bool bConnected/* = true*/) 
{
    if (bConnected) {
        //const char *msg = "GET / HTTP/1.1\r\nAccept: */*\r\nHost: www.baidu.com\r\n\r\n";
        //m_pINetwork->post_write(pConn, msg, strlen(msg));
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

bool tcpclient_imp::OnDisConnect(net_conn* pConn) 
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
        m_pINetConnMgr->del_conn(pConn);
    }
    if (m_pNetPacketMgr) {
        m_pNetPacketMgr->free_conn_packets(pConn);
    }

    return true;
}

bool tcpclient_imp::OnRead(net_conn* pConn, const char* buff, size_t len)
{
    //MyPrtLog("����������Len: %d", len);
    _ASSERT(buff);
    int iPacketLen = 0;
    bool bIsCombined = false;

    pConn->write_recv_stream(buff, len);
    bIsCombined = m_pICombiner->IsIntactPacket(pConn->get_recv_stream(), iPacketLen);

    while (bIsCombined) 
    {
        //��ӷ��
        //MyPrtLog("�ѹ����������....\r\n");

        net_packet* pPacket = m_pNetPacketMgr->alloc_net_packet();
        if (NULL == pPacket) 
        {
            _ASSERT(FALSE);
            return false;
        }

        //TRACE(TEXT("�������..."));
        pPacket->pConn = pConn;
        pPacket->datalen = iPacketLen;
        pConn->read_recv_stream(pPacket->buff, iPacketLen);

        if (m_pIDispatcher) 
        {
            m_pIDispatcher->DispatchPacket(pPacket);
            m_pNetPacketMgr->free_net_packet(pPacket);
        }
        else 
        {
            m_pNetPacketMgr->add_packet(pPacket);
        }

        if (pConn->get_recv_length() <= 0)
        {
            break;
        }

        bIsCombined = m_pICombiner->IsIntactPacket(pConn->get_recv_stream(), iPacketLen);
    }
    
    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) {
        (*itr)->OnRead(pConn, buff, len);
    }

    return true;
}

bool tcpclient_imp::OnWrite(net_conn* pConn) 
{
    //MyPrtLog("д�������..\r\n");

    INetEventHandlerListType::const_iterator itr;
    itr  = m_INetEventList.begin();
    for (; itr != m_INetEventList.end(); ++itr) {
        (*itr)->OnWrite(pConn);
    }

    return true;
}

bool tcpclient_imp::OnAccept(net_conn* pListen, net_conn* pNewConn, bool bSuccess/* = true*/) 
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