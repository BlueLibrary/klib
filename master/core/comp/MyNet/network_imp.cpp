#include "StdAfx.h"
#include "network_imp.h"
#include <process.h>

#include "inet_tcp_handler.h"
#include "net_conn.h"
#include "net_conn_mgr_imp.h"

#include <net/addr_resolver.h>
#include <net/winsock_init.h>

#include <core/scope_guard.h>

using namespace klib::core;

#include <BaseTsd.h>
#include <MSWSock.h>

#define  MAX_BUF_SIZE 1024


klib::net::winsock_init g_winsock_init_;

CObjectPool<worker_context, 10000, 1000> g_worker_contex_pool_;


//----------------------------------------------------------------------
//
bool network_worker::execute(worker_context*& ctx)
{
    ON_SCOPE_EXIT(
        g_worker_contex_pool_.Free(ctx);
    );

    if (ctx->ctx_type_ == context_recv_ctx) 
    {
        return this->on_recv_ctx(ctx);
    }
    else if (ctx->ctx_type_ == context_send_ctx) 
    {
        return this->on_send_ctx(ctx);
    }

    KLIB_ASSERT(FALSE);
    return true;
}

bool network_worker::on_recv_ctx(worker_context*& ctx)
{
    auto dwByteTransfered = ctx->recv_info.dwByteTransfered;
    auto lpOverlapped = ctx->recv_info.lpOverlapped;
    auto bResult = ctx->recv_info.bResult;
    auto pConn = ctx->pConn.lock();
    network_imp* pimp = ctx->network_;

    ON_SCOPE_EXIT(
        if (NULL != lpOverlapped) 
        {
            pimp->net_overlapped_pool_.Free(lpOverlapped);  //�ͷ�lpOverlapped�ṹ��ÿ���������ʱ�����»�ȡ
        }            
    );

    if (bResult) 
    {
        if(-1 == dwByteTransfered && NULL == lpOverlapped) 
        {
            return true;
        }

        switch(lpOverlapped->operate_type_ )
        {
        case OP_READ:
            pimp->on_read(pConn, (const char*) lpOverlapped->recv_buff_, dwByteTransfered);
            break;

        case OP_WRITE:
            pimp->on_write(pConn, dwByteTransfered);
            break;

        case OP_ACCEPT:
            pimp->on_accept(lpOverlapped->listen_conn_, lpOverlapped->op_conn_);  // AcceptConn
            break;

        case OP_CONNECT:
            pimp->on_connect(pConn, true);
            break;
        }
    }
    else 
    {
        // ����Ͷ�ݼ���
        if (lpOverlapped->operate_type_ == OP_READ) 
        {
            pConn->dec_post_read_count();
        }
        else if (lpOverlapped->operate_type_ == OP_WRITE) 
        {
            pConn->dec_post_write_count();
        }

        if (lpOverlapped->operate_type_ == OP_CONNECT) 
        {
            // ��������ʧ�ܵ�ʱ���ȹر����ӣ�֪ͨ�ϲ㴦��Ȼ���ͷ����Ӷ���
            pConn->dis_connect();
            pimp->net_event_handler_->on_connect(pConn, false);
        }
        else if (lpOverlapped->operate_type_ == OP_ACCEPT) 
        {
            // ��������ʧ��,�ͷ�����
            net_conn_ptr pListConn = pConn;                                     // �����׽���            
            net_conn_ptr pAcceptConn = lpOverlapped->op_conn_;                  // ���յ�����
            pimp->post_accept(pListConn);                                       // ����Ͷ�ݽ�����������
            pimp->net_event_handler_->on_accept(pListConn, pAcceptConn, false); // ֪ͨ�ϲ��������ʧ��
        }
        else 
        {
            // ������������������Ҫ�ͷ����ӣ�
            pConn->dis_connect();
            pimp->check_and_disconnect(pConn);
        }
    } //if (bResult) {

    return true;
}

