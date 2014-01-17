#include "StdAfx.h"
#include "tcp_net_facade_imp.h"

#include "icombiner.h"
#include "net_conn.h"
#include "inetwork_imp.h"
#include "icombiner_imp.h"
#include "inetpacket_mgr_imp.h"
#include "dispatcher_handler.h"
#include "inet_conn_mgr_imp.h"

tcp_net_facade_imp::tcp_net_facade_imp(void)
{
    icombiner_        = NULL;
    inetwork_         = NULL;
    dispatch_handler_ = NULL;
    net_packet_mgr_   = NULL;
    net_conn_mgr_     = NULL;
    init_success_     = false;

}

tcp_net_facade_imp::~tcp_net_facade_imp(void)
{
}

bool tcp_net_facade_imp::set_icombiner(icombiner* pCombiner) 
{
    _ASSERT(pCombiner);
    guard helper(mutex_);

    icombiner_ = pCombiner;
    return icombiner_ != NULL;
}

bool tcp_net_facade_imp::set_dispatch_handler(dispatcher_handler* pHandler) 
{
    _ASSERT(pHandler);
    guard helper(mutex_);

    dispatch_handler_ = pHandler;
    return dispatch_handler_ != NULL;
}

bool tcp_net_facade_imp::set_net_conn_mgr(inet_conn_mgr* pMgr) 
{
    _ASSERT(pMgr);
    guard helper(mutex_);

    net_conn_mgr_ = pMgr;
    return net_conn_mgr_ != NULL;
}

bool tcp_net_facade_imp::init_client()
{
    if (init_success_)
    {
        return init_success_;
    }

    inetwork_ = new inetwork_imp;
    _ASSERT(inetwork_);
    inetwork_->init_network(this);
    inetwork_->run_network();

    // ��ʼ��Ĭ�ϵĴ�����
    icombiner_ = new icombiner_imp;
    net_packet_mgr_ = new inetpacket_mgr_imp;
    net_conn_mgr_ = new inet_conn_mgr_imp;

    init_success_ = (icombiner_ && net_packet_mgr_ && net_conn_mgr_);

    return init_success_;
}

bool tcp_net_facade_imp::add_event_handler(inet_event_handler* handler) 
{
    guard helper(mutex_);

#ifdef _DEBUG
    INetEventHandlerListType::const_iterator itr;
    itr = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) 
    {
        if ((*itr) == handler) {
            _ASSERT(FALSE);
        }
    }
#endif

    net_event_list_.push_back(handler);
    return true;
}

bool tcp_net_facade_imp::del_event_handler(inet_event_handler* handler) 
{
    guard helper(mutex_);

    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr)
    {
        if (handler == (*itr)) {
            net_event_list_.erase(itr);
            return true;
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
bool tcp_net_facade_imp::on_connect(net_conn* pConn, bool bConnected/* = true*/) 
{
    if (bConnected) 
    {
        //const char *msg = "GET / HTTP/1.1\r\nAccept: */*\r\nHost: www.baidu.com\r\n\r\n";
        //inetwork_->post_write(pConn, msg, strlen(msg));
    }
    else 
    {
        //TRACE(TEXT("����ʧ��!\r\n"));
    }

#ifdef _DEBUG
    if (net_conn_mgr_) 
    {
        if (net_conn_mgr_->is_exist_conn(pConn)) 
        {
            _ASSERT(FALSE && "������Ӵ�����ƴ���");
        }
    }
#endif

    //��������ӵ����ӹ�������
    if (net_conn_mgr_) {
        net_conn_mgr_->add_conn(pConn);
    }

    // ֪ͨ�ϲ㴦�������¼�
    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) 
    {
        (*itr)->on_connect(pConn, bConnected);
    }

    return true;
}

bool tcp_net_facade_imp::on_disconnect(net_conn* pConn) 
{
    //MyPrtLog("���ӶϿ���...\r\n");

#ifdef _DEBUG
    if (net_conn_mgr_) 
    {
        if (!net_conn_mgr_->is_exist_conn(pConn)) 
        {
            // ������������ڸղ������ϻ�û����ӵ�mgr���棬����Ͷ�ݶ�����ͻ����;
            _ASSERT(FALSE && "���ӹرմ�����ƴ���");
            return true;
        }
    }
#endif

    //�������Ƚ����ϲ㴦�����ͷ�
    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) 
    {
        (*itr)->on_disconnect(pConn);
    }

    // ɾ������
    if (net_conn_mgr_) 
    {
        net_conn_mgr_->del_conn(pConn);
    }

    // �ͷ����е����ݰ�
    if (net_packet_mgr_) 
    {
        net_packet_mgr_->free_conn_packets(pConn);
    }

    return true;
}

bool tcp_net_facade_imp::on_read(net_conn* pConn, const char* buff, size_t len)
{
    //MyPrtLog("����������Len: %d", len);
    _ASSERT(buff);
    int iPacketLen = 0;
    bool bIsCombined = false;

    pConn->write_recv_stream(buff, len);
    bIsCombined = icombiner_->IsIntactPacket(pConn->get_recv_stream(), iPacketLen);

    while (bIsCombined) 
    {
        //��ӷ��
        //MyPrtLog("�ѹ����������....\r\n");

        net_packet* pPacket = net_packet_mgr_->alloc_net_packet();
        if (NULL == pPacket) 
        {
            _ASSERT(FALSE);
            return false;
        }

        //TRACE(TEXT("�������..."));
        pPacket->pConn    = pConn;
        pPacket->bf_size_ = iPacketLen;
        if (pPacket->init_buff(iPacketLen)) {
            return false;
        }
        pConn->read_recv_stream(pPacket->get_buff(), iPacketLen);

        if (dispatch_handler_) 
        {
            dispatch_handler_->DispatchPacket(pPacket);
            net_packet_mgr_->free_net_packet(pPacket);
        }
        else 
        {
            net_packet_mgr_->add_packet(pPacket);
        }

        if (pConn->get_recv_length() <= 0)
        {
            break;
        }

        bIsCombined = icombiner_->IsIntactPacket(pConn->get_recv_stream(), iPacketLen);
    }
    
    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) {
        (*itr)->on_read(pConn, buff, len);
    }

    return true;
}

bool tcp_net_facade_imp::on_write(net_conn* pConn) 
{
    //MyPrtLog("д�������..\r\n");

    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) {
        (*itr)->on_write(pConn);
    }

    return true;
}

bool tcp_net_facade_imp::on_accept(net_conn* pListen, net_conn* pNewConn, bool bSuccess/* = true*/) 
{
    if (net_conn_mgr_ && bSuccess) {
#ifdef _DEBUG
        if (net_conn_mgr_->is_exist_conn(pNewConn)) {
            _ASSERT(FALSE && "�ظ�������ӣ�������Ƴ���!");
        }
#endif
        net_conn_mgr_->add_conn(pNewConn);
    }

    if (bSuccess) {
        //MyPrtLog("��������...");
    }
    else {
        //MyPrtLog("��������ʧ��...");
    }

    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) {
        (*itr)->on_accept(pListen, pNewConn, bSuccess);
    }
    return true;
}