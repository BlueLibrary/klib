#pragma once

#include "INetClient.h"
#include <vector>


class INetTcpClientImp : public INetTcpClient
{
public:
    INetTcpClientImp(void);
    ~INetTcpClientImp(void);

public:
    //////////////////////////////////////////////////////////////////////////
    //��ʼ���ýӿ�
    virtual bool SetICombiner(ICombiner* pCombiner) ;               //���÷ְ�����ӿ�
    virtual bool SetIDispatchHandler(IDispatchHandler* pHandler) ;  //���÷��ɽӿ�,��������������������������������
    virtual bool SetINetConnectionMgr(INetConnectionMgr* pMgr) ;    //���ӹ�����
    virtual bool InitClient();                                      //��ʼ���ͻ���

    //////////////////////////////////////////////////////////////////////////
    //��ȡ�����ӿ�
    virtual INetPacketMgr* GetNetPacketMgr() { return m_pNetPacketMgr;}       //��������������ӿ�
    virtual INetNetwork*   GetNetwork() { return m_pINetwork; }					//���������ӿ�
    virtual INetConnectionMgr* GetINetConnectionMgr() { return m_pINetConnMgr; }	//�������ӹ���ӿ�

    //////////////////////////////////////////////////////////////////////////
    //�¼�����ӿ�
    virtual bool AddEventHandler(INetEventHandler* handler) ;
    virtual bool RemoveEventHandler(INetEventHandler* handler) ;

protected:
    virtual bool OnConnect(INetConnection* pConn, bool bConnected = true) ;
    virtual bool OnDisConnect(INetConnection* pConn) ;
    virtual bool OnRead(INetConnection* pConn, const char* buff, size_t len);
    virtual bool OnWrite(INetConnection* pConn) ;
    virtual bool OnAccept(INetConnection* pListen, INetConnection* pNewConn, bool bSuccess = true) ;

protected:

protected:
    bool                m_bInitSucc;                //��ʾ�Ƿ��ʼ���ɹ�
    ICombiner*          m_pICombiner;               //���������ж�,����Ӧ�ò��Э��
    IDispatchHandler*   m_pIDispatcher;             //��ǲ�ӿ�
    INetNetwork*           m_pINetwork;                //����ӿ�
    INetPacketMgr*      m_pNetPacketMgr;            //���������
    INetConnectionMgr*  m_pINetConnMgr;             //�������ӹ�����


    auto_cs     m_cs;                //ͬ���ṹ

    typedef std::vector<INetEventHandler*> INetEventHandlerListType;
    INetEventHandlerListType m_INetEventList;    //�¼��������б�
};