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
#define     CLIENT_DEFAULT_TIME_OUT     (150)           ///< Ĭ��Ϊ150�볬ʱ

public:
    //----------------------------------------------------------------------
    // �ź�
    sigslot::signal1<client_info*>      sign_client_online;
    sigslot::signal1<client_info*>      sign_client_offline;
    sigslot::signal3<ip_v4, USHORT, const push_msg_ptr> sign_send_push_msg;

public:
    //----------------------------------------------------------------------
    // key  :  ��Ϊ���ҿͻ��˵�һ����ֵ
    // bNew :  ���ر�ǣ��Ƿ���һ���µĿͻ��ˣ�
    client_info* update_client_info(client_key& key, PT_CMD_ONLINE& ptCmdOnline, BOOL& bNew);        ///< ���¿ͻ�����Ϣ
    void broadcast_user_msg(push_msg_ptr pUserMsg, const std::string& channel);                      ///< �㲥��Ϣ
    void for_each(std::function<void(client_info* pInfo)> fun);

    void record_client_confirm_msg(DWORD uAddr, USHORT uPort, UINT64 uMsgID);          ///< ��¼�ͻ��˵Ļ�Ӧ��Ϣ��¼
    BOOL is_client_exists(client_key& key);                                            ///< �жϿͻ����Ƿ�����

    UINT32 get_online_client_count() const;                                            ///< ��ÿͻ��˵ĸ���
    UINT32 get_channel_count(const char* pszChannel) ;                                 ///< ��ȡָ�������Ŀͻ��˸���
    BOOL   get_online_client_info(std::vector<client_info*>& vecClientInfo,            ///< ��ȡ�ͻ���Ϣ�б�(��ȡ֮��Ҫ����free_online_client_info�ͷ�)
        UINT uStartItem, 
        UINT uFetchNum
        ) const;
    void   free_online_client_info(std::vector<client_info*>& vecClientInfo);

protected:
    bool check_client_timeout();                                                    ///< ���ͻ����Ƿ�ʱ����һ��ʱ����û���������ݣ�
    BOOL get_client_info(const client_key& key, client_info*& pInfo);
    inline int get_array_index(UINT uKey) {  return uKey % bucket_size;  }          ///< ͨ��hash��ȡ���������

protected:
    // ��һ��hash�������ʵ�����
    typedef std::map<client_key, client_info*>  mapClientIntoType;
    mapClientIntoType                           client_info_map_[bucket_size];      ///< ����ͻ�����Ϣ������
    auto_cs                                     client_info_mutex_[bucket_size];    ///< �ͻ�����Ϣ�������

    mutable CObjectPool<client_info,10000, 1000>  client_info_pool_;                ///< ��ʱ��ģ��
    timer_mgr                                     timer_mgr_;
};


}