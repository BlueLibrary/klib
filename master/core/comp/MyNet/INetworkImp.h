#pragma once

#include "INetwork.h"
#include "MySocket.h"

#include <list>
#include <Mswsock.h>

/*
  ���������������
*/

class Klib_OverLapped: public OVERLAPPED
{
public:
  // �������û���Ҫ����Ϣ
  bool bFixed; //��ʾ�Ƿ��ǹ̶����ڴ棬�������ͷ�

  //�ⲿʹ��
  int  operate_type;    // �ύ�Ĳ�������
  void* pvoid;          // �����������ݵ�
  DWORD dwBytesTransfered;  //�����˵�����
  WSABUF wsaBuf;      //
  char  buff[2 * 1024];			// �������ݵ�buf
  //DWORD dwRead; //��ȡ���ֽ��� //todo 

public:
  Klib_OverLapped() {
    memset(this, 0, sizeof(Klib_OverLapped));
  }
};

class INetNetworkImp : public INetNetwork
{
public:
  INetNetworkImp(void);
  ~INetNetworkImp(void);

public:
  virtual bool InitNetwork() ;   
  virtual bool SetNetEventHandler(INetEventHandler* handler) ;    //�����¼�����ӿ�
  virtual bool RunNetwork() ;   //���������-�������߳�

  virtual net_conn* PostAccept(net_conn* pListenConn) ;	//Ͷ�ݽ�������,�ɶ��Ͷ�ݣ������׽��ֱ����� CreateListenConn ����
  virtual bool PostConnection(net_conn* pConn) ;		//Ͷ����������
  virtual bool PostRead(net_conn* pConn) ;	//Ͷ�ݶ������ڽ������Ӻ�ײ��Ĭ��Ͷ��һ������
  virtual bool PostWrite(net_conn* pConn, const char* buff, size_t len) ;	//Ͷ��д����

  virtual net_conn* CreateListenConn(USHORT uLocalPort) ;	//�������ؼ����׽��֣�����һ��net_conn�ṹ�����ڽ�������
  virtual net_conn* CreateNewConn() ;			//����һ�������׽���
  virtual bool ReleaseConnection(net_conn* pConn);		//�ͷ�����

protected:
  static unsigned int WINAPI ThreadNetwork(void* param);

  void InitFixedOverlappedList(size_t nCount);    //��ʼ�̶���Overlapped�ĸ������ⲿ���ڴ治�ܱ��ͷ�
  Klib_OverLapped* GetMyOverlapped();                //�����ص��ṹ
  bool ReleaseMyOverlapped(Klib_OverLapped* pMyoverlapped);	//�ͷ��ص��ṹ

  void CheckAndDisconnect(net_conn* pConn);     //�ж����׽����ϻ���û��δ�����Ͷ���������û������Ͽ�����

private:
  HANDLE m_hIocp;   ///< ��ɶ˿ھ��
  INetEventHandler* m_INetEventHandler;   ///< �ƽ��ϲ㴦��Ľӿ�
  LPFN_ACCEPTEX m_lpfnAcceptEx;   ///< AcceptEx����ָ��
  
  typedef std::list<Klib_OverLapped*> OverLappedListType;  /// ����Klib_OverLapped����������
  OverLappedListType m_overlappedList;      /// ����������������Klib_OverLapped
  auto_cs            m_csOverlappedList;      /// ͬ������m_overlappedList

  typedef std::list<net_conn*> INetConnListType;  /// �ӿ��б����Ͷ���
  INetConnListType m_INetConnFreeList;      /// �����������ӽӿ�����
  auto_cs          m_CsNetFreeList;         /// ͬ������m_INetConnFreeList
};