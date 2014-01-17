
#pragma once

class net_conn;
class tcp_net_facade_imp;

typedef void (*conn_callback)(net_conn* pConn, void* param);

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
    virtual int  get_conn_count() = 0;
    virtual bool for_each_conn(conn_callback* callback, void* param) = 0;
};