bool network_worker::on_send_ctx(worker_context*& ctx)
{
    ON_SCOPE_EXIT(
        if (ctx->send_info.buff_ptr_) 
        {
            // ���Ǹ��õķ��ͷ�ʽ
            delete [] ctx->send_info.buff_ptr_;
            ctx->send_info.buff_ptr_ = nullptr;
            ctx->send_info.buff_len_ = 0;
        }
    );

    auto pimp  = ctx->network_;
    auto pConn = ctx->pConn.lock();
    auto buff  = ctx->send_info.buff_ptr_;
    auto len   = ctx->send_info.buff_len_;
    if (0 == len || NULL == pConn) {
        return false;
    }

    // �Ƚ�Ҫ���͵����ݷ�������(֧�ֶ��߳�)
    pConn->write_send_stream(buff, len);

    //@todo ��Ҫ�������ʣ����Ƿ���Ҫ��������
    if (pConn->get_post_write_count() > 0) {
        return true;
    }

    // һ�������ݿ��Է���
    KLIB_ASSERT(pConn->get_send_length() > 0);

    // �ȷ������е�����
    auto seg_ptr = pConn->get_send_stream().get_read_seg_ptr();
    auto seg_len = pConn->get_send_stream().get_read_seg_len();

    return pimp->post_placement_write(pConn, (char*)seg_ptr, seg_len);
}

//----------------------------------------------------------------------
//
network_imp::network_imp(void)
{
    m_lpfnAcceptEx  = NULL;
    m_lpfnConnectEx = NULL;
    hiocp_          = INVALID_HANDLE_VALUE;

    m_bstop = false;
}

network_imp::~network_imp(void)
{
}

bool network_imp::init_network(inet_tcp_handler* handler, 
    size_t thread_num/* = 1*/,
    size_t worker_num/* = 7*/)
{
    // ������ɶ˿�
    hiocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)0, 0);
    _ASSERT(hiocp_);
    if (NULL == hiocp_) 
        return false;
    
    // �����¼�������
    _ASSERT(handler);
    net_event_handler_ = handler;

    init_threads(thread_num);
    init_workers(worker_num);
    net_conn_mgr_i_.reset(new net_conn_mgr_i_imp());

    return true;
}

bool network_imp::try_write(net_conn_ptr pconn, const char* buff, size_t len)
{
    if (0 == len)
        return true;

    //@todo תΪ�����ķ��ͷ�ʽ(���������֧�����ü���)
    char* pbuff = new char[len];
    if (NULL == pbuff) 
        return false;
    scope_guard buff_guard([&]()
    { 
        if(nullptr != pbuff){ delete[]pbuff;} 
    });

    memcpy(pbuff, buff, len);
    worker_context* ctx = g_worker_contex_pool_.Alloc();
    if (nullptr == ctx) {
        return false;
    }
    buff_guard.dismiss();

    ctx->network_  = this;
    ctx->pConn     = pconn;
    ctx->ctx_type_ = context_send_ctx;

    ctx->send_info.buff_ptr_ = pbuff;
    ctx->send_info.buff_len_ = len;
    return get_worker(pconn.get())->send(ctx);
}

net_conn_ptr network_imp::try_listen(USHORT local_port, void* bind_key/* = NULL*/) 
{
    net_conn_ptr pconn =  this->create_listen_conn(local_port);
    if (nullptr == pconn) 
        return nullptr;

    pconn->set_bind_key(bind_key);

    // Ͷ��2����������
    KLIB_ASSERT(pconn.use_count() == 1);
    this->post_accept(pconn);
    this->post_accept(pconn);

    return pconn;
}

net_conn_ptr network_imp::try_connect(const char* addr, USHORT uport, void* bind_key) 
{
    if (nullptr == addr) 
        return nullptr;

    net_conn_ptr pconn(new net_conn);
    if (nullptr == pconn) 
        return nullptr;

    pconn->set_bind_key(bind_key);
    pconn->set_peer_addr_str(addr);
    pconn->set_peer_port(uport);
    if (!this->post_connection(pconn))
        return nullptr;

    return pconn;
}

