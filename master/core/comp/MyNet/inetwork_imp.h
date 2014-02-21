#pragma once

#include "INetwork.h"
#include "net_socket.h"

#include <list>
#include <vector>
#include <Mswsock.h>

#include <kthread/thread.h>
#include <kthread/thread_local.h>

#include <pattern/object_pool.h>
#include <pattern/active_object.h>

using namespace klib::kthread;
using namespace klib::pattern;

typedef std::vector<Thread> thread_vec_type;

//----------------------------------------------------------------------
// ���������
//  ���������������
class net_overLapped: public OVERLAPPED
{
public:
    //�ⲿʹ��
    int       operate_type_;            ///< �ύ�Ĳ�������
    void*     pend_data_;               ///< �����������ݵ�
    DWORD     transfer_bytes_;          ///< �����˵�����
    WSABUF    wsaBuf_;                  ///< ����������
    char      recv_buff_[2 * 1024];		///< �������ݵ�buf
    
public:
    net_overLapped() : pend_data_(0), transfer_bytes_(0)
    {
    }
};

class inetwork_imp;
class worker_context
{
public:
    inetwork_imp*       network_;
    net_overLapped*     lpOverlapped;
    DWORD		        dwByteTransfered;
    net_conn*           pConn;
    BOOL                bResult;
};

class network_worker : public active_object<worker_context*>
{
public:
     virtual bool execute(worker_context*& ctx);
};


/// �����ʵ��
class inetwork_imp : public inetwork
{
    friend network_worker;

public:
    inetwork_imp(void);
    ~inetwork_imp(void);

public:
    //----------------------------------------------------------------------
    // �ӿ�ʵ��
    virtual bool init_network(inet_tcp_handler* handler, 
        size_t thread_num = 1,
        size_t worker_num = 7) ;

    virtual bool try_write(net_conn* pconn, const char* buff, size_t len);          ///< ���Է�������
    virtual bool try_read(net_conn* pConn) ;                                        ///< ��

    virtual net_conn* try_listen(USHORT local_port) ;                                  ///< �����˿�
    virtual net_conn* try_connect(const char* addr, USHORT uport, void* bind_key) ;    ///< Ͷ�����ӵ�������

protected:
    //----------------------------------------------------------------------
    // �ڲ��첽Ͷ�ݺ���
    virtual net_conn* post_accept(net_conn* pListenConn) ;	                        ///< Ͷ�ݽ�������,�ɶ��Ͷ�ݣ������׽��ֱ����� create_listen_conn ����
    virtual bool post_connection(net_conn* pConn) ;		                            ///< Ͷ����������
    virtual bool post_read(net_conn* pConn) ;	                                    ///< Ͷ�ݶ������ڽ������Ӻ�ײ��Ĭ��Ͷ��һ������
    //virtual bool post_write(net_conn* pConn, char* buff, size_t len) ;	        ///< Ͷ��д����
    virtual bool post_placement_write(net_conn* pConn, char* buff, size_t len) ;	///< Ͷ��д����

protected:
    //----------------------------------------------------------------------
    // ��������¼�
    void on_read(net_conn*, const char* buff, DWORD dwtransfer);
    void on_write(net_conn*, DWORD dwByteTransfered);
    void on_accept(net_conn* listen_conn, net_conn* accept_conn);
    void on_connect(net_conn*, bool bsuccess);

protected:
    //----------------------------------------------------------------------
    // net_conn ��Դ���� 
    virtual net_conn* create_listen_conn(USHORT uLocalPort) ;	            ///< �������ؼ����׽��֣�����һ��net_conn�ṹ�����ڽ�������
    net_overLapped* get_net_overlapped();                                   ///< �����ص��ṹ

    //----------------------------------------------------------------------
    // ��ʱ�����

    //----------------------------------------------------------------------
    // 
    bool init_threads(size_t thread_num) ;                 ///< ���������-�������߳�
    bool init_workers(size_t worker_num);

    network_worker* get_workder(void*);

protected:
    void worker_thread_(void* param);                 ///< �����߳�
    void check_and_disconnect(net_conn* pConn);                             ///< �ж����׽����ϻ���û��δ�����Ͷ���������û������Ͽ�����

private:
    // handle
    HANDLE                  hiocp_;                                         ///< ��ɶ˿ھ��
    inet_tcp_handler*       net_event_handler_;                             ///< �ƽ��ϲ㴦��Ľӿ�
    LPFN_ACCEPTEX           m_lpfnAcceptEx;                                 ///< AcceptEx����ָ��
    bool                    m_bstop;

    thread_vec_type         work_threads_;
    size_t                  thread_num_;

    network_worker*         worker_arr_;
    size_t                  worker_num_;

    CObjectPool<net_overLapped, 1000, 1000>     net_overlapped_pool_;       // overlapped 
    CObjectPool<net_conn, 1000, 1000>           net_conn_pool_;             // net_conn
    
};