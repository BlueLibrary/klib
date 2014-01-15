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

  virtual INetConnection* PostAccept(INetConnection* pListenConn) ;	//Ͷ�ݽ�������,�ɶ��Ͷ�ݣ������׽��ֱ����� CreateListenConn ����
  virtual bool PostConnection(INetConnection* pConn) ;		//Ͷ����������
  virtual bool PostRead(INetConnection* pConn) ;	//Ͷ�ݶ������ڽ������Ӻ�ײ��Ĭ��Ͷ��һ������
  virtual bool PostWrite(INetConnection* pConn, const char* buff, size_t len) ;	//Ͷ��д����

  virtual INetConnection* CreateListenConn(USHORT uLocalPort) ;	//�������ؼ����׽��֣�����һ��INetConnection�ṹ�����ڽ�������
  virtual INetConnection* CreateNewConn() ;			//����һ�������׽���
  virtual bool ReleaseConnection(INetConnection* pConn);		//�ͷ�����

protected:
  static unsigned int WINAPI ThreadNetwork(void* param);

  void InitFixedOverlappedList(size_t nCount);    //��ʼ�̶���Overlapped�ĸ������ⲿ���ڴ治�ܱ��ͷ�
  Klib_OverLapped* GetMyOverlapped();                //�����ص��ṹ
  bool ReleaseMyOverlapped(Klib_OverLapped* pMyoverlapped);	//�ͷ��ص��ṹ

  void CheckAndDisconnect(INetConnection* pConn);     //�ж����׽����ϻ���û��δ�����Ͷ���������û������Ͽ�����

private:
  HANDLE m_hIocp;   ///< ��ɶ˿ھ��
  INetEventHandler* m_INetEventHandler;   ///< �ƽ��ϲ㴦��Ľӿ�
  LPFN_ACCEPTEX m_lpfnAcceptEx;   ///< AcceptEx����ָ��
  
  typedef std::list<Klib_OverLapped*> OverLappedListType;  /// ����Klib_OverLapped����������
  OverLappedListType m_overlappedList;      /// ����������������Klib_OverLapped
  auto_cs            m_csOverlappedList;      /// ͬ������m_overlappedList

  typedef std::list<INetConnection*> INetConnListType;  /// �ӿ��б����Ͷ���
  INetConnListType m_INetConnFreeList;      /// �����������ӽӿ�����
  auto_cs          m_CsNetFreeList;         /// ͬ������m_INetConnFreeList
};