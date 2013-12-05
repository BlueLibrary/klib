#include "StdAfx.h"
#include "INetworkImp.h"
#include <process.h>

#include "INetEventHandler.h"
#include "INetConnection.h"
#include <net/addr_resolver.h>

#include <BaseTsd.h>
#include <MSWSock.h>

#define  MAX_BUF_SIZE 1024

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

INetNetworkImp::INetNetworkImp(void)
{
  m_lpfnAcceptEx = NULL;
  m_hIocp = INVALID_HANDLE_VALUE;
  
  //��ʼ����
  InitFixedOverlappedList(100);
}

INetNetworkImp::~INetNetworkImp(void)
{
}

bool INetNetworkImp::InitNetwork()
{
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2,2), &wsaData);

  m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,(ULONG_PTR)0, 0);
  _ASSERT(m_hIocp);
  if (NULL == m_hIocp) {
    return false;
  }

  //TRACE(TEXT("��ʼ����ɹ�"));

  return true;
}

bool INetNetworkImp::SetNetEventHandler(INetEventHandler* handler)
{
  _ASSERT(handler);
  m_INetEventHandler = handler;
  return true;
}

bool INetNetworkImp::RunNetwork() 
{
  HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &INetNetworkImp::ThreadNetwork, this, 0, NULL);
  CloseHandle(hThread);
  
  return true;
}

INetConnection* INetNetworkImp::PostAccept(INetConnection* pListenConn) 
{
  HANDLE hResult;
  DWORD dwBytes = 0;
  MYOVERLAPPED* lpOverlapped = GetMyOverlapped();
  if (lpOverlapped == NULL) {
    return false;
  }
  
  SOCKET sockAccept = WSASocket(AF_INET,SOCK_STREAM, IPPROTO_TCP, NULL, NULL,WSA_FLAG_OVERLAPPED);
  if (sockAccept == INVALID_SOCKET) {
    ReleaseMyOverlapped(lpOverlapped);
    //TRACE(TEXT("Error at socket(): %ld\n"));
    return NULL;
  }

  if (NULL == m_lpfnAcceptEx) {
    GUID GuidAcceptEx = WSAID_ACCEPTEX;
    DWORD irt = WSAIoctl(sockAccept,
                          SIO_GET_EXTENSION_FUNCTION_POINTER,
                          &GuidAcceptEx,
                          sizeof(GuidAcceptEx),
                          &m_lpfnAcceptEx,
                          sizeof(m_lpfnAcceptEx),
                          &dwBytes,
                          NULL, 
                          NULL);

    if (irt == SOCKET_ERROR)
    {
      closesocket(sockAccept);
      ReleaseMyOverlapped(lpOverlapped);
      //TRACE(TEXT("fail at WSAIoctl-AcceptEx,Error Code:%d\n"));
      return NULL;
    }
  }

  INetConnection* pNewConn = CreateNewConn();
  if (NULL == pNewConn) {
    closesocket(sockAccept);
    ReleaseMyOverlapped(lpOverlapped);
    return NULL;
  }

  hResult = CreateIoCompletionPort((HANDLE)sockAccept, m_hIocp, (ULONG_PTR)pNewConn, 0);
  if (NULL == hResult) {
    closesocket(sockAccept);
    ReleaseMyOverlapped(lpOverlapped);
    ReleaseConnection(pNewConn);
    _ASSERT(FALSE);
    return NULL;
  }

  // �Կ��Ե���getpeername��ȡ�׽�����Ϣ
  BOOL bSetUpdate = TRUE;
  setsockopt(sockAccept,
	  SOL_SOCKET,
	  SO_UPDATE_ACCEPT_CONTEXT,
	  (char *)& bSetUpdate,
	  sizeof(bSetUpdate) );

  pNewConn->GetSocket() = sockAccept;
  lpOverlapped->operate_type = OP_ACCEPT;
  lpOverlapped->pvoid = (void*) pNewConn;  // ����Ƚ����⣬�����������ͬ
  lpOverlapped->hEvent = NULL;

  BOOL brt = m_lpfnAcceptEx(pListenConn->GetSocket(),
                            sockAccept,
                            lpOverlapped->buff, 
                            0,//lpToverlapped->dwBufSize,
                            sizeof(sockaddr_in) + 16, 
                            sizeof(sockaddr_in) + 16, 
                            &(lpOverlapped->dwBytesTransfered), 
                            (LPOVERLAPPED)lpOverlapped);


  if(brt == FALSE && WSA_IO_PENDING != WSAGetLastError()) {
    closesocket(sockAccept);
    ReleaseMyOverlapped(lpOverlapped);
    ReleaseConnection(pNewConn);

    DWORD dwErrorCode = WSAGetLastError();
    //TRACE(TEXT("fail at lpfnAcceptEx,error code:%d\n"), dwErrorCode);
    return NULL;
  }
  else if (WSA_IO_PENDING == WSAGetLastError()) {
    //�ɹ�
    return pNewConn;
  }

  //�ɹ�
  return pNewConn;
}

