#pragma once

using namespace klib::mem;

#include <net/ip_v4.h>
#include <pattern/fix_circle_buff.h>
#include <macro.h>

namespace logic
{


//----------------------------------------------------------------------
// �ͻ���ȷ���˵���ϢID
struct confirm_info
{
    confirm_info() : uMsgID(0), uConfirmTime(0) {}
    confirm_info(UINT64 uID, UINT64 uTime) : uMsgID(uID), uConfirmTime(uTime) {}

    UINT64  uMsgID;
    UINT64  uConfirmTime;
};


//----------------------------------------------------------------------
// �ͻ�����Ϣ
class client_info
{
public:
    client_info() : 
        client_port_(0),

        login_time_(0), 
        last_active_time_(0), 
        heart_count_(0),
        version_(0),

        last_msg_id_(0)
     {}
    ~client_info(void) {}

    DEFINE_ACCESS_FUN_REF2(ip_v4, client_addr, client_addr_);
    DEFINE_ACCESS_FUN_REF2(USHORT, client_port, client_port_);

    DEFINE_ACCESS_FUN_REF2(UINT64, login_time,  login_time_);
    DEFINE_ACCESS_FUN_REF2(UINT64, last_active_time, last_active_time_);
    DEFINE_ACCESS_FUN_REF2(UINT64, heart_count, heart_count_);
    DEFINE_ACCESS_FUN_REF2(UINT,   version,     version_);
    
    DEFINE_ACCESS_FUN_REF2(UINT64, last_msg_id,  last_msg_id_);
    
    ip_v4   client_addr_;               ///< �ͻ��˵�IP��ַ
    USHORT  client_port_;               ///< �ͻ��˵Ķ˿ڣ������ֽ���

    UINT64  login_time_;                ///< ��½��ʱ��
    UINT64  last_active_time_;          ///< �ϴλ�Ծ��ʱ��
    UINT64  heart_count_;               ///< ��������
    UINT    version_;                   ///< �汾ֵ

    UINT64  last_msg_id_;               ///< ��Ϣ�ı��
    small_string<13>  mac_;             ///< mac��ַ��δʹ��
    small_string<21>  channel_;         ///< �ͻ��˵�����
    small_string<31>  login_name_;      ///< ���Ե�½��
    small_string<31>  account_;         ///< ���µ��ʺ�

    UINT64    uid;                      ///< �û���ID

    fix_circle_buff<confirm_info, UINT8, 10> confirm_lst_;
};



}
