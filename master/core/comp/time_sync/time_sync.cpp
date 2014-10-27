
#include "time_sync.h"
#include "debug.h"
#include <tchar.h>
#include <rdebug/log_helper.h>

using namespace klib;
using namespace klib::debug;

time_sync_impl::time_sync_impl(void)
{
    time_recved_ = false;
    unet_time_ = 0;
    retryt_times_ = 3;
}

time_sync_impl::~time_sync_impl(void)
{
}

bool time_sync_impl::set_retry_times(UINT retryTimes)
{
    retryt_times_ = retryTimes;
    return true;
}

int time_sync_impl::get_internet_time(std::string& str_time)  //��ñ�׼ʱ��
{
    ULONG ulTime = 0;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sock) 
    {
        return -1;
    }

    struct NISTSVR
    {
        int     key;	//���
        const char* server;	//����
        const char* info;	//��Ϣ
    }
    NIST_SVRS_ARY[] = 
    {
        { 0,  "time-a.timefreq.bldrdoc.gov", ""},
        { 0,  "utcnist.colorado.edu", ""},
        { 0,  "time.ien.it", ""},
        { 9,  "ntp3.fau.de", "" },
        { 1,  "nist1-ny.ustiming.org", "New York City, NY" },
        { 2,  "nist1-nj.ustiming.org", "Bridgewater, NJ" },
        { 3,  "nist1-pa.ustiming.org", "Hatfield, PA" },
        { 4,  "time-b.timefreq.bldrdoc.gov", "NIST���������У�����������" },
        { 5,  "time-c.timefreq.bldrdoc.gov", "NIST���������У�����������" },
        { 6,  "time.nist.gov"},
        { 7,  "ntp.fudan.edu.cn"},
        { 8,  "timekeeper.isi.edu"},
        { 9,  "subitaneous.cpsc.ucalgary.ca"},
        { 10,  "usno.pa-x.dec.com"},
        { 11,  "time.twc.weather.com"},
        { 12,  "swisstime.ethz.ch"},
        { 13,  "ntp0.fau.de"},
        { 14,  "ntp3.fau.de"},
        { 15,  "time-a.nist.gov"},
        { 16,  "time-b.nist.gov"},
        { 17,  "time-nw.nist.gov"},
        { 18,  "nist1-sj.glassey.com"},
    };

    /*
    { 6,  "tutcnist.colorado.edu", "���������ѧ����������" },
    { 7,  "time-nw.nist.gov", "Microsoft�����ɵ£���ʢ����" },
    { 8,  "nist1.nyc.certifiedtime.com", "Abovnet��ŦԼ��" },
    { 9,  "time.twc.weather.com", "" },
    {10,  "ntp0.fau.de", "" },
    */

    sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(37);
    // http://tf.nist.gov/tf-cgi/servers.cgi 
    // nist1-sj.WiTime.net 64.125.78.85 San Jose, California   
    //servAddr.sin_addr.S_un.S_addr = inet_addr(m_SyncServer);
    
    BOOL is_connected = FALSE;
    BOOL bRecvOK = FALSE; //��ʾ�����Ƿ���ճɹ�

    for (int svr_index = 0; 
        svr_index<_countof(NIST_SVRS_ARY) && FALSE == is_connected;
        ++svr_index) 
    {
        //����������ַ
        unsigned long ip = inet_addr(NIST_SVRS_ARY[svr_index].server);
        hostent* he = NULL;
        if (ip == INADDR_NONE) 
        {
            he = gethostbyname(NIST_SVRS_ARY[svr_index].server);
            if (NULL == he) continue;
            if (NULL == he->h_addr_list) continue;

            ip = inet_addr(inet_ntoa(*(struct in_addr *)he->h_addr_list[0]));
        }
        servAddr.sin_addr.S_un.S_addr = ip;

        //����retryt_times_�����ӣ��������ʧ�������
        for (size_t tms = 0; tms < retryt_times_; tms++)
        {
            is_connected = FALSE;
            if (-1 == connect(sock, (sockaddr *)&servAddr, sizeof(servAddr))) 
            {
                Sleep(200);
                continue;
            }
            else 
            {
                is_connected = TRUE;
                LOGGER() << "���ӷ�����: " <<  NIST_SVRS_ARY[svr_index].server << " �ɹ�";
                break;
            }
        }

        if (!is_connected) 
        {
            LOGGER() << "���ӷ�����: " << NIST_SVRS_ARY[svr_index].server << " ʧ��";
            continue;
        }

        //�������˽��ж�ȡ����
        int nRecv = SOCKET_ERROR;
        bRecvOK = FALSE;
        nRecv = recv(sock, (char *)&ulTime, sizeof(ulTime), 0);
        if (nRecv == 4) 
        {
            ulTime = ntohl(ulTime);
            bRecvOK = TRUE;
            LOGGER() << "��������: " << NIST_SVRS_ARY[svr_index].server << " �ɹ�";
            break;
        }
        else
        {
            LOGGER() << "��������ʧ��: " << NIST_SVRS_ARY[svr_index].server;
            is_connected = FALSE;
        }
    }
    closesocket(sock);

    if (FALSE == bRecvOK || 0 == ulTime)
    {
        LOGGER() << "��ȡ��׼ʱ��ʧ��... ";
        return -2;
    }
    inet_tick_count_ = GetTickCount(); //������յ�����ʱ��clock
    itime_2_stime(inet_sys_time_, ulTime);
    time_2_str(str_time, inet_sys_time_);
    unet_time_ = ulTime;
    time_recved_ = true;

    LOGGER() << "��ȡ�ɹ��˳���ȡ��׼ʱ�亯��...";
    return 0;
}

