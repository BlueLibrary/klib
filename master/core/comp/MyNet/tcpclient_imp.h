#pragma once

#include "tcpclient.h"
#include <vector>


class tcpclient_imp : public tcpclient
{
public:
    tcpclient_imp(void);
    ~tcpclient_imp(void);

public:
    //////////////////////////////////////////////////////////////////////////
    //��ʼ���ýӿ�
    virtual bool SetICombiner(icombiner* pCombiner) ;               //���÷ְ�����ӿ�
    virtual bool SetIDispatchHandler(dispatcher_handler* pHandler) ;  //���÷��ɽӿ�,��������������������������������
    virtual bool SetINetConnectionMgr(inet_conn_mgr* pMgr) ;    //���ӹ�����
    virtual bool InitClient();                                      //��ʼ���ͻ���

    //////////////////////////////////////////////////////////////////////////
    //��ȡ�����ӿ�
    virtual inetpacket_mgr* GetNetPacketMgr() { return m_pNetPacketMgr;}       //��������������ӿ�
    virtual inetwork*   GetNetwork() { return m_pINetwork; }					//���������ӿ�
    virtual inet_conn_mgr* GetINetConnectionMgr() { return m_pINetConnMgr; }	//�������ӹ���ӿ�

    //////////////////////////////////////////////////////////////////////////
    //�¼�����ӿ�
    virtual bool AddEventHandler(inet_event_handler* handler) ;
    virtual bool RemoveEventHandler(inet_event_handler* handler) ;

protected:
    virtual bool OnConnect(net_conn* pConn, bool bConnected = true) ;
    virtual bool OnDisConnect(net_conn* pConn) ;
    virtual bool OnRead(net_conn* pConn, const char* buff, size_t len);
    virtual bool OnWrite(net_conn* pConn) ;
    virtual bool OnAccept(net_conn* pListen, net_conn* pNewConn, bool bSuccess = true) ;

protected:

protected:
    bool                m_bInitSucc;                //��ʾ�Ƿ��ʼ���ɹ�
    icombiner*          m_pICombiner;               //���������ж�,����Ӧ�ò��Э��
    dispatcher_handler*   m_pIDispatcher;             //��ǲ�ӿ�
    inetwork*           m_pINetwork;                //����ӿ�
    inetpacket_mgr*      m_pNetPacketMgr;            //���������
    inet_conn_mgr*  m_pINetConnMgr;             //�������ӹ�����


    auto_cs     m_cs;                //ͬ���ṹ

    typedef std::vector<inet_event_handler*> INetEventHandlerListType;
    INetEventHandlerListType m_INetEventList;    //�¼��������б�
};