#pragma once

#include "net_socket.h"

class net_conn;
class inet_tcp_handler;

///< ����ӿ���
class inetwork
{
public:
    inetwork(void) {}
    virtual ~inetwork(void) {}

public:
    ///< ��ʼ�������ӿڣ���tcp_net_facade����
    virtual bool init_network(inet_tcp_handler* handler) = 0; 

    ///< ��������㣬�����߳���Щ
    virtual bool run_network() = 0;

    ///< Ͷ�ݽ�������
    /// @usage   net_conn* pListenConn = pClient->get_network()->CreateNewConnection();
    /// pListenConn->set_local_port(7000);
    /// pClient->get_network()->InitListenConnection(pListenConn);
    /// pClient->get_network()->post_accept(pListenConn);
    virtual net_conn* post_accept(net_conn* plistenConn) = 0;
    
    ///< �ڽ�����������Ͷ��һ��������
    /// @usage   net_conn* pMyConn = pClient->get_network()->CreateNewConnection();
    /// pClient->get_network()->post_read(pMyConn);
    virtual bool post_read(net_conn* pConn) = 0;

    ///< �ڽ�����������Ͷ��һ��д��������
    /// @usage   net_conn* pMyConn = pClient->get_network()->CreateNewConnection();
    /// pClient->get_network()->post_write(pMyConn, buff, 1024);
    virtual bool post_write(net_conn* pConn, const char* buff, size_t len) = 0;

    ///< ������ȡ�����׽ӿ�
    virtual net_conn* try_listen(USHORT uLocalPort) = 0;
    
    ///< Ͷ�����ӵ�������
    virtual net_conn* try_connect(const char* addr, USHORT uport, void* bind_key = NULL) = 0;
};