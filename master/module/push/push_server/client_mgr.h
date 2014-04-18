#pragma once


#include "client_info.h"
#include "msg_send_mgr.h"
#include "../common/protocol_st.h"


namespace logic
{

//----------------------------------------------------------------------
// �ͻ��˹�����
class client_mgr
{
public:
    client_mgr();
    ~client_mgr(void);

#define     bucket_size                 (29)            ///< ����hashͰ�Ĵ�С
#define     client_default_timeout     (150)           ///< Ĭ��Ϊ150�볬ʱ

public:
    //----------------------------------------------------------------------
    // ���ⷢ�����ź�
    handle_client_online_callback       s_on_client_online;     // ����
    handle_client_offline_callback      s_on_client_offline;    // ����
    sigslot::signal3<ip_v4, USHORT, const push_msg_ptr> s_on_post_push_msg;

public:
    //----------------------------------------------------------------------
    // key  :  ��Ϊ���ҿͻ��˵�һ����ֵ
    // bNew :  ���ر�ǣ��Ƿ���һ���µĿͻ��ˣ�
    client_info_ex* update_client_info(client_addr_key& key, PT_CMD_ONLINE& cmd_online, BOOL& bNew);        ///< ���¿ͻ�����Ϣ
    void broadcast_user_msg(push_msg_ptr pUserMsg, const std::string& channel);

    void add_client_confirm_msg_id(ip_v4 uAddr, USHORT uPort, UINT64 uMsgID);          ///< ��¼�ͻ��˵Ļ�Ӧ��Ϣ��¼
    BOOL is_client_exists(const client_addr_key& key);                                            ///< �жϿͻ����Ƿ�����

    UINT32 get_online_client_count() const;                                            ///< ��ÿͻ��˵ĸ���
    UINT32 get_channel_count(const char* pszChannel) ;                                 ///< ��ȡָ�������Ŀͻ��˸���
    BOOL   get_online_client_info(std::vector<client_info_ex*>& vecClientInfo,            ///< ��ȡ�ͻ���Ϣ�б�(��ȡ֮��Ҫ����free_online_client_info�ͷ�)
        UINT uStartItem, 
        UINT uFetchNum
    ) const;
    void   free_online_client_info(std::vector<client_info_ex*>& vecClientInfo);

protected:
    bool tiner_check_client_timeout();                                                    ///< ���ͻ����Ƿ�ʱ����һ��ʱ����û���������ݣ�
    BOOL get_client_info(const client_addr_key& key, client_info_ex*& pInfo);
    inline UINT get_array_index(UINT uKey) {  return uKey % bucket_size;  }         ///< ͨ��hash��ȡ���������

protected:
    typedef std::map<client_addr_key, client_info_ex*>  mapClientIntoType;                  ///< ��һ��hash�������ʵ�����
    mapClientIntoType              client_info_map_[bucket_size];      ///< ����ͻ�����Ϣ������
    auto_cs                        client_info_mutex_[bucket_size];    ///< �ͻ�����Ϣ�������

    mutable CObjectPool<client_info_ex, 10000, 5000>  client_info_pool_;   ///< ��ʱ��ģ��
    timer_mgr                      timer_mgr_;
};


}