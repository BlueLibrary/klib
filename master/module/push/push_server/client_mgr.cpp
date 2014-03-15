#include "stdafx.h"
#include "client_mgr.h"


namespace logic
{


client_mgr::client_mgr()
{
    timer_mgr_.start();
    timer_mgr_.add(10*1000, std::bind(&client_mgr::check_client_timeout, this));
}

client_mgr::~client_mgr(void)
{
}

client_info* client_mgr::update_client_info(client_key& key, PT_CMD_ONLINE& ptCmdOnline, BOOL& bNew)
{
    bNew = FALSE;
    int index = get_array_index(key.hash_key());
    auto_lock locker(client_info_mutex_[index]);

    auto itr = client_info_map_[index].find(key);
    if (itr == client_info_map_[index].end()) 
    {
        // δ�ҵ�����Ҫ���
        client_info* pInfo = client_info_pool_.Alloc();
        if (NULL == pInfo) 
            return NULL;

        // ����ͻ��˵���Ϣ����
        if (!ptCmdOnline.channel.empty()) 
        {
            pInfo->login_name_ = ptCmdOnline.login_name.c_str();
            pInfo->mac_        = ptCmdOnline.mac.c_str();
            pInfo->channel_    = ptCmdOnline.channel.c_str();
        }

        // �ͻ��˵�ַ+�˿�
        pInfo->client_addr_ = key.get_addr();
        pInfo->client_port_ = key.get_port();

        // ����汾ֵ
        pInfo->version_     = ptCmdOnline.version;

        // ����ʱ��
        UINT64 uTimeNow     = _time64(NULL);
        pInfo->login_time_  = uTimeNow;
        pInfo->last_active_time_ = uTimeNow;

        // ������������
        ++ pInfo->heart_count_;

        // �û����ʺŲ鿴
        if (!ptCmdOnline.account.empty() || ptCmdOnline.uid != 0) 
        {
            pInfo->account_ = ptCmdOnline.account.c_str();
            pInfo->uid = ptCmdOnline.uid;

            sign_client_online.emit(pInfo);
        }

        // ������Ϣ
        client_info_map_[index].insert(std::make_pair(key, pInfo)) ;

        // ������ӵ�
        bNew = TRUE;
        return pInfo;
    }
    else 
    {
        // ��������Ծʱ��
        itr->second->last_active_time_ = _time64(NULL);
        ++ itr->second->heart_count_;

        // �û����ʺŲ鿴
        if (!ptCmdOnline.account.empty() || ptCmdOnline.uid != 0) 
        {
            client_info* pInfo = itr->second;

            // �Ƚ�һ�������Ϣ��һ�µĻ���Ҫ���µ���Ϣ����
            if (ptCmdOnline.uid != pInfo->uid || 
                strcmp(ptCmdOnline.account.c_str(),  pInfo->account_.c_str()) != 0) 
            {
                // �����˳��ź�֪ͨ
                sign_client_offline(pInfo);

                pInfo->account_ = ptCmdOnline.account.c_str();
                pInfo->uid = ptCmdOnline.uid;

                // �����½��Ϣ��֪ͨ
                sign_client_online(pInfo);
            }
        }

        return itr->second;
    }
}

void client_mgr::broadcast_user_msg(push_msg_ptr pUserMsg, const std::string& channel)              // �㲥��Ϣ
{
    // Ͷ�ݵ������б���ȥ
    for (int i=0; i<bucket_size; ++i)
    {
        auto_lock lock(client_info_mutex_[i]);

        // ���������ж��Ƿ���Ҫ������Ϣ
        auto itr = client_info_map_[i].begin();
        for (; itr != client_info_map_[i].end(); ++ itr)
        {
            if (channel.empty() ||
                _strnicmp(channel.c_str(), 
                itr->second->channel_.c_str(), 
                channel.size()) == 0) 
            {
                // Ͷ�ݷ�����Ϣ
                sign_send_push_msg(
                    itr->second->client_addr_, 
                    itr->second->client_port_,
                    pUserMsg
                    );

                MyPrtLog("Ͷ�ݹ㲥��Ϣ:%s:%d", 
                    inet_ntoa(*(in_addr*)&itr->second->client_addr_), 
                    ntohs(itr->second->client_port_));
            }
        }
        //Sleep(10);
    }

    return ;
}

void client_mgr::for_each(std::function<void(client_info* pInfo)> fun)
{
    for (size_t i=0; i<bucket_size; ++i)
    {
        auto_lock lock(client_info_mutex_[i]);

        auto itr = client_info_map_[i].begin();
        for (; itr != client_info_map_[i].end(); ++itr)
        {
            fun(itr->second);
        }
    }
}

void  client_mgr::record_client_confirm_msg(DWORD uAddr, USHORT uPort, UINT64 uMsgID)
{
    client_key key(uAddr, uPort);

    int index = get_array_index(key.hash_key());
    auto_lock locker(client_info_mutex_[index]);

    client_info* pClientInfo;
    if (get_client_info(key, pClientInfo))
    {
        if (pClientInfo->confirm_lst_.is_full()) 
        {
            pClientInfo->confirm_lst_.pop_front();
        }
        pClientInfo->confirm_lst_.push_item(confirm_info(uMsgID, _time64(NULL)));
    }
}

BOOL client_mgr::is_client_exists(client_key& key)
{
    int index = get_array_index(key.hash_key());
    auto_lock locker(client_info_mutex_[index]);

    auto itr = client_info_map_[index].find(key);
    if (itr == client_info_map_[index].end()) {

        return FALSE;
    }

    return TRUE;
}

UINT32 client_mgr::get_online_client_count() const
{
    UINT32 uExistClient = 0;
    for (int i=0; i<bucket_size; ++i)
    {
        uExistClient += client_info_map_[i].size();
    }
    return uExistClient;
}

UINT32 client_mgr::get_channel_count(const char* pszChannel) 
{
    if (NULL == pszChannel) 
    {
        _ASSERT(FALSE) ;
        return 0;
    }

    UINT32 uChannelCount = 0;
    for (int i=0; i<bucket_size; ++i)
    {
        {
            auto_lock lock(client_info_mutex_[i]);

            auto itr = client_info_map_[i].begin();
            for (; itr != client_info_map_[i].end(); ++ itr)
            {
                if (strcmp(itr->second->channel_.c_str(), pszChannel) == 0)
                {
                    ++ uChannelCount;
                }
            }
        }
    }

    return uChannelCount;
}

BOOL client_mgr::get_online_client_info(std::vector<client_info*>& vecClientInfo,
    UINT uStartItem, 
    UINT uFetchNum
    ) const
{
    UINT uIndex = 0;
    UINT uSkipCount = 0;
    BOOL bFinded = FALSE;
    for (; uIndex < bucket_size; ++ uIndex)
    {
        uSkipCount += client_info_map_[uIndex].size();
        if (uStartItem <= uSkipCount) 
        {
            bFinded = TRUE;
            uSkipCount -= client_info_map_[uIndex].size();
            break;
        }
    }

    UINT uNeedSkip = uStartItem - uSkipCount;
    UINT uFetchedNum = 0;           // ��ȡ�ĸ���
    client_info* pInfo = NULL;
    for (int i=uIndex; i<bucket_size; ++i)
    {
        auto itr = client_info_map_[i].begin();
        for (; itr != client_info_map_[i].end(); ++ itr)
        {
            // ��Ҫ�����ĸ���
            if (uNeedSkip > 0) 
            {
                -- uNeedSkip;
                continue;
            }

            // ��ȡ���㹻�ĸ���ֱ�ӷ���
            if (uFetchedNum >= uFetchNum) 
            {
                return TRUE;
            }

            // ��ͻ�����Ϣ�ڴ�
            pInfo = client_info_pool_.Alloc();
            if (NULL == pInfo) 
            {
                return TRUE;
            }

            // ���ص�vector��
            * pInfo = *(itr->second);
            vecClientInfo.push_back(pInfo);

            ++ uFetchedNum;
        }
    }

    return TRUE;
}

void client_mgr::free_online_client_info(std::vector<client_info*>& vecClientInfo)
{
    auto itr = vecClientInfo.begin();
    for (; itr != vecClientInfo.end(); ++ itr)
    {
        client_info_pool_.Free(*itr);
    }
}

bool client_mgr::check_client_timeout()
{
    // һ��һ�εļ�鳬ʱ�Ŀͻ���
    UINT64 uTimeNow = NULL;
    for (int i=0; i<bucket_size; ++i)
    {
        if (TRUE)
        {
            auto_lock lock(client_info_mutex_[i]);

            uTimeNow = _time64(NULL);
            auto itr = client_info_map_[i].begin();
            for (; itr != client_info_map_[i].end(); )
            {
                // ��ʱ�˵��ͷŵ��ڴ����
                if (itr->second->last_active_time_ + CLIENT_DEFAULT_TIME_OUT < uTimeNow) 
                {
                    // ���ȷ������ߵ��ź�
                    sign_client_offline.emit(itr->second);

                    // ��ɾ�����߿ͻ��˵���Ϣ 
                    client_info_pool_.Free(itr->second);
                    itr = client_info_map_[i].erase(itr);
                }
                else
                {
                    ++ itr;
                }
            }

        }

        // ����CPU���ظ�
        Sleep(50);
    }


    return TRUE;
}

BOOL client_mgr::get_client_info(const client_key& key, client_info*& pInfo)
{
    int index = get_array_index(key.hash_key());

    auto itr = client_info_map_[index].find(key);
    if (itr == client_info_map_[index].end()) {

        return FALSE;
    }

    pInfo = & *itr->second;

    return TRUE;
}


}