#ifndef _push_client_imp_h_
#define _push_client_imp_h_


#include "../../include/push_interface.h"
#include <core/timer_mgr.h>
#include <comp/sign_verify/verify_helper.h>

///< �¼�����
enum em_event_type
{
    event_timer,                    ///< ��ʱ���¼�
    event_query_logic_ack_msg,      ///< ��ѯ�߼���ַ
    event_query_ver_ack_msg,        ///< ��ѯ�汾
    event_online_ack_msg,           ///< ������Ϣ�����¼�   
};


#define  DEFAULT_WATCH_TIMEOUT      (30)

///< ��ʱ���¼��ṹ����
class timer_event : public FsmEvent
{
public:
    timer_event(UINT uElapse) : FsmEvent(event_timer)
    {
        set_evt_data(uElapse);
    }
};

///< ��Ϣ�¼�����
class message_event : public FsmEvent
{
public: 
    message_event(UINT uMsg) : FsmEvent(uMsg)
    {
    }
};

///< ��ʼ״̬
class query_logic_state : public CState
{
public:
    query_logic_state() : CState(status_query_logic_addr), 
        query_addr_watch_(DEFAULT_WATCH_TIMEOUT) {}

public:
    virtual void enter(state_i* s);
    virtual void on_event(FsmEvent* e, UINT& uNewStateID);

protected:
    stop_watch query_addr_watch_;      ///< ���ߴ���
};

///< ��ѯ�°汾״̬
class query_newver_state : public CState
{
public:
    query_newver_state() : 
        CState(status_query_newver), 
        newver_watch_(DEFAULT_WATCH_TIMEOUT)//, 
        //m_bDownloading(FALSE) 
    {}

public: 
    virtual void enter(state_i* s);
    virtual void on_event(FsmEvent* e, UINT& uNewStateID);

protected:
    //BOOL  m_bDownloading;
    stop_watch  newver_watch_;
};

///< ����״̬
class online_state : public CState
{
public:
    online_state() : CState(status_online) , online_watch_(DEFAULT_WATCH_TIMEOUT)
    {
        is_online_ = FALSE;
    }

    ///< �жϳ�ʱ��ʱ��ֵ������������ֵû�з������Ļ�Ӧ��Ϣ��ô�ͻᣬת����ѯ�߼�������״̬
    #define  DEFAULT_ONLINE_TIME_OUT  (2 * 60)          

public:
    ///< ��Ҫ��ʼ������
    virtual void enter(state_i* s);

    ///< ����ʱ���¼�������Ϣ�¼�
    virtual void on_event(FsmEvent* e, UINT& uNewStateID);

protected:
    BOOL is_online_;                   ///< ָʾ�Ƿ����߳ɹ�
    UINT64  last_online_ack_time_;      ///< �ϴε�����ʱ��
    stop_watch online_watch_;           ///< ���ߴ���
};

///< ״̬���Ķ���
BEGIN_FSM(push_client_fsm)
    BEGIN_REGISTER_STATE()
        REGISTER_INIT_STATE(query_logic_state)
        REGISTER_STATE(query_newver_state)
        REGISTER_STATE(online_state)
    END_REGISTER_STATE
END

//----------------------------------------------------------------------
// �ͻ��˵ľ���ʵ����
class push_client_imp : 
    public singleton<push_client_imp>
{
    DECLARE_SINGLETON_CLASS(push_client_imp);

public:
    void start();
    void stop();
    push_client_status get_status();
    void reinit();

    DEFINE_ACCESS_FUN_REF2(udp_client, udp_client, client_);

protected:
    virtual void on_msg(udp_client* client_, UINT32 uAddr, USHORT uPort, char* buff, int iLen) ;		///< UDP��Ϣ�ص��ӿ�

public:
    void send_query_logic_addr();
    void send_query_curver();                           ///< ���Ͳ�ѯ��ǰ�汾��Ϣ
    void send_online();                                 ///< ����������Ϣ 
    void send_offline();                                ///< ����������Ϣ 
    void send_msg_ack(UINT64 uMsgID);                   ///< ����ack��Ϣ

protected:
    // ��������������Ϣ 
    void on_query_logic_svr_ack(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar);
    void on_online_msg_ack(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar);///< CMD_ONLINE_ACK
    void on_msg_notify(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar);
    void on_msg_content(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar);
    void on_cur_ver_ack(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar);

protected:
    bool timer_check_status() ;           ///< ���Ӧ�õ�״̬

protected:
    udp_client      client_;
    push_client_fsm push_fsm_;
    timer_mgr       timer_mgr_;
    verify_helper   verify_helper_;
};


#endif // _push_client_imp_h_