//----------------------------------------------------------------------
// 
net_conn_ptr network_imp::post_accept(net_conn_ptr pListenConn) 
{
    HANDLE hResult;
    DWORD dwBytes = 0;
    net_overLapped* lpOverlapped = get_net_overlapped();
    if (nullptr == lpOverlapped) {
        return nullptr;
    }

    KLIB_ASSERT(pListenConn.use_count() >= 2);
    SOCKET sockAccept = WSASocket(AF_INET,SOCK_STREAM, IPPROTO_TCP, NULL, NULL,WSA_FLAG_OVERLAPPED);
    if (sockAccept == INVALID_SOCKET) {
        net_overlapped_pool_.Free(lpOverlapped);
        //TRACE(TEXT("Error at socket(): %ld\n"));
        return nullptr;
    }

    if (nullptr == m_lpfnAcceptEx) {
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
            net_overlapped_pool_.Free(lpOverlapped);
            //TRACE(TEXT("fail at WSAIoctl-AcceptEx,Error Code:%d\n"));
            return nullptr;
        }
    }

    net_conn_ptr pNewConn(new net_conn) ;//= net_conn_pool_.Alloc();
    if (nullptr == pNewConn) {
        closesocket(sockAccept);
        net_overlapped_pool_.Free(lpOverlapped);
        return nullptr;
    }

    hResult = CreateIoCompletionPort((HANDLE)sockAccept, 
        hiocp_, 
        (ULONG_PTR) 0, 
        0);
    if (NULL == hResult) {
        closesocket(sockAccept);
        net_overlapped_pool_.Free(lpOverlapped);
        //net_conn_pool_.Free(pNewConn);
        _ASSERT(FALSE);
        return nullptr;
    }
    KLIB_ASSERT(pNewConn.use_count() == 1);

    // �Կ��Ե���getpeername��ȡ�׽�����Ϣ
    BOOL bSetUpdate = TRUE;
    setsockopt(sockAccept,
        SOL_SOCKET,
        SO_UPDATE_ACCEPT_CONTEXT,
        (char *)& bSetUpdate,
        sizeof(bSetUpdate) );

    pNewConn->get_socket()      = sockAccept;
    lpOverlapped->operate_type_ = OP_ACCEPT;
    lpOverlapped->op_conn_      = pNewConn;
    lpOverlapped->listen_conn_  = pListenConn;
    lpOverlapped->hEvent        = NULL;
    KLIB_ASSERT(pNewConn.use_count() == 2);

    BOOL brt = m_lpfnAcceptEx(pListenConn->get_socket(),
        sockAccept,
        lpOverlapped->recv_buff_, 
        0,//lpToverlapped->dwBufSize,
        sizeof(sockaddr_in) + 16, 
        sizeof(sockaddr_in) + 16, 
        &(lpOverlapped->transfer_bytes_), 
        (LPOVERLAPPED)lpOverlapped);

    if(brt == FALSE && WSA_IO_PENDING != WSAGetLastError()) 
    {
        closesocket(sockAccept);
        net_overlapped_pool_.Free(lpOverlapped);
        //net_conn_pool_.Free(pNewConn);

        DWORD dwErrorCode = WSAGetLastError();
        //TRACE(TEXT("fail at lpfnAcceptEx,error code:%d\n"), dwErrorCode);
        return nullptr;
    }
    else if (WSA_IO_PENDING == WSAGetLastError()) 
    {
        KLIB_ASSERT(pNewConn.use_count() >= 2);
        KLIB_ASSERT(pListenConn.use_count() >= 2);

        //�ɹ�
        return pNewConn;
    }

    KLIB_ASSERT(pNewConn.use_count() == 2);
    KLIB_ASSERT(pListenConn.use_count() == 1);
    //�ɹ�
    return pNewConn;
}

bool network_imp::post_connection(net_conn_ptr pConn) 
{
    _ASSERT(pConn);

    UINT32  uPeerAddr = 0;
    klib::net::addr_resolver ip_resolver_(pConn->get_peer_addr_str());
    if (ip_resolver_.size() < 0)  
    {
        return false;
    }
    uPeerAddr = ip_resolver_.at(0);
    pConn->peer_addr_dw_ = uPeerAddr;

    SOCKET& sock = pConn->get_socket();
    sock = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
    if (sock == INVALID_SOCKET) 
    {
        _ASSERT(FALSE);  // DWORD dwErr = WSAGetLastError();
        _ASSERT(!"�����׽���ʧ��!!!");
        return false;
    }

    sockaddr_in local_addr;
    ZeroMemory(&local_addr, sizeof(sockaddr_in));
    local_addr.sin_family = AF_INET;
    int iret = ::bind(sock, (sockaddr*)(&local_addr), sizeof(sockaddr_in));
    if (SOCKET_ERROR == iret) 
    {
        closesocket(sock);
        _ASSERT(!"�󶨶˿�ʧ��!!!");
        return false;
    }

    HANDLE hResult = CreateIoCompletionPort((HANDLE)sock,
        hiocp_, 
        (ULONG_PTR) 0, 
        0);
    if (NULL == hResult) 
    {
        closesocket(sock);
        _ASSERT(FALSE);
        return false;
    }
    KLIB_ASSERT(pConn.use_count() >= 2);

    DWORD dwBytes                  = 0;
    GUID GuidConnectEx             = WSAID_CONNECTEX;

    if (nullptr == m_lpfnConnectEx)
    {
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
    }

    net_overLapped *pmyoverlapped = get_net_overlapped(); // socket��I/OͨѶ������
    if (NULL == pmyoverlapped) 
    {
        _ASSERT(FALSE && "TODO ����Ҫ�ͷ���Դ��");
        return false;
    }
    pmyoverlapped->operate_type_ = OP_CONNECT;  // �����������ͣ��õ�I/O���ʱ���ݴ�                                   //                                         //��ʶ����������
    pmyoverlapped->hEvent        = NULL;  // ���¼�ģ��
    pmyoverlapped->op_conn_      = pConn;

    // ��������Ŀ���ַ
    sockaddr_in addrPeer;  
    ZeroMemory(&addrPeer, sizeof(sockaddr_in));
    addrPeer.sin_family      = AF_INET;
    addrPeer.sin_addr.s_addr = uPeerAddr;
    addrPeer.sin_port        = htons( pConn->get_peer_port() );

    int nLen = sizeof(addrPeer);
    PVOID lpSendBuffer      = nullptr;
    DWORD dwSendDataLength  = 0;
    DWORD dwBytesSent       = 0;

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
        if( WSAGetLastError() != ERROR_IO_PENDING) 
        { 
            // ����ʧ��
            closesocket(sock);
            _ASSERT(FALSE);
            net_overlapped_pool_.Free(pmyoverlapped);
            return FALSE;
        }
        else 
        {
            //TRACE(TEXT("WSAGetLastError() == ERROR_IO_PENDING\n"));// �������ڽ�����
        }
    }

    return true;
}

