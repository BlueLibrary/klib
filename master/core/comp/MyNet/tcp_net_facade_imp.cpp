#include "stdafx.h"
#include "tcp_net_facade_imp.h"

#include "icombiner.h"
#include "net_conn.h"
#include "network_imp.h"
#include "icombiner_imp.h"
#include "dispatcher_handler.h"
#include "net_conn_mgr_imp.h"

tcp_net_facade_imp::tcp_net_facade_imp(void)
{
    icombiner_        = NULL;
    inetwork_         = NULL;

    dispatch_handler_ = NULL;
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

bool tcp_net_facade_imp::init()
{
    if (init_success_)
    {
        return init_success_;
    }

    // ��ʼ�������
    inetwork_ = new network_imp;
    _ASSERT(inetwork_);
    inetwork_->init_network(this);

    init_success_ = (nullptr != inetwork_ );

    return init_success_;
}

bool tcp_net_facade_imp::add_event_handler(inet_tcp_handler* handler) 
{
    guard helper(mutex_);

#ifdef _DEBUG
    INetEventHandlerListType::const_iterator itr;
    itr = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) 
    {
        if ((*itr) == handler) 
        {
            _ASSERT(FALSE);
        }
    }
#endif

    net_event_list_.push_back(handler);
    return true;
}

bool tcp_net_facade_imp::del_event_handler(inet_tcp_handler* handler) 
{
    guard helper(mutex_);

    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr)
    {
        if (handler == (*itr)) 
        {
            net_event_list_.erase(itr);
            return true;
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
bool tcp_net_facade_imp::on_connect(net_conn_ptr pConn, bool bConnected/* = true*/) 
{
    if (bConnected) 
    {
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

bool tcp_net_facade_imp::on_disconnect(net_conn_ptr pConn) 
{
    //�������Ƚ����ϲ㴦�����ͷ�
    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) 
    {
        (*itr)->on_disconnect(pConn);
    }

    return true;
}

bool tcp_net_facade_imp::on_read(net_conn_ptr pConn, const char* buff, size_t len)
{
    _ASSERT(pConn && buff && len > 0);
    int iPacketLen = 0;
    bool bIsCombined = false;
    net_packet packet_;

    while (icombiner_ && (bIsCombined = icombiner_->is_intact_packet(pConn->get_recv_stream(), iPacketLen))) 
    {
        // ��ʼ��packet
        net_packet* pPacket = & packet_;
        pPacket->conn_      = pConn;
        pPacket->buff_size_ = iPacketLen;
        if (!pPacket->init_buff(iPacketLen)) 
            return false;

        // ��������������
        pConn->read_recv_stream(pPacket->get_buff(), iPacketLen);

        // ���ɵ���
        if (dispatch_handler_) 
            dispatch_handler_->dispatch_packet(pPacket);
    }
    
    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) 
    {
        (*itr)->on_read(pConn, buff, len);
    }

    return true;
}

bool tcp_net_facade_imp::on_write(net_conn_ptr pConn, size_t len) 
{
    //MyPrtLog("д�������..\r\n");
    
    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) 
    {
        (*itr)->on_write(pConn, len);
    }
    
    return true;
}

bool tcp_net_facade_imp::on_accept(net_conn_ptr pListen, net_conn_ptr pNewConn, bool bSuccess/* = true*/) 
{
    if (bSuccess) 
    {
        //MyPrtLog("��������...");
    }
    else 
    {
        //MyPrtLog("��������ʧ��...");
    }

    INetEventHandlerListType::const_iterator itr;
    itr  = net_event_list_.begin();
    for (; itr != net_event_list_.end(); ++itr) {
        (*itr)->on_accept(pListen, pNewConn, bSuccess);
    }
    return true;
}