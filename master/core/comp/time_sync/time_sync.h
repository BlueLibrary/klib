#pragma once

#include <windows.h>
#include <vector>
#include <time.h>

//+ time_sync_impl
class time_sync_impl
{
public:
    virtual bool set_retry_times(UINT retryTimes);
    virtual int  get_internet_time(std::string& strTime) ; //��ñ�׼ʱ��
    virtual int  get_nettm_throught_elapse(std::string& strTime) ;
    virtual int  get_nettm_throught_elapse(tm& theStdTime) ;
    virtual bool sync_time();
    virtual bool is_time_ok();     //�жϻ�ȡ����ʱ���Ƿ�ɹ�
    virtual bool get_local_time(std::string& strTime); //��ȡ����ʱ��

public:
    time_sync_impl(void);
    ~time_sync_impl(void);

private:
    void itime_2_stime(SYSTEMTIME &st, const ULONG ulTime);
    void time_2_str(std::string& strTime, SYSTEMTIME &st);

private:
    UINT          retryt_times_; //���Դ���
    bool          time_recved_; //��ʾ�Ƿ���յ�������ʱ��

    SYSTEMTIME    inet_sys_time_; //����������ϻ�ȡ��ʱ��
    ULONG         unet_time_;     //�������ϻ�ȡ��ʱ��ֵ
    DWORD         inet_tick_count_; //����ʱ��ʱ��clock����
};