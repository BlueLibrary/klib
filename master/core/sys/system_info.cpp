#include "system_info.h"


#include <windows.h>

using namespace klib::sys;

//��ȡ����ϵͳ������  
void system_info::get_system_name(std::string& osname)
{  
    SYSTEM_INFO info;        //��SYSTEM_INFO�ṹ�ж�64λAMD������   
    GetSystemInfo(&info);    //����GetSystemInfo�������ṹ   
    OSVERSIONINFOEX os;   
    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);     

    osname = ("unknown OperatingSystem.");  

    if(GetVersionEx((OSVERSIONINFO *)&os))  
    {   
        //������ݰ汾��Ϣ�жϲ���ϵͳ����   
        switch(os.dwMajorVersion)//�ж����汾��  
        {  
        case 4:  
            switch(os.dwMinorVersion)//�жϴΰ汾��   
            {   
            case 0:  
                if(os.dwPlatformId==VER_PLATFORM_WIN32_NT)  
                    osname = ("Microsoft Windows NT 4.0"); //1996��7�·���   
                else if(os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)  
                    osname = ("Microsoft Windows 95");  
                break;  
            case 10:  
                osname = ("Microsoft Windows 98");  
                break;  
            case 90:  
                osname = ("Microsoft Windows Me");  
                break;  
            }  
            break;  

        case 5:  
            switch(os.dwMinorVersion)   //�ٱȽ�dwMinorVersion��ֵ  
            {   
            case 0:  
                osname = ("Microsoft Windows 2000");//1999��12�·���  
                break;  

            case 1:  
                osname = ("Microsoft Windows XP");//2001��8�·���  
                break;  

            case 2:  
                if(os.wProductType==VER_NT_WORKSTATION   
                    && info.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)  
                {  
                    osname =_T("Microsoft Windows XP Professional x64 Edition");  
                }  
                else if(GetSystemMetrics(SM_SERVERR2)==0)  
                    osname =_T("Microsoft Windows Server 2003");//2003��3�·���   
                else if(GetSystemMetrics(SM_SERVERR2)!=0)  
                    osname =_T("Microsoft Windows Server 2003 R2");  
                break;  
            }  
            break;  

        case 6:  
            switch(os.dwMinorVersion)  
            {  
            case 0:  
                if(os.wProductType == VER_NT_WORKSTATION)  
                    osname = ("Microsoft Windows Vista");  
                else  
                    osname = ("Microsoft Windows Server 2008");//�������汾   
                break;  
            case 1:  
                if(os.wProductType == VER_NT_WORKSTATION)  
                    osname = ("Microsoft Windows 7");  
                else  
                    osname = ("Microsoft Windows Server 2008 R2");  
                break;  
            }  
            break;  
        }  
    }//if(GetVersionEx((OSVERSIONINFO *)&os))  

}  

//��ȡ����ϵͳ�İ汾����  
void system_info::get_version_mark(std::string& vmark)
{
    OSVERSIONINFOEX os; 
    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    vmark.clear();

    if(GetVersionEx((OSVERSIONINFO*)&os))  
    {   
        switch(os.dwMajorVersion)  
        {                //���жϲ���ϵͳ�汾   
        case 5:   
            switch(os.dwMinorVersion)  
            {   
            case 0:                  //Windows 2000   
                if(os.wSuiteMask==VER_SUITE_ENTERPRISE)
                {
                    vmark = ("Advanced Server");
                }
                break;   
            case 1:                  //Windows XP   
                if(os.wSuiteMask==VER_SUITE_EMBEDDEDNT) 
                {
                    vmark = ("Embedded");
                }
                else if(os.wSuiteMask==VER_SUITE_PERSONAL)
                {
                    vmark = ("Home Edition");
                }
                else 
                {
                    vmark = ("Professional");
                }
                break;
            case 2:
                if(GetSystemMetrics(SM_SERVERR2)==0
                    &&os.wSuiteMask==VER_SUITE_BLADE)  //Windows Server 2003 
                {
                    vmark = ("Web Edition");
                }
                else if(GetSystemMetrics(SM_SERVERR2)==0
                    && os.wSuiteMask==VER_SUITE_COMPUTE_SERVER)
                {
                    vmark = ("Compute Cluster Edition");
                }
                else if(GetSystemMetrics(SM_SERVERR2)==0
                    && os.wSuiteMask==VER_SUITE_STORAGE_SERVER)
                {
                    vmark = ("Storage Server");
                }
                else if(GetSystemMetrics(SM_SERVERR2) == 0 
                    && os.wSuiteMask==VER_SUITE_DATACENTER)
                {
                    vmark = ("Datacenter Edition");
                }
                else if(GetSystemMetrics(SM_SERVERR2) == 0
                    && os.wSuiteMask==VER_SUITE_ENTERPRISE)
                {
                    vmark = ("Enterprise Edition");
                }
                else if(GetSystemMetrics(SM_SERVERR2) != 0
                    && os.wSuiteMask==VER_SUITE_STORAGE_SERVER)
                {
                    vmark = ("Storage Server");
                }
                break;
            }
            break;

        case 6:  
            switch(os.dwMinorVersion)
            {
            case 0:
                if(os.wProductType!=VER_NT_WORKSTATION
                    && os.wSuiteMask==VER_SUITE_DATACENTER)
                {
                    vmark = ("Datacenter Server");
                }
                else if(os.wProductType!=VER_NT_WORKSTATION
                    && os.wSuiteMask==VER_SUITE_ENTERPRISE)
                {
                    vmark = ("Enterprise");
                }
                else if(os.wProductType==VER_NT_WORKSTATION 
                    && os.wSuiteMask==VER_SUITE_PERSONAL)  //Windows Vista  
                {
                    vmark = ("Home");
                }
                break;
            }
            break;
        }
    }
}