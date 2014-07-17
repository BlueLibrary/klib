#ifndef _klib_network_imp_h
#define _klib_network_imp_h

#include "network_i.h"
#include "net_socket.h"
#include "net_conn.h"
#include "net_conn_mgr_i.h"

#include <list>
#include <vector>
#include <Mswsock.h>

#include <kthread/thread.h>
#include <kthread/thread_local.h>

#include <pattern/object_pool.h>
#include <pattern/active_object.h>

using namespace klib::kthread;
using namespace klib::pattern;


#if (_MSC_VER == 1200)
typedef
    BOOL
    (PASCAL FAR * LPFN_CONNECTEX) (
    SOCKET s,
    const struct sockaddr FAR *name,
    int namelen,
    PVOID lpSendBuffer,
    DWORD dwSendDataLength,
    LPDWORD lpdwBytesSent,
    LPOVERLAPPED lpOverlapped
    );

#define WSAID_CONNECTEX \
{0x25a207b9,0xddf3,0x4660,{0x8e,0xe9,0x76,0xe5,0x8c,0x74,0x06,0x3e}}

#endif

typedef std::vector<Thread> thread_vec_type;

#define  RECV_BUFF_LEN   (2*1024)
//----------------------------------------------------------------------
// ���������
//  ���������������
class net_overLapped: public OVERLAPPED
{
public:
    //�ⲿʹ��
    em_operation_type   operate_type_;              ///< �ύ�Ĳ�������
    net_conn_ptr        op_conn_;                   ///< ���Ӷ���
    net_conn_ptr        listen_conn_;               ///< ���������ӣ��п���Ϊ��
    DWORD               transfer_bytes_;            ///< �����˵�����
    WSABUF              wsaBuf_;                    ///< ����������
    char                recv_buff_[RECV_BUFF_LEN];		///< �������ݵ�buf
    
public:
    net_overLapped() : /*pend_data_(0),*/ 
        operate_type_(OP_NONE),
        transfer_bytes_(0)
    {
    }
};

enum contex_type
{
    context_recv_ctx,
    context_send_ctx,
};

class network_imp;
class worker_context
{
public:
    contex_type                 ctx_type_;
    network_imp*                network_;
    std::weak_ptr<net_conn>     pConn;
    union
    {
        struct recv_info
        {
            net_overLapped*     lpOverlapped;
            DWORD               dwByteTransfered;
            BOOL                bResult;
        } recv_info;

        struct send_info
        {
            char*               buff_ptr_;
            size_t              buff_len_;
        } send_info;
    };
};

class network_worker : public active_object<worker_context*>
{
public:
    virtual bool execute(worker_context*& ctx);

protected:
    bool on_recv_ctx(worker_context*& ctx);
    bool on_send_ctx(worker_context*& ctx);
};


/// �����ʵ��
class network_imp : public network_i
{
    friend network_worker;

public:
    network_imp(void);
    ~network_imp(void);

public:
    //----------------------------------------------------------------------
    // �ӿ�ʵ��
    virtual bool init_network(inet_tcp_handler* handler, 
        size_t thread_num = 1,
        size_t worker_num = 7) ;

    virtual net_conn_mgr_i* get_net_conn_mgr() { return net_conn_mgr_i_.get(); }

    virtual bool try_write(net_conn_ptr pconn, const char* buff, size_t len);          ///< ���Է�������

    virtual net_conn_ptr try_listen(USHORT local_port, void* bind_key = NULL) ;                                  ///< �����˿�
    virtual net_conn_ptr try_connect(const char* addr, USHORT uport, void* bind_key) ;    ///< Ͷ�����ӵ�������

protected:
    //----------------------------------------------------------------------
    // �ڲ��첽Ͷ�ݺ���
    virtual net_conn_ptr post_accept(net_conn_ptr pListenConn) ;	                        ///< Ͷ�ݽ�������,�ɶ��Ͷ�ݣ������׽��ֱ����� create_listen_conn ����
    virtual bool post_connection(net_conn_ptr pConn) ;		                            ///< Ͷ����������
    virtual bool post_read(net_conn_ptr pConn) ;	                                    ///< Ͷ�ݶ������ڽ������Ӻ�ײ��Ĭ��Ͷ��һ������
    //virtual bool post_write(net_conn_ptr pConn, char* buff, size_t len) ;	        ///< Ͷ��д����
    virtual bool post_placement_write(net_conn_ptr pConn, char* buff, size_t len) ;	///< Ͷ��д����

protected:
    //----------------------------------------------------------------------
    // ��������¼�
    void on_read(net_conn_ptr, const char* buff, DWORD dwtransfer);
    void on_write(net_conn_ptr, DWORD dwByteTransfered);
    void on_accept(net_conn_ptr listen_conn, net_conn_ptr accept_conn);
    void on_connect(net_conn_ptr, bool bsuccess);

protected:
    //----------------------------------------------------------------------
    // net_conn ��Դ���� 
    virtual net_conn_ptr create_listen_conn(USHORT uLocalPort) ;	            ///< �������ؼ����׽��֣�����һ��net_conn�ṹ�����ڽ�������
    net_overLapped* get_net_overlapped();                                   ///< �����ص��ṹ

    //----------------------------------------------------------------------
    // ��ʱ�����

    //----------------------------------------------------------------------
    // ��ʼ��
    bool init_threads(size_t thread_num) ;                                  ///< ���������-�������߳�
    bool init_workers(size_t worker_num);                                   ///< ��ʼ����������
    network_worker* get_worker(void*);                                     ///< ��ȡworker

protected:
    void worker_thread_();                                       ///< �����߳�
    void check_and_disconnect(net_conn_ptr pConn);                             ///< �ж����׽����ϻ���û��δ�����Ͷ���������û������Ͽ�����

private:
    // handle
    HANDLE                  hiocp_;                                         ///< ��ɶ˿ھ��
    inet_tcp_handler*       net_event_handler_;                             ///< �ƽ��ϲ㴦��Ľӿ�
    LPFN_ACCEPTEX           m_lpfnAcceptEx;                                 ///< AcceptEx����ָ��
    LPFN_CONNECTEX          m_lpfnConnectEx;
    bool                    m_bstop;

    thread_vec_type         work_threads_;
    size_t                  thread_num_;

    active_obj_mgr<network_worker> worker_mgr_;

    CObjectPool<net_overLapped, 2000, 2000>     net_overlapped_pool_;       // overlapped 
    
    std::shared_ptr<net_conn_mgr_i> net_conn_mgr_i_;
};

#endif
