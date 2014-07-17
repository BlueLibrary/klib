
#ifndef _klib_net_conn_mgr_i_h
#define _klib_net_conn_mgr_i_h

class net_conn;
class network_imp;


#include <functional>
typedef std::function<void(const net_conn_weak_ptr&, const void*& v)> conn_callback;

//----------------------------------------------------------------------
////< �������ӹ�����
class net_conn_mgr_i
{
    friend network_imp;

public:
    // �����Է���
    virtual bool is_exist_conn(const net_conn_weak_ptr& pConn) = 0;
    virtual size_t get_conn_count() = 0;
    virtual bool set_conn_timeout(size_t tm_seconds) = 0; ///< �������ӳ�ʱʱ��

protected:
    // ֻ�����ض��������
    virtual bool add_conn(net_conn_weak_ptr pConn) = 0;
    virtual bool del_conn(net_conn_weak_ptr pConn) = 0;
};


#endif
