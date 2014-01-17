#pragma once
#pragma warning(disable: 4996)

#include "MySocket.h"
#include <string>
#include <list>

#include <kthread/auto_lock.h>
#include <io/mem_seg_stream.h>

using namespace klib::kthread;


#define  MAX_CLIENT_BUFF_LEN 5*1024

//------------------------------------------------------------------------
// ��������
enum emOperationType
{
    OP_NONE,
    OP_ACCEPT,
    OP_READ,
    OP_WRITE,
    OP_CONNECT,
};

typedef klib::io::mem_seg_stream<2048> net_stream_type;

class NetPacket;
class INetConnection
{
public:
    INetConnection(void);
    ~INetConnection(void);

public:
    inline NetSocket& get_socket() { return m_socket; }
    bool init_peer_info();  // ��ʼ�Զ���Ϣ��ͨ��getpeername����ȡ
    inline void set_peer_addr(const char* straddr) { strncpy(m_strAddress, straddr, sizeof(m_strAddress)-1); }
    char* get_peer_addr() 
    {
        if (m_strAddress[0] == '\0') {
            set_peer_addr(inet_ntoa(*(in_addr*) &m_PeerAddr));
        }
        return m_strAddress;
    }

    inline void set_peer_port(USHORT port) { m_PeerPort = port; }
    inline USHORT get_peer_port() { return m_PeerPort; }
    inline void set_local_port(USHORT port) { m_LocalPort = port; }
    inline USHORT get_local_port() { return m_LocalPort; }

    inline void dis_connect() {  closesocket(m_socket); m_socket = INVALID_SOCKET; }

    const net_stream_type& get_recv_stream() { return recv_stream_; }
    bool write_recv_stream(const char* buff, size_t len) ;
    bool read_recv_stream(char* buff, size_t len);
    size_t get_recv_length();

    inline DWORD GetLastActiveTimestamp() { return m_tLastActive; }
    void UpdateLastActiveTimestamp() ;

    void AddPostReadCount();
    void DecPostReadCount();
    USHORT GetPostReadCount();
    void AddPostWriteCount();
    void DecPostWriteCount();
    USHORT GetPostWriteCount();

    inline BOOL get_is_closing() { return m_bClosing; }
    inline void set_is_closing(BOOL bClose = TRUE) { m_bClosing = bClose; }

    inline void set_bind_key(void* key) { bind_key_ = key; }
    inline void* get_bind_key() { return bind_key_; }

    inline DWORD GetBytesWrite() { return m_dwBytesWrite; }
    inline DWORD AddBytesWrite(DWORD dwBytes) { return (m_dwBytesWrite+=dwBytes); }
    inline DWORD GetBytesRead() { return m_dwBytesRead; }
    inline DWORD AddBytesRead(DWORD dwBytes) { return m_dwBytesRead += dwBytes; }

    void lock() { mutex_.lock(); }
    void unlock(){ mutex_.unlock() ;}

protected:
    NetSocket m_socket;		//�׽���
    DWORD     m_tLastActive;	//����Ծʱ��

    USHORT    m_PeerPort;		//�Զ˶˿�,������
    DWORD     m_PeerAddr;		//�Զ˵�ַ�������ֽ���

    USHORT    m_LocalPort;            //���ض˿ڣ��������õ�
    USHORT    m_PostReadCount;        //Ͷ�ݽ��յ�����
    USHORT    m_PostWriteCount;       //Ͷ�ݷ��͵�����
    BOOL      m_bClosing;             //ָʾ�Ƿ����ڹر�
    void*     bind_key_;               //�󶨵ļ�ֵ
    DWORD     m_dwBytesWrite;         //����˶����ֽ���
    DWORD     m_dwBytesRead;          //���յ��˶����ֽ���
    mutex     mutex_;                 //�ٽ����������ڻ������ݵķ���
    char      m_strAddress[50];		//�ַ�����ַ
    int       datalen;					    //�������б������ݵĳ���

    net_stream_type  recv_stream_;
    net_stream_type  send_stream_;
};