bool INetNetworkImp::PostConnection(INetConnection* pConn) 
{
  _ASSERT(pConn);

  UINT32  uPeerAddr = 0;
  klib::net::addr_resolver ipresover(pConn->GetPeerAddress());
  if (ipresover.size() < 0)  {
	  return false;
  }
  uPeerAddr = ipresover.at(0);

  SOCKET& sock = pConn->GetSocket();
  sock = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
  if (sock == INVALID_SOCKET) {
      _ASSERT(FALSE);  // DWORD dwErr = WSAGetLastError();
      _ASSERT(!"�����׽���ʧ��!!!");
      return false;
  }

  sockaddr_in local_addr;
  ZeroMemory(&local_addr, sizeof(sockaddr_in));
  local_addr.sin_family = AF_INET;
  int iret = ::bind(sock, (sockaddr*)(&local_addr), sizeof(sockaddr_in));
  if (SOCKET_ERROR == iret) {
    closesocket(sock);
	_ASSERT(!"�󶨶˿�ʧ��!!!");
	return false;
  }

  HANDLE hResult = CreateIoCompletionPort((HANDLE)sock,m_hIocp, (ULONG_PTR)pConn,0);
  if (NULL == hResult) {
    closesocket(sock);
    _ASSERT(FALSE);
    return false;
  }

  LPFN_CONNECTEX m_lpfnConnectEx = NULL;
  DWORD dwBytes = 0;
  GUID GuidConnectEx = WSAID_CONNECTEX;

  // �ص㣬���ConnectEx������ָ��
  if (SOCKET_ERROR == WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
    &GuidConnectEx, sizeof (GuidConnectEx),
    &m_lpfnConnectEx, sizeof (m_lpfnConnectEx), &dwBytes, 0, 0))
  {
    closesocket(sock);
    //TRACE( TEXT("WSAIoctl is failed. Error code = %d"));//, WSAGetLastError());
    _ASSERT(FALSE);
    return FALSE;
  }

  MYOVERLAPPED *pmyoverlapped = GetMyOverlapped(); // socket��I/OͨѶ������
  if (NULL == pmyoverlapped) {
    _ASSERT(FALSE && "TODO ����Ҫ�ͷ���Դ��");
    return false;
  }
  pmyoverlapped->operate_type = OP_CONNECT;  // �����������ͣ��õ�I/O���ʱ���ݴ�                                   //                                         //��ʶ����������
  pmyoverlapped->hEvent = NULL;  // ���¼�ģ��

  // ��������Ŀ���ַ
  sockaddr_in addrPeer;  
  ZeroMemory(&addrPeer, sizeof(sockaddr_in));
  addrPeer.sin_family = AF_INET;
  addrPeer.sin_addr.s_addr = uPeerAddr;
  addrPeer.sin_port = htons( pConn->GetPeerPort() );

  int nLen = sizeof(addrPeer);
  PVOID lpSendBuffer = NULL;
  DWORD dwSendDataLength = 0;
  DWORD dwBytesSent = 0;

  //�ص�
  BOOL bResult = m_lpfnConnectEx(sock,
                      (sockaddr*)&addrPeer,  // [in] �Է���ַ
                      nLen,  				// [in] �Է���ַ����
                      lpSendBuffer,		// [in] ���Ӻ�Ҫ���͵����ݣ����ﲻ��
                      dwSendDataLength,	// [in] �������ݵ��ֽ��� �����ﲻ��
                      &dwBytesSent,		// [out] �����˶��ٸ��ֽڣ����ﲻ��
                      (OVERLAPPED*)pmyoverlapped); // [in] �ⶫ�����ӣ���һƪ�����

  if (!bResult )  // ����ֵ����
  {
    if( WSAGetLastError() != ERROR_IO_PENDING) { // ����ʧ��
      closesocket(sock);
      _ASSERT(FALSE);
      //TRACE(TEXT("ConnextEx error: %d\n"));
      return FALSE;
    }
    else {//����δ�������ڽ����С���
      //TRACE(TEXT("WSAGetLastError() == ERROR_IO_PENDING\n"));// �������ڽ�����
    }
  }

  return true;
}