bool network_imp::post_read(net_conn_ptr pConn) 
{
    DWORD dwRecvedBytes = 0;
    DWORD dwFlag = 0;
    DWORD transfer_bytes_ = 0;
    net_overLapped* pMyoverlapped = get_net_overlapped();
    if (NULL == pMyoverlapped) 
    {
        KLIB_ASSERT(pMyoverlapped);
        return false;
    }

    pMyoverlapped->op_conn_      = pConn;
    pMyoverlapped->hEvent        = NULL;	// 
    pMyoverlapped->operate_type_ = OP_READ ; // ������
    pMyoverlapped->wsaBuf_.buf   = pMyoverlapped->recv_buff_;
    pMyoverlapped->wsaBuf_.len   = sizeof(pMyoverlapped->recv_buff_);

    int nResult = WSARecv (pConn->get_socket(),  // �Ѿ���IOCP������socket
        &pMyoverlapped->wsaBuf_,			// �������ݵ�WSABUF�ṹ
        1,                              // ����������
        &transfer_bytes_,               // ��������ɣ��򷵻ؽ��յõ����ֽ���
        &dwFlag,                        // 
        (OVERLAPPED*) pMyoverlapped,    // �����ﴫ��OVERLADDED�ṹ
        NULL);
    if (SOCKET_ERROR  == nResult) 
    {
        if (WSAGetLastError() ==  WSA_IO_PENDING) 
        {
        }
        else 
        {
            net_overlapped_pool_.Free(pMyoverlapped);

            _ASSERT(net_event_handler_);
            check_and_disconnect(pConn);
            return false;
        }
    }

    pConn->inc_post_read_count();
    return true;
}

bool network_imp::post_placement_write(net_conn_ptr pconn, char* buff, size_t len) 	///< Ͷ��д����
{
    // �����ύ��������
    DWORD dwWriteLen = 0;
    net_overLapped *pmyoverlapped = get_net_overlapped();
    if (nullptr == pmyoverlapped) {
        return false;
    }

    pmyoverlapped->op_conn_      = pconn;
    pmyoverlapped->operate_type_ = OP_WRITE;  // ����I/O��������
    pmyoverlapped->wsaBuf_.buf   = buff;
    pmyoverlapped->wsaBuf_.len   = len;

    int nResult = WSASend(                  // ��ʼ����
        pconn->get_socket(),                // �����ӵ�socket
        &pmyoverlapped->wsaBuf_,            // ����buf�����ݳ���
        1,
        &dwWriteLen,                        // ��������ɣ��򷵻ط��ͳ���
        0,	                                // 
        (LPWSAOVERLAPPED)pmyoverlapped,     // OVERLAPPED �ṹ
        0);                                 // ���̣�����

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
        else 
        {
            return false;
        }
    }
    else 
    {
        //TRACE(TEXT("���������"));
    }

    pconn->inc_post_write_count();
    return true;
}

