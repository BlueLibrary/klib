#pragma once

#include "proxy_server_i.h"

#include <core/lock_stl.h>
#include <core/mem_buffer.h>
#include <pattern/fsm.h>

using namespace klib::stl;
using namespace klib::pattern;
using namespace klib::mem;
using namespace klib::pattern::fsm;

typedef mem_buffer<1024*1024, 1024, FALSE> buffer_list_type;


enum http_proxy_status
{
    status_local_read = 1,
    status_connect_remote,
    status_interact,
};

enum http_proxy_event
{
    event_connect = 1,
    event_disconnect,
    event_read,
    event_send,
};

class http_proxy_server_t;
class http_prxy_context;


class proxy_event_base : public FsmEvent
{
public:
    net_conn_ptr local_conn_;
    http_prxy_context* ctx_;
    http_proxy_server_t* svr_;
};

class proxy_event_connect : public proxy_event_base
{
public:
    net_conn_ptr pconn;
    bool bConnected;
};

class proxy_event_read : public proxy_event_base
{
public:
    net_conn_ptr pconn;
    const char* buff;
    size_t len;
};

class proxy_event_disconnect : public proxy_event_base
{
public:
    net_conn_ptr pconn;
};

BEGIN_STATE_DECLARE(local_read_state, status_local_read)

virtual void on_event(FsmEvent* e, UINT& uNewStateID);
END

BEGIN_STATE_DECLARE(connect_remote_state, status_connect_remote)

virtual void on_event(FsmEvent* e, UINT& uNewStateID);
END

BEGIN_STATE_DECLARE(interactive_state, status_interact)

virtual void on_event(FsmEvent* e, UINT& uNewStateID) ;
END


///< ״̬���Ķ���
BEGIN_FSM(http_proxy_fsm)
    BEGIN_REGISTER_STATE()
        REGISTER_INIT_STATE(local_read_state)
        REGISTER_STATE(connect_remote_state)
        REGISTER_STATE(interactive_state)
    END_REGISTER_STATE

public:

END


class http_prxy_context : public proxy_context
{
public:
    buffer_list_type recv_buff_;
    http_proxy_fsm   fsm_;
};

typedef http_prxy_context http_prxy_context_ptr;

class http_proxy_server_t : public proxy_server_imp
{
public:
    http_proxy_server_t(void);
    ~http_proxy_server_t(void);

protected:
    virtual void on_accept(net_conn_ptr listen_conn, net_conn_ptr accept_conn, bool bsuccess);
    virtual void on_connect(net_conn_ptr pconn, bool bConnected);
    virtual void on_read(net_conn_ptr pconn, const char* buff, size_t len);
    virtual void on_disconnect(net_conn_ptr pconn);

public:
    CObjectPool<http_prxy_context, 20000, 10000>            proxy_ctx_pool_;
    uint64_t                                                connected_num_;
};

