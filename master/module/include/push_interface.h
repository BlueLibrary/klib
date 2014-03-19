#ifndef _push_framework_interface_h_
#define _push_framework_interface_h_

// {5DBCA606-57CA-4d08-96A3-2A117550DB85}
static const GUID IID_PUSH_CLIENT = 
{ 0x5dbca606, 0x57ca, 0x4d08, { 0x96, 0xa3, 0x2a, 0x11, 0x75, 0x50, 0xdb, 0x85 } };

// {DC52BBBC-45C7-44fd-86F0-C8C00C21F7EB}
static const GUID IID_PUSH_BALANCE_SERVER = 
{ 0xdc52bbbc, 0x45c7, 0x44fd, { 0x86, 0xf0, 0xc8, 0xc0, 0xc, 0x21, 0xf7, 0xeb } };

// {A71E5C2E-947C-425d-BEA2-2B9DF8C6420A}
static const GUID IID_PUSH_LOGIC_SERVER = 
{ 0xa71e5c2e, 0x947c, 0x425d, { 0xbe, 0xa2, 0x2b, 0x9d, 0xf8, 0xc6, 0x42, 0xa } };


#include <net/ip_v4.h>
#include <net/udp_client.h>
#include <functional>
#include <macro.h>
#include <core/small_string.h>

using namespace klib::mem;
using namespace klib::net;


// ���͵���Ϣ����
class push_msg
{
public:
    push_msg() : msg_id_(0), msg_type_(0), delay_fetch_(0), delay_show_(0) {}

protected:
    UINT64           msg_id_;             ///< ��Ϣ���
    UINT32           msg_type_;           ///< ��Ϣ����
    UINT32           show_time_;          ///< ��ʾ��ʱ�䣨��Ҫ��ʾ�೤ʱ�䣩
    UINT32           delay_fetch_;        ///< �ӳٶ೤ʱ���ȡ��Ϣ
    UINT32           delay_show_;         ///< ��ʾ�ӳ�ʱ�䣨��ָ��Ҫ�ӳٶ���ʱ����ʾ��
    std::string      str_sign_;           ///< ��Ϣ���ݵ�ǩ������������ݵ�md5ֵ,ʹ��ǩ���㷨ǩ����
    std::string      content_;            ///< ��Ϣ����(xml����)
    
    DEFINE_ACCESS_FUN_REF2(UINT64, msg_id, msg_id_);
    DEFINE_ACCESS_FUN_REF2(UINT32, msg_type, msg_type_);
    DEFINE_ACCESS_FUN_REF2(UINT32, show_time, show_time_);
    DEFINE_ACCESS_FUN_REF2(UINT32, delay_fetch, delay_fetch_);
    DEFINE_ACCESS_FUN_REF2(UINT32, delay_show, delay_show_);
    DEFINE_ACCESS_FUN_REF2(std::string, str_sign, str_sign_);
    DEFINE_ACCESS_FUN_REF2(std::string, content, content_);
};
typedef std::shared_ptr<push_msg> push_msg_ptr;

// �������ݻص�
typedef std::function<void(udp_client*client_, ip_v4, USHORT, char* buff, size_t len)> 
    handle_data_callback;

// ������Ϣ�ص�
typedef std::function<void(push_msg_ptr)> 
    handle_msg_callback;

enum push_client_status
{
    status_connecting,
    status_query_logic_addr,
    status_query_newver,
    status_online,
};
class push_client_i
{
public:
    virtual bool set_logic_server_info(const std::string& domain, USHORT uport) = 0;
    virtual bool set_data_callback(handle_data_callback& callback) = 0;
    virtual bool set_msg_callback(handle_msg_callback& callback) = 0;
    virtual bool set_client_info(const std::string& channel,
        const std::string& mac,
        const std::string& user,
        const std::string& pass,
        const std::string& last_msg_id, 
        const std::string& appid) = 0;

    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual push_client_status get_status() = 0;
};


//----------------------------------------------------------------------
// �߼���������ַ��Ϣ����
struct logic_addr_info 
{
    logic_addr_info() : addr_(0), uport_(0) {}
    ip_v4   addr_;
    USHORT  uport_;
};

class push_balance_server_i
{
public:
    virtual bool add_logic_info(logic_addr_info info) = 0;
    virtual bool start(USHORT work_port, USHORT maintain_port) = 0;
    
};

///< �ͻ��˵�ַ��Ϣ
struct client_addr_key
{
    client_addr_key(ip_v4 addr, USHORT port_) : addr_(addr), port_(port_)  {  }
    client_addr_key(){}

private:
    ip_v4  addr_;
    USHORT port_;

public:
    DEFINE_ACCESS_FUN_REF2(ip_v4, addr, addr_);
    DEFINE_ACCESS_FUN_REF2(USHORT, port, port_);

    bool operator < (const client_addr_key& other) const 
    {
        if (addr_ < other.addr_) { return true; }
        else if (addr_ > other.addr_) { return false; }

        return port_ < other.port_;
    }

    UINT  hash_key() const {   return addr_;  }
};

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
};

typedef std::function<void(client_info*)> handle_client_online_callback;
typedef std::function<void(client_info*)> handle_client_offline_callback;
typedef std::function<void(client_addr_key, push_msg_ptr, bool)> 
    handle_send_msg_result_callback;

class push_logic_server_i
{
public:
    virtual void set_handle(handle_client_online_callback online_handle,
        handle_client_offline_callback offline_handle,
        handle_send_msg_result_callback msg_result_handle ) = 0;
    virtual bool start(USHORT uport) = 0;
    virtual bool post_send_msg(ip_v4 addr, USHORT port, push_msg_ptr msg) = 0;
    virtual size_t get_online_client_count() = 0;
};


#endif // _push_framework_interface_h_