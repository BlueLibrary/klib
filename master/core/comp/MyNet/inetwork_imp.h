#pragma once

#include "INetwork.h"
#include "net_socket.h"

#include <list>
#include <vector>
#include <Mswsock.h>

#include <core/timer_mgr.h>
#include <kthread/thread.h>
#include <kthread/thread_local.h>

using namespace klib::kthread;
typedef std::vector<Thread> thread_vec_type;

//----------------------------------------------------------------------
// ���������
//  ���������������
class net_overLapped: public OVERLAPPED
{
public:
    ///<  �������û���Ҫ����Ϣ
    bool bFixed;                        ///< ��ʾ�Ƿ��ǹ̶����ڴ棬�������ͷ�

    //�ⲿʹ��
    int       operate_type_;            ///< �ύ�Ĳ�������
    void*     pend_data_;               ///< �����������ݵ�
    DWORD     transfer_bytes_;          ///< �����˵�����
    WSABUF    wsaBuf_;                  ///< ����������
    char      recv_buff_[2 * 1024];		///< �������ݵ�buf
    
public:
    net_overLapped() : bFixed(false), pend_data_(0), transfer_bytes_(0)
    {
    }
};


/// �����ʵ��
class inetwork_imp : public inetwork
{
public:
    inetwork_imp(void);
    ~inetwork_imp(void);

public:
    //----------------------------------------------------------------------
    // �ӿ�ʵ��
    virtual bool init_network(inet_tcp_handler* handler, size_t thread_num = 1) ;   
    virtual bool run_network() ;                 ///< ���������-�������߳�

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
    virtual net_conn* create_conn() ;			                            ///< ����һ�������׽���
    virtual bool release_conn(net_conn* pConn);		                        ///< �ͷ�����

    //----------------------------------------------------------------------
    // overlapped ��Դ����
    void init_fixed_overlapped_list(size_t nCount);                         ///< ��ʼ�̶���Overlapped�ĸ������ⲿ���ڴ治�ܱ��ͷ�
    net_overLapped* get_net_overlapped();                                   ///< �����ص��ṹ
    bool release_net_overlapped(net_overLapped* pMyoverlapped);	            ///< �ͷ��ص��ṹ

    //----------------------------------------------------------------------
    // ��ʱ�����

protected:
    void worker_thread_(void* param);                 ///< �����߳�
    void check_and_disconnect(net_conn* pConn);                             ///< �ж����׽����ϻ���û��δ�����Ͷ���������û������Ͽ�����

private:
    // handle
    HANDLE                  hiocp_;                                         ///< ��ɶ˿ھ��
    inet_tcp_handler*       net_event_handler_;                             ///< �ƽ��ϲ㴦��Ľӿ�
    LPFN_ACCEPTEX           m_lpfnAcceptEx;                                 ///< AcceptEx����ָ��
    thread_vec_type         work_threads_;
    size_t                  thread_num_;

    // overlapped 
    typedef std::list<net_overLapped*> OverLappedListType;                  ///< ����net_overLapped����������
    OverLappedListType      overlapped_list_;                               ///< ����������������net_overLapped
    mutex                   overlapped_list_mutex_;                         ///< ͬ������overlapped_list_

    // net_conn
    typedef std::list<net_conn*> INetConnListType;                          ///< �ӿ��б����Ͷ���
    INetConnListType        free_net_conn_list_;                            ///< �����������ӽӿ�����
    mutex                   free_net_conn_mutex_;                           ///< ͬ������free_net_conn_list_

    klib::core::timer_mgr   tmr_mgr_;
};