bool INetNetworkImp::PostRead(INetConnection* pConn) 
{
  DWORD dwRecvedBytes = 0;
  DWORD dwFlag = 0;
  DWORD dwBytesTransfered = 0;
  MYOVERLAPPED* pMyoverlapped = GetMyOverlapped();
  if (NULL == pMyoverlapped) {
    _ASSERT(pMyoverlapped);
    return false;
  }
  
  pMyoverlapped->hEvent = NULL;	// 
  pMyoverlapped->operate_type = OP_READ ; // ������
  pMyoverlapped->wsaBuf.buf = pMyoverlapped->buff;
  pMyoverlapped->wsaBuf.len = sizeof(pMyoverlapped->buff);
 
  int nResult = WSARecv (pConn->GetSocket(),  // �Ѿ���IOCP������socket
                          &pMyoverlapped->wsaBuf,			// �������ݵ�WSABUF�ṹ
                          1,
                          &dwBytesTransfered, // ��������ɣ��򷵻ؽ��յõ����ֽ���
                          &dwFlag,
                          (OVERLAPPED*)pMyoverlapped,  //�����ﴫ��OVERLADDED�ṹ
                          NULL);
  if (SOCKET_ERROR  == nResult) {
    if (WSAGetLastError() ==  WSA_IO_PENDING) {
    }
    else {
      ReleaseMyOverlapped(pMyoverlapped);

      _ASSERT(m_INetEventHandler);
      CheckAndDisconnect(pConn);
      return false;
    }
  }

  pConn->AddPostReadCount();
  return true;
}

bool INetNetworkImp::PostWrite(INetConnection* pConn, const char* buff, size_t len) 
{
  // �����ύ��������
  DWORD dwWriteLen = 0;
  MYOVERLAPPED *pmyoverlapped = GetMyOverlapped();
  if(pmyoverlapped == NULL) {
    //TRACE(TEXT("fail to molloc memory for pmyoverlapped\n"));
    return false;
  }

  memcpy(pmyoverlapped->buff, buff, len);  // �����ݵ�buf��
  pmyoverlapped->operate_type = OP_WRITE;  //����I/O��������
  pmyoverlapped->wsaBuf.buf = pmyoverlapped->buff;
  pmyoverlapped->wsaBuf.len = len;
  
  int nResult =  WSASend(  //��ʼ����
                          pConn->GetSocket(),  // �����ӵ�socket
                          &pmyoverlapped->wsaBuf, // ����buf�����ݳ���
                          1,
                          &dwWriteLen,// ��������ɣ��򷵻ط��ͳ���
                          0,	// 
                          (LPWSAOVERLAPPED)pmyoverlapped, // OVERLAPPED �ṹ
                          0); // ���̣�����
  
  if (nResult == SOCKET_ERROR)
  {
    int iErrorCode = WSAGetLastError();

    if (iErrorCode ==  WSA_IO_PENDING)
    {
      //TRACE(TEXT("iErrorCode ==  WSA_IO_PENDING\n"));
    }
    else if (iErrorCode == WSAEWOULDBLOCK)
    {
      ///TRACE(TEXT("iErrorCode == WSAEWOULDBLOCK\n"));
    }
    else {
      return false;
    }
  }
  else {
    //TRACE(TEXT("���������"));
  }

  pConn->AddPostWriteCount();
  return true;
}

INetConnection* INetNetworkImp::CreateListenConn(USHORT uLocalPort)
{
  INetConnection* pListenConn = CreateNewConn();
  if (NULL == pListenConn) {
    return NULL;
  }

  SOCKET& sockListen = pListenConn->GetSocket();

  pListenConn->SetLocalPort(uLocalPort);
  pListenConn->DisConnect();
  sockListen = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

  sockaddr_in local_addr;
  ZeroMemory(&local_addr, sizeof(sockaddr_in));
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(pListenConn->GetLocalPort());
  int irt = ::bind(sockListen, (sockaddr*)(&local_addr), sizeof(sockaddr_in));
  listen(sockListen, 5);

  if (SOCKET_ERROR == irt) {
	  ReleaseConnection(pListenConn);
	  return NULL;
  }

  HANDLE hResult = CreateIoCompletionPort((HANDLE)sockListen,m_hIocp, (ULONG_PTR)pListenConn, 0);
  if (NULL == hResult) {
    _ASSERT(FALSE);
    ReleaseConnection(pListenConn);
    return NULL;
  }
  
  return pListenConn;
}

