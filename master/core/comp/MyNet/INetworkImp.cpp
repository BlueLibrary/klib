#include "StdAfx.h"
#include "INetworkImp.h"
#include <process.h>

#include "INetEventHandler.h"
#include "net_conn.h"
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

net_conn* INetNetworkImp::PostAccept(net_conn* pListenConn) 
{
  HANDLE hResult;
  DWORD dwBytes = 0;
  Klib_OverLapped* lpOverlapped = GetMyOverlapped();
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

  net_conn* pNewConn = CreateNewConn();
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

  pNewConn->get_socket() = sockAccept;
  lpOverlapped->operate_type = OP_ACCEPT;
  lpOverlapped->pvoid = (void*) pNewConn;  // ����Ƚ����⣬�����������ͬ
  lpOverlapped->hEvent = NULL;

  BOOL brt = m_lpfnAcceptEx(pListenConn->get_socket(),
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

bool INetNetworkImp::PostConnection(net_conn* pConn) 
{
  _ASSERT(pConn);

  UINT32  uPeerAddr = 0;
  klib::net::addr_resolver ipresover(pConn->get_peer_addr());
  if (ipresover.size() < 0)  {
	  return false;
  }
  uPeerAddr = ipresover.at(0);

  SOCKET& sock = pConn->get_socket();
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

  Klib_OverLapped *pmyoverlapped = GetMyOverlapped(); // socket��I/OͨѶ������
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
  addrPeer.sin_port = htons( pConn->get_peer_port() );

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

bool INetNetworkImp::PostRead(net_conn* pConn) 
{
  DWORD dwRecvedBytes = 0;
  DWORD dwFlag = 0;
  DWORD dwBytesTransfered = 0;
  Klib_OverLapped* pMyoverlapped = GetMyOverlapped();
  if (NULL == pMyoverlapped) {
    _ASSERT(pMyoverlapped);
    return false;
  }
  
  pMyoverlapped->hEvent = NULL;	// 
  pMyoverlapped->operate_type = OP_READ ; // ������
  pMyoverlapped->wsaBuf.buf = pMyoverlapped->buff;
  pMyoverlapped->wsaBuf.len = sizeof(pMyoverlapped->buff);
 
  int nResult = WSARecv (pConn->get_socket(),  // �Ѿ���IOCP������socket
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

  pConn->inc_post_read_count();
  return true;
}

bool INetNetworkImp::PostWrite(net_conn* pConn, const char* buff, size_t len) 
{
  // �����ύ��������
  DWORD dwWriteLen = 0;
  Klib_OverLapped *pmyoverlapped = GetMyOverlapped();
  if(pmyoverlapped == NULL) {
    //TRACE(TEXT("fail to molloc memory for pmyoverlapped\n"));
    return false;
  }

  memcpy(pmyoverlapped->buff, buff, len);  // �����ݵ�buf��
  pmyoverlapped->operate_type = OP_WRITE;  //����I/O��������
  pmyoverlapped->wsaBuf.buf = pmyoverlapped->buff;
  pmyoverlapped->wsaBuf.len = len;
  
  int nResult =  WSASend(  //��ʼ����
                          pConn->get_socket(),  // �����ӵ�socket
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

  pConn->inc_post_write_count();
  return true;
}

net_conn* INetNetworkImp::CreateListenConn(USHORT uLocalPort)
{
  net_conn* pListenConn = CreateNewConn();
  if (NULL == pListenConn) {
    return NULL;
  }

  SOCKET& sockListen = pListenConn->get_socket();

  pListenConn->set_local_port(uLocalPort);
  pListenConn->dis_connect();
  sockListen = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

  sockaddr_in local_addr;
  ZeroMemory(&local_addr, sizeof(sockaddr_in));
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(pListenConn->get_local_port());
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

net_conn* INetNetworkImp::CreateNewConn() 
{
  net_conn* pConn = NULL;
  auto_lock helper(m_CsNetFreeList);
  if (!m_INetConnFreeList.empty()) {
    pConn = m_INetConnFreeList.front();
    new (pConn) net_conn;   //placement new
    m_INetConnFreeList.pop_front();
  }
  else {
    pConn = new net_conn;
  }
  return pConn;
}

bool INetNetworkImp::ReleaseConnection(net_conn* pConn)
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
    pConn->~net_conn();
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
  Klib_OverLapped *lpOverlapped = NULL;
  DWORD		dwByteTransfered = 0;
  net_conn *pConn = NULL;

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
          pConn->dec_post_read_count();
          if (dwByteTransfered == 0) {

            //_ASSERT(FALSE && "��������ǶԷ�ֱ�ӹر�������");
            pINetwork->CheckAndDisconnect(pConn);
          }
          else {
			//ͳ�Ƹ��׽��ֵĶ��ֽ���
            pConn->add_readed_bytes(dwByteTransfered);

			//֪ͨ�ϲ㴦����¼�
            pINetEventHandler->OnRead(pConn, (const char*)lpOverlapped->buff, dwByteTransfered);

			//����Ͷ�ݶ�����
            pINetwork->PostRead(pConn);
          }
          //���������ٸ��»�Ծʱ�䣬�Ա���Լ�����ӷ������ݵ��ٶ�
          pConn->upsate_last_active_tm();
        }
        break;

      case OP_WRITE:
		{
		  //���ٸ��׽����ϵ�дͶ�ݼ���
		  pConn->dec_post_write_count();

		  //ͳ�Ƹ��׽��ֵ�д�ֽ���
          pConn->add_rwited_bytes(dwByteTransfered);

          //֪ͨ�ϲ㴦��д�¼�
          pINetEventHandler->OnWrite(pConn);

		  //�����ϴλ�Ծ��ʱ���
          pConn->upsate_last_active_tm();
        }
        break;
      case OP_ACCEPT:
        {
          net_conn* pListConn = pConn;      //�����׽���
          net_conn* pAcceptConn = (net_conn*) lpOverlapped->pvoid;    //���յ�����

		  //�����׽��ָ��������ģ��Ա����ͨ��getpeername��ȡ��ip��ַ��
		  setsockopt(pAcceptConn->get_socket(), 
					SOL_SOCKET, 
					SO_UPDATE_ACCEPT_CONTEXT,  
					( char* )&( pListConn->get_socket() ), 
					sizeof( pListConn->get_socket()) );

		  //��ȡ�Զ�ip����Ϣ
		  pAcceptConn->init_peer_info();

		  //�ٴ�Ͷ���µĽ�����������
          pINetwork->PostAccept(pListConn);
			
		  //�����ϲ㴦���¼�,��INetClientImp
          pINetEventHandler->OnAccept(pListConn, pAcceptConn); 

		  //����ʱ���
          pListConn->upsate_last_active_tm();
          pAcceptConn->upsate_last_active_tm();

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
          pConn->upsate_last_active_tm();

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
        pConn->dec_post_read_count();
      }
      else if (lpOverlapped->operate_type == OP_WRITE) {
        pConn->dec_post_write_count();
      }

      if (lpOverlapped->operate_type == OP_CONNECT) {
        pConn->dis_connect();
        pINetEventHandler->OnConnect(pConn, false);
      }
      else if (lpOverlapped->operate_type == OP_ACCEPT) {
        //
        net_conn* pListConn = pConn;      //�����׽���
        net_conn* pAcceptConn = (net_conn*) lpOverlapped->pvoid;    //���յ�����
        pINetwork->PostAccept(pListConn);
        pINetEventHandler->OnAccept(pListConn, pAcceptConn, false);

		pINetwork->ReleaseConnection(pAcceptConn);
      }
      else {
		//������������������Ҫ�ͷ����ӣ�
        pConn->dis_connect();
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
  Klib_OverLapped* pList = new Klib_OverLapped[nCount];
  if (NULL == pList) {
    return;
  }

  for (size_t i=0; i<nCount; ++i)
  {
    pList[i].bFixed = true;
    m_overlappedList.push_back(&pList[i]);
  }
}

Klib_OverLapped* INetNetworkImp::GetMyOverlapped()
{
  auto_lock helper(m_csOverlappedList);

  Klib_OverLapped* ptmp = NULL;
  if (!m_overlappedList.empty()) {
    ptmp = m_overlappedList.front();
    m_overlappedList.pop_front();

	bool bFix = ptmp->bFixed;
    memset(ptmp, 0, sizeof(OVERLAPPED));
	ptmp->bFixed = bFix;

    //TRACE(TEXT("���ڴ���л�ȡKlib_OverLapped..."));
  }
  else {

    //TRACE(TEXT("�½�Klib_OverLapped�ṹ..."));
    ptmp = new Klib_OverLapped;
  }

  return ptmp;
}

bool INetNetworkImp::ReleaseMyOverlapped(Klib_OverLapped* pMyoverlapped)
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

void INetNetworkImp::CheckAndDisconnect(net_conn* pConn)
{
  pConn->lock();
  if (!pConn->get_is_closing() && pConn->get_post_read_count() == 0 && pConn->get_post_read_count() == 0) {
    pConn->set_is_closing(TRUE);
    pConn->unlock();

    m_INetEventHandler->OnDisConnect(pConn);
  }
  else {
    pConn->unlock();
  }
}