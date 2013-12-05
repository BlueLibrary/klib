// mynet_test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <crtdbg.h>

#include <comp/MyNet/imynetdef.h>


class MyHandler : public INetEventHandler
{
public:
    MyHandler(INetTcpClient* client) {
        m_pClient = client;
    }

    virtual bool OnAccept(INetConnection* pListen, INetConnection* pNewConn, bool bSuccess) 
    {
        printf("�������ӣ���ǰ������ ��%d \r\n", m_pClient->GetINetConnectionMgr()->GetConnectionCount());
        return true;
    }

    virtual bool OnDisConnect(INetConnection* pConn)  
    {
        printf("���ӶϿ�����ǰ������ ��%d \r\n", m_pClient->GetINetConnectionMgr()->GetConnectionCount());
        // m_pClient->GetNetwork()->FreeConnection(pConn);
        return true;
    }

    virtual bool OnRead(INetConnection* pConn, const char* buff, size_t len)
    {
        printf(buff);
        return true;
    }
    virtual bool OnWrite(INetConnection* pConn) {return true;}

    virtual bool OnConnect(INetConnection* pConn, bool bConnected /* = true */) 
    {
        printf("�������ӣ���ǰ������ ��%d \r\n", m_pClient->GetINetConnectionMgr()->GetConnectionCount());

        std::string str = "GET / HTTP/1.1\r\n"
            "Host: www.baidu.com\r\n"
            "Accept: */*\r\n"
            "\r\n\r\n";

        m_pClient->GetNetwork()->PostWrite(pConn, str.c_str(), str.size());
        
        return true;
    }

protected:
    INetTcpClient* m_pClient;
};

int _tmain(int argc, _TCHAR* argv[])
{
    klib::aom::iframework* pframework = klib::aom::framework::instance();

    HRESULT hr = pframework->load_module(_T("mynet.dll"));
    _ASSERT(S_OK == hr);

    IMyNet* pNet = NULL;

    pframework->start();
    pframework->find_next_interface(IID_IMyNet, (void**)&pNet);

    INetTcpClient* pClient = pNet->CreateTcpClient();

    MyHandler thehandler(pClient);
    pClient->InitClient();
    pClient->AddEventHandler(&thehandler);

    INetConnection* pConn = pClient->GetNetwork()->CreateNewConn();
    pConn->SetPeerAddress("www.baidu.com");
    pConn->SetPeerPort(80);
    pClient->GetNetwork()->PostConnection(pConn);


    Sleep(-1);
	return 0;
}