INetConnection* INetNetworkImp::CreateNewConn() 
{
  INetConnection* pConn = NULL;
  auto_lock helper(m_CsNetFreeList);
  if (!m_INetConnFreeList.empty()) {
    pConn = m_INetConnFreeList.front();
    new (pConn) INetConnection;   //placement new
    m_INetConnFreeList.pop_front();
  }
  else {
    pConn = new INetConnection;
  }
  return pConn;
}

bool INetNetworkImp::ReleaseConnection(INetConnection* pConn)
{
  _ASSERT(pConn);
  auto_lock helper(m_CsNetFreeList);

#ifdef _DEBUG

  INetConnListType::const_iterator itr;
  itr = m_INetConnFreeList.begin();
  for (; itr != m_INetConnFreeList.end(); ++itr) {
    if ((*itr) == pConn) {
      _ASSERT(FALSE && "��������⣬�������!");
    }
  }
#endif

  if (m_INetConnFreeList.size() > 1000) {
    delete pConn;
  }
  else {
    pConn->~INetConnection();
    m_INetConnFreeList.push_back(pConn);
  }

  return true;
}

unsigned int WINAPI INetNetworkImp::ThreadNetwork(void* param)
{
  //ʹ����ɶ˿�ģ��
  INetNetworkImp* pINetwork = (INetNetworkImp*)param;
  INetEventHandler* pINetEventHandler = pINetwork->m_INetEventHandler;
  _ASSERT(pINetwork);
  _ASSERT(pINetwork->m_hIocp);
  _ASSERT(pINetEventHandler);
  MYOVERLAPPED *lpOverlapped = NULL;
  DWORD		dwByteTransfered = 0;
  INetConnection *pConn = NULL;

  while (true)
  {
    lpOverlapped = NULL;

    // ����ĺ������þ���ȥI/O��������ȴ��������I/O�������
    BOOL bResult = GetQueuedCompletionStatus(
                        pINetwork->m_hIocp, // ָ�����ĸ�IOCP����������
                        &dwByteTransfered, // ��û��Ƿ����˶����ֽڵ�����
                        (PULONG_PTR)&pConn, // socket������IOCPʱָ����һ������ֵ
                        (LPWSAOVERLAPPED*)&lpOverlapped,  // ���ConnectEx �������Ľṹ
                        INFINITE);				// һֱ�ȴ���ֱ���н��

    _ASSERT(lpOverlapped);
    if (lpOverlapped == NULL)  {
      //TRACE(TEXT("�˳�...."));
      return 0;
    }

    if (bResult) {
      if(dwByteTransfered==-1 && lpOverlapped==NULL) {
        //TRACE(TEXT("�˳��̲߳�����..."));
        return 1L;
      }

      switch(lpOverlapped->operate_type )
      {
      case OP_READ:
        {
          pConn->DecPostReadCount();
          if (dwByteTransfered == 0) {

            //_ASSERT(FALSE && "��������ǶԷ�ֱ�ӹر�������");
            pINetwork->CheckAndDisconnect(pConn);
          }
          else {
			//ͳ�Ƹ��׽��ֵĶ��ֽ���
            pConn->AddBytesRead(dwByteTransfered);

			//֪ͨ�ϲ㴦����¼�
            pINetEventHandler->OnRead(pConn, (const char*)lpOverlapped->buff, dwByteTransfered);

			//����Ͷ�ݶ�����
            pINetwork->PostRead(pConn);
          }
          //���������ٸ��»�Ծʱ�䣬�Ա���Լ�����ӷ������ݵ��ٶ�
          pConn->UpdateLastActiveTimestamp();
        }
        break;

      case OP_WRITE:
		{
		  //���ٸ��׽����ϵ�дͶ�ݼ���
		  pConn->DecPostWriteCount();

		  //ͳ�Ƹ��׽��ֵ�д�ֽ���
          pConn->AddBytesWrite(dwByteTransfered);

          //֪ͨ�ϲ㴦��д�¼�
          pINetEventHandler->OnWrite(pConn);

		  //�����ϴλ�Ծ��ʱ���
          pConn->UpdateLastActiveTimestamp();
        }
        break;
      case OP_ACCEPT:
        {
          INetConnection* pListConn = pConn;      //�����׽���
          INetConnection* pAcceptConn = (INetConnection*) lpOverlapped->pvoid;    //���յ�����

		  //�����׽��ָ��������ģ��Ա����ͨ��getpeername��ȡ��ip��ַ��
		  setsockopt(pAcceptConn->GetSocket(), 
					SOL_SOCKET, 
					SO_UPDATE_ACCEPT_CONTEXT,  
					( char* )&( pListConn->GetSocket() ), 
					sizeof( pListConn->GetSocket()) );

		  //��ȡ�Զ�ip����Ϣ
		  pAcceptConn->InitPeerInfo();

		  //�ٴ�Ͷ���µĽ�����������
          pINetwork->PostAccept(pListConn);
			
		  //�����ϲ㴦���¼�,��INetClientImp
          pINetEventHandler->OnAccept(pListConn, pAcceptConn); 

		  //����ʱ���
          pListConn->UpdateLastActiveTimestamp();
          pAcceptConn->UpdateLastActiveTimestamp();

		  //Ͷ�ݶ���������Ͽ������ڲ��ᴦ��
          pINetwork->PostRead(pAcceptConn);
        }
        break;

      case OP_CONNECT:
        {
          //TRACE(TEXT("connected successfully!\n"));

		  //���ӳɹ���֪ͨ�ϲ㴦���¼�
          pINetEventHandler->OnConnect(pConn, true);

		  //�����ϴλ�Ծ��ʱ���
          pConn->UpdateLastActiveTimestamp();

		  //Ͷ�ݶ�����
          pINetwork->PostRead(pConn);
        }
        break;
      }

	  //�ͷ�lpOverlapped�ṹ��ÿ���������ʱ�����»�ȡ
      pINetwork->ReleaseMyOverlapped(lpOverlapped);
    }
    else {
		//@todo ��Ҫ����Ͽ����ӵĴ���

      if (lpOverlapped->operate_type == OP_READ) {
        pConn->DecPostReadCount();
      }
      else if (lpOverlapped->operate_type == OP_WRITE) {
        pConn->DecPostWriteCount();
      }

      if (lpOverlapped->operate_type == OP_CONNECT) {
        pConn->DisConnect();
        pINetEventHandler->OnConnect(pConn, false);
      }
      else if (lpOverlapped->operate_type == OP_ACCEPT) {
        //
        INetConnection* pListConn = pConn;      //�����׽���
        INetConnection* pAcceptConn = (INetConnection*) lpOverlapped->pvoid;    //���յ�����
        pINetwork->PostAccept(pListConn);
        pINetEventHandler->OnAccept(pListConn, pAcceptConn, false);

		pINetwork->ReleaseConnection(pAcceptConn);
      }
      else {
		//������������������Ҫ�ͷ����ӣ�
        pConn->DisConnect();
        pINetwork->CheckAndDisconnect(pConn);
		pINetwork->ReleaseConnection(pConn);
      }

      pINetwork->ReleaseMyOverlapped(lpOverlapped);
    } //if (bResult) {

  }

  return 0;
}

