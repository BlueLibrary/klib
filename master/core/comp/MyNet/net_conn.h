#pragma once
#pragma warning(disable: 4996)

#include "net_socket.h"
#include <string>
#include <list>

#include <kthread/auto_lock.h>
#include <io/mem_seg_stream.h>

using namespace klib::kthread;


#define  MAX_CLIENT_BUFF_LEN 5*1024

//------------------------------------------------------------------------
// ��������
enum em_operation_type
{
    OP_NONE,
    OP_ACCEPT,              ///< ������������
    OP_READ,                ///< ������
    OP_WRITE,               ///< д����
    OP_CONNECT,             ///< ��������
};

typedef klib::io::mem_seg_stream<2048> net_stream_type;

class net_packet;
class tcp_net_facade_imp;
class inetwork_imp;

//----------------------------------------------------------------------
///< ����������,��Ҫͬ�������ⲿ����
class net_conn
{
    friend tcp_net_facade_imp;
    friend inetwork_imp;
public:
    net_conn(void);
    ~net_conn(void);

    typedef void* bind_key_type;

public:
    // ��ȡ�����Ϣ
    inline net_socket& get_socket() { return m_socket; }
    bool init_peer_info();                                      // ��ʼ�Զ���Ϣ��ͨ��getpeername����ȡ
    void set_peer_addr_str(const char* straddr) {strncpy(peer_addr_str_, straddr, sizeof(peer_addr_str_) - 1);}                  // ����ip��ַ
    char* get_peer_addr_str() ;

    inline void   set_peer_port(USHORT port) { peer_port_ = port; }
    inline USHORT get_peer_port() { return peer_port_; }
    inline void   set_local_port(USHORT port) { local_port_ = port; }
    inline USHORT get_local_port() { return local_port_; }

    // �Ͽ�����
    void dis_connect() ;

    // �������Ĵ���
    const net_stream_type& get_recv_stream() { return recv_stream_; }
    bool    read_recv_stream(char* buff, size_t len);
    size_t  get_recv_length();

    // �������Ĵ���
    const net_stream_type& get_send_stream() { return send_stream_; }
    bool    write_send_stream(const char* buff, size_t len) ;
    bool    mark_send_stream(size_t len);
    size_t  get_send_length();

    // ������
    inline void  set_bind_key(void* key) { bind_key_ = key; }
    inline void* get_bind_key() { return bind_key_; }

    // �ṩ���ⲿ������������ر���Ҫ�õ����ĳ���
    mutex& get_lock() { return mutex_; }
    void lock() { mutex_.lock(); }
    void unlock(){ mutex_.unlock() ;}

    // ��ȡ����ͳ��
    inline size_t get_writed_bytes()               { return writed_bytes_; }
    inline size_t get_readed_bytes()               { return readed_bytes_; }

protected:
    bool write_recv_stream(const char* buff, size_t len) ;
    bool read_send_stream(char* buff, size_t len);

    // ʱ�����
    inline DWORD get_last_active_tm() { return last_active_tm_; }
    void upsate_last_active_tm() ;

    // Ͷ�ݼ���
    void inc_post_read_count();
    void dec_post_read_count();
    USHORT get_post_read_count();
    void inc_post_write_count();
    void dec_post_write_count();
    USHORT get_post_write_count();

    // �Ƿ�ر�״̬
    inline bool get_is_closing() { return is_closing_; }
    inline void set_is_closing(bool bClose = TRUE) { is_closing_ = bClose; }

    // ��������ͳ��
    inline size_t add_rwited_bytes(size_t dwBytes) { return (writed_bytes_+=dwBytes); }
    inline size_t add_readed_bytes(size_t dwBytes) { return readed_bytes_ += dwBytes; }

protected:
    net_socket m_socket;		            // �׽���
    DWORD     last_active_tm_;	            // ����Ծʱ��

    USHORT    peer_port_;		            // �Զ˶˿�,������
    DWORD     peer_addr_dw_;		        // �Զ˵�ַ�������ֽ���
    char      peer_addr_str_[50];		    // �ַ�����ַ

    USHORT    local_port_;                  // ���ض˿ڣ��������õ�
    USHORT    post_read_count_;             // Ͷ�ݽ��յ�����
    USHORT    post_write_count_;            // Ͷ�ݷ��͵�����
    bool      is_closing_;                  // ָʾ�Ƿ����ڹر�
    void*     bind_key_;                    // �󶨵ļ�ֵ

    size_t    writed_bytes_;                // ����˶����ֽ���
    size_t    readed_bytes_;                // ���յ��˶����ֽ���

    mutex     mutex_;                       // �ٽ����������ڻ������ݵķ���

    net_stream_type  recv_stream_;          // ������
    net_stream_type  send_stream_;          // ������
};