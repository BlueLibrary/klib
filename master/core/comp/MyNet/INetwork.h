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
    virtual bool init_network(inet_tcp_handler* handler, 
        size_t thread_num = 1,
        size_t worker_num = 6) = 0; 

    ///< Ͷ��һ��д����
    virtual bool try_write(net_conn* pconn, const char* buff, size_t len) = 0;

    ///< ������ȡ�����׽ӿ�
    virtual net_conn* try_listen(USHORT uLocalPort, void* bind_key = NULL) = 0;
    
    ///< Ͷ�����ӵ�������
    virtual net_conn* try_connect(const char* addr, USHORT uport, void* bind_key = NULL) = 0;
};