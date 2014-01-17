
#pragma once

class net_conn;
class tcp_net_facade_imp;


#include <functional>
typedef std::function<void(const net_conn*&, const void*& v)> conn_callback;

//----------------------------------------------------------------------
////< �������ӹ�����
class inet_conn_mgr
{
    friend tcp_net_facade_imp;

protected:
    // ֻ�����ض��������
    virtual bool add_conn(net_conn* pConn) = 0;
    virtual bool del_conn(net_conn* pConn) = 0;

public:
    // �����Է���
    virtual bool is_exist_conn(net_conn* pConn) = 0;
    virtual size_t get_conn_count() = 0;
};