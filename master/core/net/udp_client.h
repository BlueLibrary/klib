// UdpClient.h: interface for the udp_client class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPCLIENT_H__E02768E0_20F8_4E86_B807_FDEFD7BF7D67__INCLUDED_)
#define AFX_UDPCLIENT_H__E02768E0_20F8_4E86_B807_FDEFD7BF7D67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <WinSock2.h>
#include <windows.h>

#include "../kthread/thread.h"
#include <list>

namespace klib {
namespace net{

    
class udp_client;
class udp_handler {
public:
	virtual void on_msg(udp_client* client_, 
                        UINT32 uAddr, 
                        USHORT uPort, 
                        char* buff, 
                        int iLen) = 0;		// UDP��Ϣ�ص��ӿ�
};


class udp_client
{
public:
	udp_client();
	virtual ~udp_client();

public:
	virtual BOOL init(USHORT uPort = 0) ;     ///< ��ʼ���׽���
    virtual BOOL reinit();                    ///< ���³�ʼ���׽��֣���Ҫ��Ϊ�˰󶨲�ͬ�Ķ˿�
    virtual BOOL bind_port(USHORT uPort) ;           ///< �ﶨ�ض��Ķ˿�
    virtual BOOL enable_reuse(BOOL bReuse = TRUE) ;
    virtual BOOL enable_broadcast(BOOL bBroadCast = TRUE) ;
    virtual BOOL enable_udpreset(BOOL bEnable = FALSE);
	
	virtual BOOL start(BOOL bBlock);
	virtual void stop(BOOL bStop = TRUE);
	virtual BOOL is_stop() ;
	virtual void set_handler(udp_handler* handler_);

	virtual void set_svr_info(UINT32 uAddrServer, USHORT uPortServer) ;//��ַ�Ͷ˿�Ϊ������
	virtual BOOL send_to(UINT32 uAddr, USHORT uPort, const char* buff, int iLen);//��ַ�Ͷ˿�Ϊ������
	virtual BOOL send_to(const char* strAddr, USHORT uPort, const char*buff, int iLen);//�˿�Ϊ������
	virtual BOOL send_to_svr(const char* buff, int iLen);

private:
	void work_thread(void* param);

	BOOL do_server();

protected:
	//----------------------------------------------------------------------
	SOCKET  m_sock;                         ///< �׽���
	BOOL    m_bStop;                        ///< �Ƿ�ֹͣ
	BOOL	m_bSocketInit;                  ///< �׽����Ƿ��ʼ����

	//----------------------------------------------------------------------
	//���±������������ֽ���
	UINT32 m_uSvrAddr;						///< ��������ַ
	USHORT m_uSvrPort;						///< �������˿�

	udp_handler* udp_handler_;				///< UDP�¼��ص��ӿ�
    klib::kthread::Thread  thread_;         ///< �߳�
};


}}



#endif // !defined(AFX_UDPCLIENT_H__E02768E0_20F8_4E86_B807_FDEFD7BF7D67__INCLUDED_)
