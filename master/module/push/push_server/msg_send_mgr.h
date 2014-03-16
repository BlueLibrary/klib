#pragma once


#include "../../include/push_interface.h"
#include <kthread/auto_lock.h>
#include <kthread/threadpool.h>
#include <core/timer_mgr.h>
#include <pattern/object_pool.h>
using namespace klib::kthread;
using namespace klib::pattern;

#include <third/sigslot.h>
#include <comp/sign_verify/sign_helper.h>

#define  DEFAULT_RETRY_INTERVAL     (6)         ///< ��λΪ��
#define  DEFAULT_MAX_RETRY_TIMES    (4)         ///< ������Դ���

namespace logic
{

//----------------------------------------------------------------------
// �ͻ�����Ϣ
struct client_key
{
    client_key(ip_v4 addr, USHORT port_) : addr_(addr), port_(port_)  {  }
    client_key(){}

private:
    ip_v4  addr_;
    USHORT port_;

public:
    DEFINE_ACCESS_FUN_REF2(ip_v4, addr, addr_);
    DEFINE_ACCESS_FUN_REF2(USHORT, port, port_);

    bool operator < (const client_key& other) const 
    {
        if (addr_ < other.addr_) { return true; }
        else if (addr_ > other.addr_) { return false; }

        return port_ < other.port_;
    }

    UINT  hash_key() const {   return addr_;  }
};

//----------------------------------------------------------------------
// ��Ϣȷ����Ϣ
class msg_confirm_info
{
public:
    msg_confirm_info() : msg_id_(0), last_send_time_(0), sended_times_(0) {}
    
private:
    UINT64              msg_id_;                 ///< ��Ϣ�ı��
    UINT64              last_send_time_;         ///< ����͵�ʱ��
    UINT64              sended_times_;           ///< ���͵Ĵ���
    push_msg_ptr        user_msg_;               ///< �û���Ϣ��ָ��

    DEFINE_ACCESS_FUN_REF2(UINT64, msg_id, msg_id_);
    DEFINE_ACCESS_FUN2(UINT64, last_send_time, last_send_time_);
    DEFINE_ACCESS_FUN_REF2(UINT64, sended_times, sended_times_);
    DEFINE_ACCESS_FUN_REF2(push_msg_ptr, user_msg, user_msg_);
};


//----------------------------------------------------------------------
// ��Ϣ���͹���
class msg_send_mgr : 
    public singleton<msg_send_mgr>,
    public sigslot::has_slots<>
{
public:
    msg_send_mgr(void);
    ~msg_send_mgr(void);

    // ��Ҫ����ṩ���͵Ľӿ�
    sigslot::signal3<ip_v4, USHORT, push_msg_ptr> sign_on_send;
    // ɾ����Ϣʱ��Ҫ����Ľӿ�
    sigslot::signal2<const client_key&, const push_msg_ptr&> sign_send_msg_failed;
    sigslot::signal2<const client_key&, const push_msg_ptr&> sign_send_msg_success;

public:
    // ��������(����ʹ��������Լ��)
    void set_max_retry_times(UINT32 uMaxRetryTimes);                                        ///< ����������Դ���
    void set_retry_send_interval(UINT32 uRetrySendInterval);                                ///< �������Լ��

    UINT32 get_max_retry_time() { return max_retry_times_; }                                ///< ��ȡ������Դ���
    UINT32 get_retry_send_interval() { return retry_send_interval; }                       ///< ��ȡ���Լ��

    void post_send_msg(ip_v4 addr, USHORT port, const push_msg_ptr msg);                 ///< Ͷ�ݷ�����Ϣ
    void post_send_msg(ip_v4 addr, USHORT port, const std::vector<push_msg_ptr>& vecMsg);     ///< һ��Ͷ�ݶ����Ϣ

    void on_client_msg_ack(DWORD addr, USHORT port, UINT64 uMsgID);                         ///< ��Ϣ��������
    BOOL remove_msg_confirm_info(UINT64 uMsgID);                                                ///< ɾ�����ڷ���ȷ�ϵ���Ϣ

protected:
    bool timer_check_resend_msg();        ///< ��鲢���Է�����Ϣ

private:
    typedef std::map<client_key, msg_confirm_info*>        MapClientConfirmMsgInfoType;
    typedef std::map<UINT64,  MapClientConfirmMsgInfoType>  MapMsgIDMsgConfirmType;
    MapMsgIDMsgConfirmType              confirm_msg_map_;                                ///< ����ȷ����Ϣ�б�
    mutex                               confirm_msg_mutex_;                              ///< 

    CObjectPool<msg_confirm_info, 10000, 4000>         confirm_msg_pool_;                               ///< ��ȷ����Ϣ�ڴ��
    ThreadPool                          task_thread_pool_;                      
    timer_mgr                           timer_mgr_;

    UINT32                              max_retry_times_;                               ///< ������Է��ʹ���
    UINT32                              retry_send_interval;                           ///< ���Է��͵ļ��
    LONGLONG                            whole_resend_times_;   

    sign_helper                         sign_helper_;
};

}