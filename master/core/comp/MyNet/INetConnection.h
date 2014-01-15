#pragma once
#pragma warning(disable: 4996)

#include "MySocket.h"
#include <string>
#include <list>
#include <kthread/auto_lock.h>

using namespace klib::kthread;


#define  MAX_CLIENT_BUFF_LEN 5*1024

enum emOperationType
{
  OP_NONE,
  OP_ACCEPT,
  OP_READ,
  OP_WRITE,
  OP_CONNECT,
};


class NetPacket;
class INetConnection
{
public:
  INetConnection(void);
  ~INetConnection(void);
  
public:
  inline NetSocket& GetSocket() { return m_socket; }
  bool InitPeerInfo();  // ��ʼ�Զ���Ϣ��ͨ��getpeername����ȡ
  inline void SetPeerAddress(const char* straddr) { strncpy(m_strAddress, straddr, sizeof(m_strAddress)-1); }
  char* GetPeerAddress() 
  {
	  if (m_strAddress[0] == '\0') {
		  SetPeerAddress(inet_ntoa(*(in_addr*) &m_PeerAddr));
	  }
	  return m_strAddress;
  }

  inline void SetPeerPort(USHORT port) { m_PeerPort = port; }
  inline USHORT GetPeerPort() { return m_PeerPort; }
  inline void SetLocalPort(USHORT port) { m_LocalPort = port; }
  inline USHORT GetLocalPort() { return m_LocalPort; }

  inline void DisConnect() {  closesocket(m_socket); m_socket = INVALID_SOCKET; }

  bool AddStream(const char* buff, int len) ;
  bool GetStream(char* buff, int len);
  inline char* GetBuff() { return mbuff; }
  inline int  GetDataLen() {  return datalen; }

  inline DWORD GetLastActiveTimestamp() { return m_tLastActive; }
  void UpdateLastActiveTimestamp() ;

  void AddPostReadCount();
  void DecPostReadCount();
  USHORT GetPostReadCount();
  void AddPostWriteCount();
  void DecPostWriteCount();
  USHORT GetPostWriteCount();

  inline BOOL GetIsClosing() { return m_bClosing; }
  inline void SetIsClosing(BOOL bClose = TRUE) { m_bClosing = bClose; }

  inline void SetKey(DWORD dwKey) { m_dwKey = dwKey; }
  inline DWORD GetKey() { return m_dwKey; }

  inline DWORD GetBytesWrite() { return m_dwBytesWrite; }
  inline DWORD AddBytesWrite(DWORD dwBytes) { return (m_dwBytesWrite+=dwBytes); }
  inline DWORD GetBytesRead() { return m_dwBytesRead; }
  inline DWORD AddBytesRead(DWORD dwBytes) { return m_dwBytesRead += dwBytes; }

  void lock() { mutex_.lock(); }
  void unlock(){ mutex_.unlock() ;}

protected:
  NetSocket m_socket;		//�׽���
  DWORD     m_tLastActive;		//����Ծʱ��

  USHORT m_PeerPort;		//�Զ˶˿�,������
  DWORD  m_PeerAddr;		//�Զ˵�ַ�������ֽ���

  USHORT m_LocalPort;     //���ض˿ڣ��������õ�
  USHORT  m_PostReadCount;       //Ͷ�ݽ��յ�����
  USHORT  m_PostWriteCount;       //Ͷ�ݷ��͵�����
  BOOL   m_bClosing;            //ָʾ�Ƿ����ڹر�
  DWORD  m_dwKey;           //�󶨵ļ�ֵ
  DWORD  m_dwBytesWrite;     //����˶����ֽ���
  DWORD  m_dwBytesRead;     //���յ��˶����ֽ���
  mutex  mutex_;        //�ٽ����������ڻ������ݵķ���
  char   m_strAddress[50];		//�ַ�����ַ
  int datalen;					//�������б������ݵĳ���
  char  mbuff[MAX_CLIENT_BUFF_LEN];		//�����ӵ����ݻ�����
};