//----------------------------------------------------------------------
// 
void network_imp::on_read(net_conn_ptr pConn, const char* buff, DWORD dwByteTransfered)
{
     pConn->dec_post_read_count();

    if (dwByteTransfered == 0) 
    {
        // _ASSERT(FALSE && "��������ǶԷ�ֱ�ӹر�������");
        this->check_and_disconnect(pConn);
    }
    else 
    {
        // д�뵽����
        pConn->write_recv_stream(buff, dwByteTransfered);

        // ͳ�Ƹ��׽��ֵĶ��ֽ���
        pConn->add_readed_bytes(dwByteTransfered);

        // ֪ͨ�ϲ㴦����¼�
        net_event_handler_->on_read(pConn, buff, dwByteTransfered);

        // ����Ͷ�ݶ�����
        this->post_read(pConn);
    }

    // ���������ٸ��»�Ծʱ�䣬�Ա���Լ�����ӷ������ݵ��ٶ�
    pConn->upsate_last_active_tm();
}

void network_imp::on_write(net_conn_ptr pconn, DWORD dwByteTransfered)
{
    // �����д�˵�����
    pconn->mark_send_stream(dwByteTransfered);

    // ���ٸ��׽����ϵ�дͶ�ݼ���
    pconn->dec_post_write_count();

    // ͳ�Ƹ��׽��ֵ�д�ֽ���
    pconn->add_writed_bytes(dwByteTransfered);

    // ֪ͨ�ϲ㴦��д�¼�
    net_event_handler_->on_write(pconn, dwByteTransfered);

    // �����ϴλ�Ծ��ʱ���
    pconn->upsate_last_active_tm();

    // ���ŷ���û�з����������
    if (pconn->get_send_length() > 0) 
    {
        // �ٷ������е�����
        auto seg_ptr = pconn->get_send_stream().get_read_seg_ptr();
        auto seg_len = pconn->get_send_stream().get_read_seg_len();

        this->post_placement_write(pconn, (char*)seg_ptr, seg_len);
    }
}

void network_imp::on_accept(net_conn_ptr listen_conn, net_conn_ptr accept_conn)
{
    if (net_conn_mgr_i_) 
    {
#ifdef _DEBUG
        if (net_conn_mgr_i_->is_exist_conn(accept_conn)) 
        {
            _ASSERT(FALSE && "�ظ�������ӣ�������Ƴ���!");
        }
#endif
        net_conn_mgr_i_->add_conn(accept_conn);
    }

    // �����׽��ָ��������ģ��Ա����ͨ��getpeername��ȡ��ip��ַ��
    setsockopt(accept_conn->get_socket(), 
        SOL_SOCKET, 
        SO_UPDATE_ACCEPT_CONTEXT,  
        (char*)&( listen_conn->get_socket() ), 
        sizeof( listen_conn->get_socket()) );

    // ��ȡ�Զ�ip����Ϣ
    accept_conn->init_peer_info();

    // �ٴ�Ͷ���µĽ�����������
    this->post_accept(listen_conn);

    // �����ϲ㴦���¼�,��INetClientImp
    net_event_handler_->on_accept(listen_conn, accept_conn); 

    // ����ʱ���
    listen_conn->upsate_last_active_tm();
    accept_conn->upsate_last_active_tm();

    //Ͷ�ݶ���������Ͽ������ڲ��ᴦ��
    this->post_read(accept_conn);
}

void network_imp::on_connect(net_conn_ptr pConn, bool bsuccess)
{
    if (bsuccess)
    {
#ifdef _DEBUG
        if (net_conn_mgr_i_) 
        {
            if (net_conn_mgr_i_->is_exist_conn(pConn)) 
            {
                _ASSERT(FALSE && "������Ӵ�����ƴ���");
            }
        }
#endif

        net_conn_mgr_i_->add_conn(pConn);
    }


    // ���ӳɹ���֪ͨ�ϲ㴦���¼�
    net_event_handler_->on_connect(pConn, bsuccess);

    // �����ϴλ�Ծ��ʱ���
    pConn->upsate_last_active_tm();

    // Ͷ�ݶ�����
    this->post_read(pConn);
}

