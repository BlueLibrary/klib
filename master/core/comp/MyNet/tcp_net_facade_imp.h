#ifndef _klib_tcp_net_facade_imp_h
#define _klib_tcp_net_facade_imp_h

#include "tcp_net_facade.h"
#include "network_i.h"
#include <vector>

#include <pattern/object_pool.h>
using namespace klib::pattern;

class tcp_net_facade_imp : public tcp_net_facade
{
public:
    tcp_net_facade_imp(void);
    ~tcp_net_facade_imp(void);

public:
    //////////////////////////////////////////////////////////////////////////
    // ��ʼ���ýӿ�
    virtual bool set_icombiner(icombiner* pCombiner) ;                  //���÷ְ�����ӿ�
    virtual bool set_dispatch_handler(dispatcher_handler* pHandler) ;   //���÷��ɽӿ�,��������������������������������
    virtual bool init();                                         //��ʼ���ͻ���

    //////////////////////////////////////////////////////////////////////////
    // ��ȡ�����ӿ�
    virtual network_i*       get_network() { return inetwork_; }					//���������ӿ�
    virtual net_conn_mgr_i*  get_net_conn_mgr() { return inetwork_->get_net_conn_mgr(); }	        //�������ӹ���ӿ�

    //////////////////////////////////////////////////////////////////////////
    // �¼�����ӿ�
    virtual bool add_event_handler(inet_tcp_handler* handler) ;
    virtual bool del_event_handler(inet_tcp_handler* handler) ;

protected:
    //----------------------------------------------------------------------
    // ������Ϣ������
    virtual bool on_connect(net_conn_ptr pConn, bool bConnected = true) ;
    virtual bool on_disconnect(net_conn_ptr pConn) ;
    virtual bool on_read(net_conn_ptr pConn, const char* buff, size_t len);
    virtual bool on_write(net_conn_ptr pConn, size_t len) ;
    virtual bool on_accept(net_conn_ptr pListen, net_conn_ptr pNewConn, bool bSuccess = true) ;

protected:
    bool                        init_success_;                      //��ʾ�Ƿ��ʼ���ɹ�
    icombiner*                  icombiner_;                         //���������ж�,����Ӧ�ò��Э��
    dispatcher_handler*         dispatch_handler_;                  //��ǲ�ӿ�
    network_i*                   inetwork_;                         //����ӿ�

    mutex                       mutex_;                             //ͬ���ṹ

    typedef std::vector<inet_tcp_handler*> INetEventHandlerListType;
    INetEventHandlerListType    net_event_list_;                    //�¼��������б�
};

#endif
