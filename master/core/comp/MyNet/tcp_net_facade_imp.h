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
    //��ʼ���ýӿ�
    virtual bool set_icombiner(icombiner* pCombiner) ;               //���÷ְ�����ӿ�
    virtual bool set_dispatch_handler(dispatcher_handler* pHandler) ;  //���÷��ɽӿ�,��������������������������������
    virtual bool set_net_conn_mgr(inet_conn_mgr* pMgr) ;    //���ӹ�����
    virtual bool init_client();                                      //��ʼ���ͻ���

    //////////////////////////////////////////////////////////////////////////
    //��ȡ�����ӿ�
    virtual inetpacket_mgr* get_net_packet_mgr() { return m_pNetPacketMgr;}         //��������������ӿ�
    virtual inetwork*       get_network() { return m_pINetwork; }					//���������ӿ�
    virtual inet_conn_mgr*  get_net_conn_mgr() { return m_pINetConnMgr; }	        //�������ӹ���ӿ�

    //////////////////////////////////////////////////////////////////////////
    //�¼�����ӿ�
    virtual bool add_event_handler(inet_event_handler* handler) ;
    virtual bool del_event_handler(inet_event_handler* handler) ;

protected:
    virtual bool OnConnect(net_conn* pConn, bool bConnected = true) ;
    virtual bool OnDisConnect(net_conn* pConn) ;
    virtual bool OnRead(net_conn* pConn, const char* buff, size_t len);
    virtual bool OnWrite(net_conn* pConn) ;
    virtual bool OnAccept(net_conn* pListen, net_conn* pNewConn, bool bSuccess = true) ;

protected:

protected:
    bool                    m_bInitSucc;                    //��ʾ�Ƿ��ʼ���ɹ�
    icombiner*              m_pICombiner;                   //���������ж�,����Ӧ�ò��Э��
    dispatcher_handler*     m_pIDispatcher;                 //��ǲ�ӿ�
    inetwork*               m_pINetwork;                    //����ӿ�
    inetpacket_mgr*         m_pNetPacketMgr;                //���������
    inet_conn_mgr*          m_pINetConnMgr;                 //�������ӹ�����

    auto_cs     m_cs;                //ͬ���ṹ

    typedef std::vector<inet_event_handler*> INetEventHandlerListType;
    INetEventHandlerListType m_INetEventList;    //�¼��������б�
};