net_conn_ptr network_imp::create_listen_conn(USHORT uLocalPort)
{
    net_conn_ptr pListenConn(new net_conn) ;//= net_conn_pool_.Alloc();
    if (nullptr == pListenConn) {
        return nullptr;
    }

    SOCKET& sockListen = pListenConn->get_socket();

    pListenConn->set_local_port(uLocalPort);
    pListenConn->dis_connect();
    sockListen = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
    if (INVALID_SOCKET == sockListen) {
        //net_conn_pool_.Free(pListenConn);
        return nullptr;
    }

    sockaddr_in local_addr;
    ZeroMemory(&local_addr, sizeof(sockaddr_in));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(pListenConn->get_local_port());
    int irt = ::bind(sockListen, (sockaddr*)(&local_addr), sizeof(sockaddr_in));
    listen(sockListen, 5);

    if (SOCKET_ERROR == irt) {
        return nullptr;
    }

    HANDLE hResult = CreateIoCompletionPort((HANDLE)sockListen,
        hiocp_, 
        (ULONG_PTR) 0, 
        0);
    if (nullptr == hResult) {
        _ASSERT(FALSE);
        return nullptr;
    }

    return pListenConn;
}

bool network_imp::init_threads(size_t thread_num)                 ///< ���������-�������߳�
{
    thread_num_ = thread_num;

    work_threads_.resize(thread_num_);
    for (auto itr = work_threads_.begin(); itr != work_threads_.end(); ++ itr)
    {
        itr->start(std::bind(&network_imp::worker_thread_, this));
    }
    return true;
}

bool network_imp::init_workers(size_t worker_num)
{
    return worker_mgr_.init(worker_num);
}

network_worker* network_imp::get_worker(void* p)
{
    return worker_mgr_.choose_worker(p);
}

void network_imp::worker_thread_()
{
    //ʹ����ɶ˿�ģ��
    net_overLapped *lpOverlapped = nullptr;
    DWORD           dwByteTransfered = 0;
    void*           bind_key = nullptr;

    while (!this->m_bstop)
    {
        lpOverlapped = nullptr;

        // ����ĺ������þ���ȥI/O��������ȴ��������I/O�������
        BOOL bResult = GetQueuedCompletionStatus(
            this->hiocp_,                       // ָ�����ĸ�IOCP����������
            &dwByteTransfered,                  // ��û��Ƿ����˶����ֽڵ�����
            (PULONG_PTR)&bind_key,              // socket������IOCPʱָ����һ������ֵ
            (LPWSAOVERLAPPED*)&lpOverlapped,    // ���ConnectEx �������Ľṹ
            INFINITE);                          // һֱ�ȴ���ֱ���н��

        KLIB_ASSERT(lpOverlapped);
        if (nullptr == lpOverlapped)  // �˳�
            return;

        KLIB_ASSERT(nullptr == bind_key);
        KLIB_ASSERT(lpOverlapped->op_conn_.get());

        worker_context* ctx = g_worker_contex_pool_.Alloc();
        scope_guard ctx_guard([&]()
        { 
            if(nullptr != ctx){g_worker_contex_pool_.Free(ctx);} 
        });
        if (nullptr == ctx || nullptr == lpOverlapped->op_conn_.get()) 
        {
            return;
        }

        ctx->network_                     = this;
        ctx->pConn                        = lpOverlapped->op_conn_;
        ctx->ctx_type_                    = context_recv_ctx;

        ctx->recv_info.bResult            = bResult;
        ctx->recv_info.dwByteTransfered   = dwByteTransfered;
        ctx->recv_info.lpOverlapped       = lpOverlapped;
        get_worker(ctx->pConn.lock().get())->send(ctx);
        ctx_guard.dismiss();
    }

    return;
}

net_overLapped* network_imp::get_net_overlapped()
{
    net_overLapped* ptmp = net_overlapped_pool_.Alloc();
    if (ptmp) 
    {
        memset(ptmp, 0, sizeof(OVERLAPPED));
    }

    return ptmp;
}

void network_imp::check_and_disconnect(net_conn_ptr pConn)
{
    if (!pConn->get_is_closing() && 
        pConn->get_post_read_count() == 0 && 
        pConn->get_post_read_count() == 0) 
    {
        pConn->set_is_closing(TRUE);


#ifdef _DEBUG
        if (net_conn_mgr_i_) 
        {
            if (!net_conn_mgr_i_->is_exist_conn(pConn)) 
            {
                // ������������ڸղ������ϻ�û����ӵ�mgr���棬����Ͷ�ݶ�����ͻ����;
                _ASSERT(FALSE && "���ӹرմ�����ƴ���");
                return ;
            }
        }
#endif

        net_event_handler_->on_disconnect(pConn);

        // ɾ������
        if (net_conn_mgr_i_) 
            net_conn_mgr_i_->del_conn(pConn);
    }
}