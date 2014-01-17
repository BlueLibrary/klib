#include "StdAfx.h"
#include "INetworkImp.h"
#include <process.h>

#include "inet_event_handler.h"
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

inetwork_imp::inetwork_imp(void)
{
    m_lpfnAcceptEx = NULL;
    hiocp_ = INVALID_HANDLE_VALUE;

    //��ʼ����
    init_fixed_overlapped_list(100);
}

inetwork_imp::~inetwork_imp(void)
{
}

bool inetwork_imp::init_network()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    hiocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,(ULONG_PTR)0, 0);
    _ASSERT(hiocp_);
    if (NULL == hiocp_) {
        return false;
    }

    //TRACE(TEXT("��ʼ����ɹ�"));

    return true;
}

bool inetwork_imp::set_net_event_handler(inet_event_handler* handler)
{
    _ASSERT(handler);
    net_event_handler_ = handler;
    return true;
}

bool inetwork_imp::run_network() 
{
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &inetwork_imp::work_thread_, this, 0, NULL);
    CloseHandle(hThread);

    return true;
}

net_conn* inetwork_imp::post_accept(net_conn* pListenConn) 
{
    HANDLE hResult;
    DWORD dwBytes = 0;
    net_overLapped* lpOverlapped = get_net_overlapped();
    if (lpOverlapped == NULL) {
        return false;
    }

    SOCKET sockAccept = WSASocket(AF_INET,SOCK_STREAM, IPPROTO_TCP, NULL, NULL,WSA_FLAG_OVERLAPPED);
    if (sockAccept == INVALID_SOCKET) {
        release_net_overlapped(lpOverlapped);
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
            release_net_overlapped(lpOverlapped);
            //TRACE(TEXT("fail at WSAIoctl-AcceptEx,Error Code:%d\n"));
            return NULL;
        }
    }

    net_conn* pNewConn = create_conn();
    if (NULL == pNewConn) {
        closesocket(sockAccept);
        release_net_overlapped(lpOverlapped);
        return NULL;
    }

    hResult = CreateIoCompletionPort((HANDLE)sockAccept, hiocp_, (ULONG_PTR)pNewConn, 0);
    if (NULL == hResult) {
        closesocket(sockAccept);
        release_net_overlapped(lpOverlapped);
        release_conn(pNewConn);
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
    lpOverlapped->operate_type_ = OP_ACCEPT;
    lpOverlapped->pend_data_ = (void*) pNewConn;  // ����Ƚ����⣬�����������ͬ
    lpOverlapped->hEvent = NULL;

    BOOL brt = m_lpfnAcceptEx(pListenConn->get_socket(),
        sockAccept,
        lpOverlapped->buff_, 
        0,//lpToverlapped->dwBufSize,
        sizeof(sockaddr_in) + 16, 
        sizeof(sockaddr_in) + 16, 
        &(lpOverlapped->transfer_bytes_), 
        (LPOVERLAPPED)lpOverlapped);


    if(brt == FALSE && WSA_IO_PENDING != WSAGetLastError()) {
        closesocket(sockAccept);
        release_net_overlapped(lpOverlapped);
        release_conn(pNewConn);

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

bool inetwork_imp::post_connection(net_conn* pConn) 
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

    HANDLE hResult = CreateIoCompletionPort((HANDLE)sock,hiocp_, (ULONG_PTR)pConn,0);
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

    net_overLapped *pmyoverlapped = get_net_overlapped(); // socket��I/OͨѶ������
    if (NULL == pmyoverlapped) {
        _ASSERT(FALSE && "TODO ����Ҫ�ͷ���Դ��");
        return false;
    }
    pmyoverlapped->operate_type_ = OP_CONNECT;  // �����������ͣ��õ�I/O���ʱ���ݴ�                                   //                                         //��ʶ����������
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

bool inetwork_imp::post_read(net_conn* pConn) 
{
    DWORD dwRecvedBytes = 0;
    DWORD dwFlag = 0;
    DWORD transfer_bytes_ = 0;
    net_overLapped* pMyoverlapped = get_net_overlapped();
    if (NULL == pMyoverlapped) 
    {
        _ASSERT(pMyoverlapped);
        return false;
    }

    pMyoverlapped->hEvent = NULL;	// 
    pMyoverlapped->operate_type_ = OP_READ ; // ������
    pMyoverlapped->wsaBuf_.buf = pMyoverlapped->buff_;
    pMyoverlapped->wsaBuf_.len = sizeof(pMyoverlapped->buff_);

    int nResult = WSARecv (pConn->get_socket(),  // �Ѿ���IOCP������socket
        &pMyoverlapped->wsaBuf_,			// �������ݵ�WSABUF�ṹ
        1,
        &transfer_bytes_, // ��������ɣ��򷵻ؽ��յõ����ֽ���
        &dwFlag,
        (OVERLAPPED*)pMyoverlapped,  //�����ﴫ��OVERLADDED�ṹ
        NULL);
    if (SOCKET_ERROR  == nResult) 
    {
        if (WSAGetLastError() ==  WSA_IO_PENDING) 
        {
        }
        else 
        {
            release_net_overlapped(pMyoverlapped);

            _ASSERT(net_event_handler_);
            check_and_disconnect(pConn);
            return false;
        }
    }

    pConn->inc_post_read_count();
    return true;
}

bool inetwork_imp::post_write(net_conn* pConn, const char* buff, size_t len) 
{
    // �����ύ��������
    DWORD dwWriteLen = 0;
    net_overLapped *pmyoverlapped = get_net_overlapped();
    if(pmyoverlapped == NULL) {
        return false;
    }

    memcpy(pmyoverlapped->buff_, buff, len);  // �����ݵ�buf��
    pmyoverlapped->operate_type_ = OP_WRITE;  //����I/O��������
    pmyoverlapped->wsaBuf_.buf = pmyoverlapped->buff_;
    pmyoverlapped->wsaBuf_.len = len;

    int nResult =  WSASend(  //��ʼ����
        pConn->get_socket(),  // �����ӵ�socket
        &pmyoverlapped->wsaBuf_, // ����buf�����ݳ���
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

net_conn* inetwork_imp::create_listen_conn(USHORT uLocalPort)
{
    net_conn* pListenConn = create_conn();
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
        release_conn(pListenConn);
        return NULL;
    }

    HANDLE hResult = CreateIoCompletionPort((HANDLE)sockListen,hiocp_, (ULONG_PTR)pListenConn, 0);
    if (NULL == hResult) {
        _ASSERT(FALSE);
        release_conn(pListenConn);
        return NULL;
    }

    return pListenConn;
}

net_conn* inetwork_imp::create_conn() 
{
    net_conn* pConn = NULL;
    auto_lock helper(free_net_conn_mutex_);
    if (!free_net_conn_list_.empty()) {
        pConn = free_net_conn_list_.front();
        new (pConn) net_conn;   //placement new
        free_net_conn_list_.pop_front();
    }
    else {
        pConn = new net_conn;
    }
    return pConn;
}

bool inetwork_imp::release_conn(net_conn* pConn)
{
    _ASSERT(pConn);
    auto_lock helper(free_net_conn_mutex_);

#ifdef _DEBUG

    INetConnListType::const_iterator itr;
    itr = free_net_conn_list_.begin();
    for (; itr != free_net_conn_list_.end(); ++itr) {
        if ((*itr) == pConn) {
            _ASSERT(FALSE && "��������⣬�������!");
        }
    }
#endif

    if (free_net_conn_list_.size() > 1000) {
        delete pConn;
    }
    else {
        pConn->~net_conn();
        free_net_conn_list_.push_back(pConn);
    }

    return true;
}

unsigned int WINAPI inetwork_imp::work_thread_(void* param)
{
    //ʹ����ɶ˿�ģ��
    inetwork_imp* pINetwork = (inetwork_imp*)param;
    inet_event_handler* pINetEventHandler = pINetwork->net_event_handler_;
    _ASSERT(pINetwork);
    _ASSERT(pINetwork->hiocp_);
    _ASSERT(pINetEventHandler);
    net_overLapped *lpOverlapped = NULL;
    DWORD		dwByteTransfered = 0;
    net_conn *pConn = NULL;

    while (true)
    {
        lpOverlapped = NULL;

        // ����ĺ������þ���ȥI/O��������ȴ��������I/O�������
        BOOL bResult = GetQueuedCompletionStatus(
            pINetwork->hiocp_, // ָ�����ĸ�IOCP����������
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

            switch(lpOverlapped->operate_type_ )
            {
            case OP_READ:
                {
                    pConn->dec_post_read_count();
                    if (dwByteTransfered == 0) {

                        //_ASSERT(FALSE && "��������ǶԷ�ֱ�ӹر�������");
                        pINetwork->check_and_disconnect(pConn);
                    }
                    else {
                        //ͳ�Ƹ��׽��ֵĶ��ֽ���
                        pConn->add_readed_bytes(dwByteTransfered);

                        //֪ͨ�ϲ㴦����¼�
                        pINetEventHandler->on_read(pConn, (const char*)lpOverlapped->buff_, dwByteTransfered);

                        //����Ͷ�ݶ�����
                        pINetwork->post_read(pConn);
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
                    pINetEventHandler->on_write(pConn);

                    //�����ϴλ�Ծ��ʱ���
                    pConn->upsate_last_active_tm();
                }
                break;
            case OP_ACCEPT:
                {
                    net_conn* pListConn = pConn;      //�����׽���
                    net_conn* pAcceptConn = (net_conn*) lpOverlapped->pend_data_;    //���յ�����

                    //�����׽��ָ��������ģ��Ա����ͨ��getpeername��ȡ��ip��ַ��
                    setsockopt(pAcceptConn->get_socket(), 
                        SOL_SOCKET, 
                        SO_UPDATE_ACCEPT_CONTEXT,  
                        ( char* )&( pListConn->get_socket() ), 
                        sizeof( pListConn->get_socket()) );

                    //��ȡ�Զ�ip����Ϣ
                    pAcceptConn->init_peer_info();

                    //�ٴ�Ͷ���µĽ�����������
                    pINetwork->post_accept(pListConn);

                    //�����ϲ㴦���¼�,��INetClientImp
                    pINetEventHandler->on_accept(pListConn, pAcceptConn); 

                    //����ʱ���
                    pListConn->upsate_last_active_tm();
                    pAcceptConn->upsate_last_active_tm();

                    //Ͷ�ݶ���������Ͽ������ڲ��ᴦ��
                    pINetwork->post_read(pAcceptConn);
                }
                break;

            case OP_CONNECT:
                {
                    //TRACE(TEXT("connected successfully!\n"));

                    //���ӳɹ���֪ͨ�ϲ㴦���¼�
                    pINetEventHandler->on_connect(pConn, true);

                    //�����ϴλ�Ծ��ʱ���
                    pConn->upsate_last_active_tm();

                    //Ͷ�ݶ�����
                    pINetwork->post_read(pConn);
                }
                break;
            }

            //�ͷ�lpOverlapped�ṹ��ÿ���������ʱ�����»�ȡ
            pINetwork->release_net_overlapped(lpOverlapped);
        }
        else {
            //@todo ��Ҫ����Ͽ����ӵĴ���

            if (lpOverlapped->operate_type_ == OP_READ) {
                pConn->dec_post_read_count();
            }
            else if (lpOverlapped->operate_type_ == OP_WRITE) {
                pConn->dec_post_write_count();
            }

            if (lpOverlapped->operate_type_ == OP_CONNECT) {
                pConn->dis_connect();
                pINetEventHandler->on_connect(pConn, false);
            }
            else if (lpOverlapped->operate_type_ == OP_ACCEPT) {
                //
                net_conn* pListConn = pConn;      //�����׽���
                net_conn* pAcceptConn = (net_conn*) lpOverlapped->pend_data_;    //���յ�����
                pINetwork->post_accept(pListConn);
                pINetEventHandler->on_accept(pListConn, pAcceptConn, false);

                pINetwork->release_conn(pAcceptConn);
            }
            else {
                //������������������Ҫ�ͷ����ӣ�
                pConn->dis_connect();
                pINetwork->check_and_disconnect(pConn);
                pINetwork->release_conn(pConn);
            }

            pINetwork->release_net_overlapped(lpOverlapped);
        } //if (bResult) {

    }

    return 0;
}

void inetwork_imp::init_fixed_overlapped_list(size_t nCount)
{
    net_overLapped* pList = new net_overLapped[nCount];
    if (NULL == pList) {
        return;
    }

    for (size_t i=0; i<nCount; ++i)
    {
        pList[i].bFixed = true;
        overlapped_list_.push_back(&pList[i]);
    }
}

net_overLapped* inetwork_imp::get_net_overlapped()
{
    auto_lock helper(overlapped_list_mutex_);

    net_overLapped* ptmp = NULL;
    if (!overlapped_list_.empty()) {
        ptmp = overlapped_list_.front();
        overlapped_list_.pop_front();

        bool bFix = ptmp->bFixed;
        memset(ptmp, 0, sizeof(OVERLAPPED));
        ptmp->bFixed = bFix;

        //TRACE(TEXT("���ڴ���л�ȡnet_overLapped..."));
    }
    else {
        //TRACE(TEXT("�½�net_overLapped�ṹ..."));
        ptmp = new net_overLapped;
    }

    return ptmp;
}

bool inetwork_imp::release_net_overlapped(net_overLapped* pMyoverlapped)
{
    _ASSERT(pMyoverlapped);
    if (NULL == pMyoverlapped) {
        return false;
    }

    auto_lock helper(overlapped_list_mutex_);

    if (!pMyoverlapped->bFixed) {
        delete pMyoverlapped;
        //TRACE(TEXT("û���棬�ͷ��ڴ�..."));
    }
    else {
        //TRACE(TEXT("����Overlapped��������..."));
        overlapped_list_.push_back(pMyoverlapped);
    }

    return true;
}

void inetwork_imp::check_and_disconnect(net_conn* pConn)
{
    pConn->lock();
    if (!pConn->get_is_closing() && pConn->get_post_read_count() == 0 && pConn->get_post_read_count() == 0) 
    {
        pConn->set_is_closing(TRUE);
        pConn->unlock();

        net_event_handler_->on_disconnect(pConn);
    }
    else 
    {
        pConn->unlock();
    }
}