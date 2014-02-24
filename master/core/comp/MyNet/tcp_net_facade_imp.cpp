#include "stdafx.h"
#include "tcp_net_facade_imp.h"

#include "icombiner.h"
#include "net_conn.h"
#include "inetwork_imp.h"
#include "icombiner_imp.h"
#include "dispatcher_handler.h"
#include "inet_conn_mgr_imp.h"

tcp_net_facade_imp::tcp_net_facade_imp(void)
{
    icombiner_        = NULL;
    inetwork_         = NULL;
    net_conn_mgr_     = NULL;

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
    inetwork_ = new inetwork_imp;
    _ASSERT(inetwork_);
    inetwork_->init_network(this);

    // ��ʼ��Ĭ�ϵĴ�����
    //NULL == icombiner_? icombiner_ = new icombiner_imp : (void)0;
    NULL == net_conn_mgr_ ? net_conn_mgr_ = new inet_conn_mgr_imp : (void)0;

    init_success_ = (icombiner_ && net_conn_mgr_);

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
bool tcp_net_facade_imp::on_connect(net_conn* pConn, bool bConnected/* = true*/) 
{
    if (bConnected) 
    {
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
        if (net_conn_mgr_) 
        {
            net_conn_mgr_->add_conn(pConn);
        }
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
        net_conn_mgr_->del_conn(pConn);

    return true;
}

bool tcp_net_facade_imp::on_read(net_conn* pConn, const char* buff, size_t len)
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

bool tcp_net_facade_imp::on_write(net_conn* pConn, size_t len) 
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

bool tcp_net_facade_imp::on_accept(net_conn* pListen, net_conn* pNewConn, bool bSuccess/* = true*/) 
{
    if (net_conn_mgr_ && bSuccess) 
    {
#ifdef _DEBUG
        if (net_conn_mgr_->is_exist_conn(pNewConn)) 
        {
            _ASSERT(FALSE && "�ظ�������ӣ�������Ƴ���!");
        }
#endif
        net_conn_mgr_->add_conn(pNewConn);
    }

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