#ifndef _klib_udp_socket_h
#define _klib_udp_socket_h

#include <WinSock2.h>
#include <windows.h>


#include "../third/sigslot.h"
#include "../kthread/thread.h"
#include "ip_v4.h"


namespace klib {
namespace net {


///< UDP�׽��֣����ڽ����뷢��UDP���ݰ�
class udp_socket
{
public:
	udp_socket(void);
	virtual ~udp_socket(void);

public:
    ///< ʹ��ǰ�����Ƚ��г�ʼ��
	BOOL init(USHORT uBindPort = 0);

    ///< ��ȡ�󶨵Ķ˿�
    USHORT  get_bind_port();

    ///< ��ַ����
    BOOL enable_reuse(BOOL bReuse = TRUE);

    ///< ���ù㲥
    BOOL enable_broadcast(BOOL bBroadCast = TRUE);

    ///< 
    BOOL udp_socket::enable_udpreset(BOOL bEnable = FALSE);

    ///< ���첽�ķ�ʽ����,�ᴴ���̣߳��յ���Ϣ�������Ϣ�ŵķ�ʽ����
    BOOL start_async();

    /*
    * @brief ֪ͨ��Ҫֹͣ,���ܻỨһ��ʱ�����ȴ�
    * dwWaitSec  -  ��Ҫ���������� 
    */
    BOOL stop_async(DWORD dwMilliseconds = INFINITE) ;
    
    ///< ����ʼ��
	BOOL uninit();

    ///< ��ȡ�׽���
	SOCKET& get_socket();

    ///< ����TTLֵ
    BOOL set_ttl(UINT uValue);

    ///< ����TTLֵ
    UINT get_ttl();

    ///< ����Ҫ���͵�Ŀ��ip�Ͷ˿�
	BOOL connect(const char* strSvrIp, USHORT uSvrPort);

    ///< ����ķ���Ҫ����connect�����ʹ��
	int  send(const char* buff, int iLen);

    ///< ͨ��ip�ַ�����������,�˿�Ϊ������
	BOOL send_to(const char* pszSvrIp, USHORT uSvrPort, const char* buff, int iLen); //������

    ///< ͨ���ֽ����ip��ַ��������
	BOOL send_to(ip_v4 svrIp, USHORT uSvrPort, const char* buff, int iLen); //�����ֽ���

    ///< ��������
	int recv_from(sockaddr_in* addrFrom, char* buff, int iBuffLen);

    ///< ���������ź�
    sigslot::signal5<udp_socket*, ip_v4, USHORT, char*, int> sign_msg_recv;

protected:
    ///< ������
    void work_func(void*);

private:
	BOOL		m_bInitSocket;		///< ��ʼ�׽���
	BOOL		m_bConnected;		///< udpģʽ��

	SOCKET		m_socket;			///< �׽���
	USHORT		m_uBindPort;		///< �󶨶˿�

    klib::kthread::Thread thread_;
};


}}


#endif
