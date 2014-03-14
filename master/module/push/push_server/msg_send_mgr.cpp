#include "stdafx.h"
#include "msg_send_mgr.h"
#include <time.h>


namespace logic
{

msg_send_mgr::msg_send_mgr():
    task_thread_pool_(2),
    max_retry_times_(DEFAULT_MAX_RETRY_TIMES),
    retry_send_interval(DEFAULT_RETRY_INTERVAL),
    whole_resend_times_(0)
{

    timer_mgr_.start();
    timer_mgr_.add(5*1000, std::bind(&msg_send_mgr::check_resend_msg, this));
}

msg_send_mgr::~msg_send_mgr(void)
{
}

void msg_send_mgr::set_max_retry_times(UINT32 uMaxRetryTimes)
{
    _ASSERT(uMaxRetryTimes > 0);
    max_retry_times_ = uMaxRetryTimes;
}

void msg_send_mgr::set_retry_send_interval(UINT32 uRetrySendInterval)
{
    _ASSERT(uRetrySendInterval > 0);
    retry_send_interval = uRetrySendInterval;
}

BOOL msg_send_mgr::post_send_msg(DWORD uAddr, USHORT uPort, push_msg_ptr msg)
{
    auto_lock lock(confirm_msg_mutex_);

    // 1.�����Ƿ��и���ϢID�Ĵ�ȷ���б�
    // 2.���û���ҵ���ֱ����Ӹ���ϢID����Ӧ��Ϣ
    auto itr = confirm_msg_map_.find(msg->get_msg_id());
    if (itr == confirm_msg_map_.end()) 
    {
        msg_confirm_info* pConfirmMsgInfo = confirm_msg_pool_.Alloc();
        if (NULL == pConfirmMsgInfo) 
        {
            return FALSE;
        }
        pConfirmMsgInfo->set_user_msg(msg);

        pConfirmMsgInfo->set_msg_id(msg->get_msg_id());
        pConfirmMsgInfo->set_sended_times(1 + pConfirmMsgInfo->get_sended_times());
        pConfirmMsgInfo->set_last_send_time(_time64(NULL));

        confirm_msg_map_[msg->get_msg_id()][client_key(uAddr, uPort)] = pConfirmMsgInfo;
    }
    else
    {
        // �ҵ��˸���ϢID�ģ���δ�ҵ��ͻ��˵�
        MapClientConfirmMsgInfoType& mapClientMsgMap = itr->second;
        auto itMsg = mapClientMsgMap.find(client_key(uAddr, uPort));
        if (itMsg == mapClientMsgMap.end()) 
        {
            msg_confirm_info* pConfirmMsgInfo = confirm_msg_pool_.Alloc();
            if (NULL == pConfirmMsgInfo) 
            {
                return FALSE;
            }
            pConfirmMsgInfo->set_user_msg(msg);

            pConfirmMsgInfo->set_msg_id(msg->get_msg_id());
            pConfirmMsgInfo->set_sended_times(1 + pConfirmMsgInfo->get_sended_times());
            pConfirmMsgInfo->set_last_send_time(_time64(NULL));

            mapClientMsgMap.insert(std::make_pair(client_key(uAddr, uPort), pConfirmMsgInfo));
        }
        else
        {
            // �ҵ���Ӧ�Ŀͻ�����
            msg_confirm_info* pConfirmMsgInfo = itMsg->second;
            pConfirmMsgInfo->set_last_send_time(_time64(NULL));
            pConfirmMsgInfo->set_sended_times(1 + pConfirmMsgInfo->get_sended_times());
        }
    }

    sign_on_send(uAddr, uPort, msg);

    return TRUE;
}

BOOL msg_send_mgr::post_send_msg(DWORD uAddr, USHORT uPort, std::vector<push_msg_ptr>& vecMsg)
{
    auto itr = vecMsg.begin();
    for (; itr != vecMsg.end(); ++ itr)
    {
        post_send_msg(uAddr, uPort, *itr);
    }
    return TRUE;
}

void msg_send_mgr::on_msg_confirm(DWORD uAddr, USHORT uPort, UINT64 uMsgID)
{
    client_key key(uAddr, uPort);

    auto_lock lock(confirm_msg_mutex_);
    auto itr = confirm_msg_map_.find(uMsgID);
    if (itr == confirm_msg_map_.end()) 
    {
        // ȷ�ϵ���Ϣ�������б��У���������б��������ʧ��
        _ASSERT(FALSE);
        return;
    }

    MapClientConfirmMsgInfoType& mapClientConfirMsg = itr->second;
    auto itMsg = mapClientConfirMsg.find(key);
    if (itMsg == mapClientConfirMsg.end()) 
    {
        // ȷ�ϵ���Ϣ�������б��У���������б��������ʧ��
        _ASSERT(FALSE);
        return; 
    }

    // ��ȷ���б����������Ϣ
    MyPrtLog(_T("�յ��ͻ��˵�ȷ����Ϣ..."));
    confirm_msg_pool_.Free(itMsg->second);
    mapClientConfirMsg.erase(itMsg);
}

BOOL msg_send_mgr::remove_confirm_msg(UINT64 uMsgID)
{
    auto_lock lock(confirm_msg_mutex_);

    auto itrMsgList = confirm_msg_map_.find(uMsgID);
    if (itrMsgList == confirm_msg_map_.end()) 
    {
        return FALSE;
    }

    auto itrConfirm = itrMsgList->second.begin();
    for (; itrConfirm != itrMsgList->second.end(); ++ itrConfirm)
    {
        confirm_msg_pool_.Free(itrConfirm->second);
    }
    itrMsgList->second.clear();
    confirm_msg_map_.erase(itrMsgList);

    return TRUE;
}

bool msg_send_mgr::check_resend_msg()
{
    auto_lock lock(confirm_msg_mutex_);

    UINT64 uTimeNow = _time64(NULL);
    msg_confirm_info* pConfirmInfo = NULL;
    auto itr = confirm_msg_map_.begin();
    for (; itr != confirm_msg_map_.end(); ++ itr)
    {
        auto itMsg = itr->second.begin();
        for (; itMsg != itr->second.end(); )
        {
            pConfirmInfo = itMsg->second;

            // ɾ����������ʹ�����ȷ����Ϣ
            if (pConfirmInfo->get_sended_times() >= max_retry_times_) 
            {
                confirm_msg_pool_.Free(pConfirmInfo);
                itMsg = itr->second.erase(itMsg);
                continue;
            }
            // ����ʱ��û�лظ���Ϣ�ģ��������Է���
            else 
            {
                if (pConfirmInfo->get_last_send_time() + retry_send_interval < uTimeNow)
                {
                    pConfirmInfo->set_last_send_time(uTimeNow);
                    pConfirmInfo->set_sended_times(1 + pConfirmInfo->get_sended_times());
                    InterlockedIncrement64(&whole_resend_times_);

                    MyPrtLog(_T("���Է�����Ϣ:%I64d, �ѷ��ʹ���%I64d"), pConfirmInfo->get_msg_id(), pConfirmInfo->get_sended_times());
                    sign_on_send(itMsg->first.get_addr(), itMsg->first.get_port(), pConfirmInfo->get_user_msg());
                }

                ++ itMsg;
            }
        }
    }

    return TRUE;
}



}