void INetNetworkImp::InitFixedOverlappedList(size_t nCount)
{
  MYOVERLAPPED* pList = new MYOVERLAPPED[nCount];
  if (NULL == pList) {
    return;
  }

  for (size_t i=0; i<nCount; ++i)
  {
    pList[i].bFixed = true;
    m_overlappedList.push_back(&pList[i]);
  }
}

MYOVERLAPPED* INetNetworkImp::GetMyOverlapped()
{
  auto_lock helper(m_csOverlappedList);

  MYOVERLAPPED* ptmp = NULL;
  if (!m_overlappedList.empty()) {
    ptmp = m_overlappedList.front();
    m_overlappedList.pop_front();

	bool bFix = ptmp->bFixed;
    memset(ptmp, 0, sizeof(OVERLAPPED));
	ptmp->bFixed = bFix;

    //TRACE(TEXT("���ڴ���л�ȡMYOVERLAPPED..."));
  }
  else {

    //TRACE(TEXT("�½�MYOVERLAPPED�ṹ..."));
    ptmp = new MYOVERLAPPED;
  }

  return ptmp;
}

bool INetNetworkImp::ReleaseMyOverlapped(MYOVERLAPPED* pMyoverlapped)
{
  _ASSERT(pMyoverlapped);
  if (NULL == pMyoverlapped) {
    return false;
  }

  auto_lock helper(m_csOverlappedList);

  if (!pMyoverlapped->bFixed) {
    delete pMyoverlapped;
    //TRACE(TEXT("û���棬�ͷ��ڴ�..."));
  }
  else {
    //TRACE(TEXT("����Overlapped��������..."));
    m_overlappedList.push_back(pMyoverlapped);
  }

  return true;
}

void INetNetworkImp::CheckAndDisconnect(INetConnection* pConn)
{
  pConn->lock();
  if (!pConn->GetIsClosing() && pConn->GetPostReadCount() == 0 && pConn->GetPostReadCount() == 0) {
    pConn->SetIsClosing(TRUE);
    pConn->unlock();

    m_INetEventHandler->OnDisConnect(pConn);
  }
  else {
    pConn->unlock();
  }
}