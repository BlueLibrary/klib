#pragma once

#include "tcp_net_facade.h"
#include <vector>


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
    virtual bool set_net_conn_mgr(inet_conn_mgr* pMgr) ;                //���ӹ�����
    virtual bool init();                                         //��ʼ���ͻ���

    //////////////////////////////////////////////////////////////////////////
    // ��ȡ�����ӿ�
    virtual inetpacket_mgr* get_net_packet_mgr() { return net_packet_mgr_;}         //��������������ӿ�
    virtual inetwork*       get_network() { return inetwork_; }					//���������ӿ�
    virtual inet_conn_mgr*  get_net_conn_mgr() { return net_conn_mgr_; }	        //�������ӹ���ӿ�

    //////////////////////////////////////////////////////////////////////////
    // �¼�����ӿ�
    virtual bool add_event_handler(inet_tcp_handler* handler) ;
    virtual bool del_event_handler(inet_tcp_handler* handler) ;

protected:
    //----------------------------------------------------------------------
    // ������Ϣ������
    virtual bool on_connect(net_conn* pConn, bool bConnected = true) ;
    virtual bool on_disconnect(net_conn* pConn) ;
    virtual bool on_read(net_conn* pConn, const char* buff, size_t len);
    virtual bool on_write(net_conn* pConn, size_t len) ;
    virtual bool on_accept(net_conn* pListen, net_conn* pNewConn, bool bSuccess = true) ;

protected:
    bool                        init_success_;                      //��ʾ�Ƿ��ʼ���ɹ�
    icombiner*                  icombiner_;                         //���������ж�,����Ӧ�ò��Э��
    dispatcher_handler*         dispatch_handler_;                  //��ǲ�ӿ�
    inetwork*                   inetwork_;                          //����ӿ�
    inetpacket_mgr*             net_packet_mgr_;                    //���������
    inet_conn_mgr*              net_conn_mgr_;                      //�������ӹ�����

    mutex                       mutex_;                             //ͬ���ṹ

    typedef std::vector<inet_tcp_handler*> INetEventHandlerListType;
    INetEventHandlerListType    net_event_list_;                    //�¼��������б�
};