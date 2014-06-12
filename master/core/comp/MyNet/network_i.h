#ifndef _klib_network_i_h
#define _klib_network_i_h

#include "net_conn.h"
#include "net_socket.h"

class inet_tcp_handler;
class net_conn_mgr_i;

///< ����ӿ���
class network_i
{
public:
    network_i(void) {}
    virtual ~network_i(void) {}

public:
    ///< ��ʼ�������ӿڣ���tcp_net_facade����
    virtual bool init_network(inet_tcp_handler* handler, 
        size_t thread_num = 1,
        size_t worker_num = 6) = 0; 

    virtual net_conn_mgr_i* get_net_conn_mgr() = 0;

    ///< Ͷ��һ��д����
    virtual bool try_write(net_conn_ptr pconn, const char* buff, size_t len) = 0;

    ///< ������ȡ�����׽ӿ�
    virtual net_conn_ptr try_listen(USHORT uLocalPort, void* bind_key = NULL) = 0;
    
    ///< Ͷ�����ӵ�������
    virtual net_conn_ptr try_connect(const char* addr, USHORT uport, void* bind_key = NULL) = 0;
};

#endif