int time_sync_impl::get_nettm_throught_elapse(std::string& str_time)
{
    //���û�н��յ�internetʱ�����ܽ��м���
    if (!time_recved_) 
    {
        return get_local_time(str_time);
    }
    DWORD now_tick = GetTickCount();
    UINT elapse = UINT((now_tick - inet_tick_count_)*1.0 / 1000);

    SYSTEMTIME st;
    itime_2_stime(st, unet_time_ + elapse);
    time_2_str(str_time, st);
    return 1;
}

int time_sync_impl::get_nettm_throught_elapse(tm& theStdTime)
{
    //���û�н��յ�internetʱ�����ܽ��м���
    if (!time_recved_) 
    {
        return 0;
    }

    DWORD now_tick = GetTickCount();
    UINT elapse = UINT((now_tick - inet_tick_count_)*1.0 / 1000);

    std::string str_time;
    SYSTEMTIME st;
    itime_2_stime(st, unet_time_ + elapse);

    theStdTime.tm_year = st.wYear;
    theStdTime.tm_mon  = st.wMonth;
    theStdTime.tm_mday = st.wDay;
    theStdTime.tm_hour = st.wHour;
    theStdTime.tm_min  = st.wMinute;
    theStdTime.tm_sec  = st.wSecond;
    return 1;
}

bool time_sync_impl::sync_time()
{
    if (!time_recved_) 
    {
        return false;
    }

    DWORD now_tick = GetTickCount();
    UINT elapse = (now_tick - inet_tick_count_) / 1000;

    SYSTEMTIME st;
    itime_2_stime(st, unet_time_ + elapse);

    SetLocalTime(&st);
    return true;
}

bool time_sync_impl::is_time_ok()
{
    return time_recved_;
}

bool time_sync_impl::get_local_time(std::string& str_time)
{
    SYSTEMTIME st;
    ::GetLocalTime(&st);
    time_2_str(str_time, st);

    return true;
}

void time_sync_impl::itime_2_stime(SYSTEMTIME &st, const ULONG ulTime)
{
    time_t tm = ulTime - 2208988800L; //1900.1.1 00:00:00 ~ 1970.1.1 00:00:00 ʱ���
    LONGLONG ll;
    FILETIME ftm;
    FILETIME ft_local;

    ll = Int32x32To64(tm, 10000000) + 116444736000000000;
    ftm.dwLowDateTime = (DWORD)ll;
    ftm.dwHighDateTime = ll >> 32;
    FileTimeToLocalFileTime(&ftm, &ft_local);
    FileTimeToSystemTime(&ft_local, &st);
}

void time_sync_impl::time_2_str(std::string& str_time, SYSTEMTIME &st)
{
    char timebuf[50] = {0};
    sprintf_s(timebuf, sizeof(timebuf),
        "%u-%u-%u %02u:%02u:%02u",
        st.wYear,
        st.wMonth,
        st.wDay,
        st.wHour,
        st.wMinute,
        st.wSecond,
        st.wMilliseconds);
    str_time = timebuf;
}