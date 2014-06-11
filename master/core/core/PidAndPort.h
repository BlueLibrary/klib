#ifndef _klib_PidAndPort_h
#define _klib_PidAndPort_h


#include <Windows.h> 
#include <Psapi.h>
#include <Iprtrmib.h>  
#include <iostream>  
#include <vector>  

namespace klib{
namespace core{


#pragma  comment(lib,"Psapi.lib")  
#pragma  comment(lib,"Iphlpapi.Lib")  
#pragma  comment(lib,"WS2_32.lib")  

//------------------------------------------------------------------------
// Remarks:
//      ����ID��˿��໥����(���̶˿ڲ鿴���Netstat -anb)
//------------------------------------------------------------------------
class pid_and_port
{
public:
    pid_and_port(void);
    ~pid_and_port(void);

    enum TcpOrUdp  
    {  
        TcpType,
        UdpType
    };

    typedef struct  
    {  
        DWORD dwState;      //����״̬  
        DWORD dwLocalAddr;  //���ص�ַ  
        DWORD dwLocalPort;  //���ض˿�  
        DWORD dwRemoteAddr; //Զ�̵�ַ  
        DWORD dwRemotePort; //Զ�̶˿�  
        DWORD dwProcessId;  //���̱�ʶ  
    }MIB_TCPEXROW,*PMIB_TCPEXROW;  


    typedef struct  
    {  
        DWORD dwLocalAddr;  //���ص�ַ  
        DWORD dwLocalPort;  //���ض˿�  
        DWORD dwProcessId;  //���̱�ʶ  

    }MIB_UDPEXROW,*PMIB_UDPEXROW;  


    typedef struct  
    {  
        DWORD dwState;      //����״̬  
        DWORD dwLocalAddr;  //���ص�ַ  
        DWORD dwLocalPort;  //���ض˿�  
        DWORD dwRemoteAddr; //Զ�̵�ַ  
        DWORD dwRemotePort; //Զ�̶˿�  
        DWORD dwProcessId;  //���̱�ʶ  
        DWORD Unknown;      //������ʶ  

    }MIB_TCPEXROW_VISTA,*PMIB_TCPEXROW_VISTA;  


    typedef struct  
    {  
        DWORD dwNumEntries;  
        MIB_TCPEXROW table[ANY_SIZE];  

    }MIB_TCPEXTABLE,*PMIB_TCPEXTABLE;  


    typedef struct  
    {  
        DWORD dwNumEntries;  
        MIB_TCPEXROW_VISTA table[ANY_SIZE];  

    }MIB_TCPEXTABLE_VISTA,*PMIB_TCPEXTABLE_VISTA;  


    typedef struct  
    {  
        DWORD dwNumEntries;  
        MIB_UDPEXROW table[ANY_SIZE];  

    }MIB_UDPEXTABLE,*PMIB_UDPEXTABLE;  


    //=====================================================================================//  
    //Name: DWORD AllocateAndGetTcpExTableFromStack()                                      //  
    //                                                                                     //  
    //Descripion: �ú�������ֻ�� Windows XP��Windows Server 2003 ����Ч                           //  
    //                                                                                     //  
    //=====================================================================================//  
    typedef DWORD (WINAPI *PFNAllocateAndGetTcpExTableFromStack)(  
        PMIB_TCPEXTABLE *pTcpTabel,  
        bool bOrder,  
        HANDLE heap,  
        DWORD zero,  
        DWORD flags  
        );  

    //=====================================================================================//  
    //Name: DWORD AllocateAndGetUdpExTableFromStack()                                      //  
    //                                                                                     //  
    //Descripion: �ú�������ֻ�� XP��Windows Server 2003 ����Ч                                   //  
    //                                                                                     //  
    //=====================================================================================//  
    typedef DWORD (WINAPI *PFNAllocateAndGetUdpExTableFromStack)(  
        PMIB_UDPEXTABLE *pUdpTable,  
        bool bOrder,  
        HANDLE heap,  
        DWORD zero,  
        DWORD flags  
        );  

    //=====================================================================================//  
    //Name: DWORD InternalGetTcpTable2()                                                   //  
    //                                                                                     //  
    //Descripion: �ú����� Windows Vista �Լ� Windows 7 ����Ч                                  //  
    //                                                                                     //  
    //=====================================================================================//  
    typedef DWORD (WINAPI *PFNInternalGetTcpTable2)(  
        PMIB_TCPEXTABLE_VISTA *pTcpTable_Vista,  
        HANDLE heap,  
        DWORD flags  
        );  

    //=====================================================================================//  
    //Name: DWORD InternalGetUdpTableWithOwnerPid()                                        //  
    //                                                                                     //  
    //Descripion: �ú����� Windows Vista �Լ� Windows 7 ����Ч                                  //  
    //                                                                                     //  
    //=====================================================================================//  
    typedef DWORD (WINAPI *PFNInternalGetUdpTableWithOwnerPid)(  
        PMIB_UDPEXTABLE *pUdpTable,  
        HANDLE heap,  
        DWORD flags  
        );  


    //=====================================================================================//  
    //Name: DWORD GetProcessIdByPort()                                                     //  
    //                                                                                     //  
    //Descripion: ���ݶ˿ں�����򿪸ö˿ںŵĽ��� ID(֧�� XP��Server 2003��Vista��Win7)            //  
    //                                                                                     //  
    //=====================================================================================//  
    DWORD GetProcessIdByPort(TcpOrUdp type, DWORD dwPort)  
    {  
        HMODULE hModule = LoadLibraryW(L"iphlpapi.dll");  
        if (hModule == NULL)  
        {  
            return 0;  
        }  

        if(type == TcpType)  
        {  
            // ������ѯ���� TCP ��Ϣ  
            PFNAllocateAndGetTcpExTableFromStack pAllocateAndGetTcpExTableFromStack;  
            pAllocateAndGetTcpExTableFromStack =   
                (PFNAllocateAndGetTcpExTableFromStack)GetProcAddress(hModule, "AllocateAndGetTcpExTableFromStack");  
            if (pAllocateAndGetTcpExTableFromStack != NULL)  
            {  
                // ����Ϊ XP ���� Server 2003 ����ϵͳ  
                PMIB_TCPEXTABLE pTcpExTable = NULL;  
                if (pAllocateAndGetTcpExTableFromStack(&pTcpExTable, TRUE, GetProcessHeap(), 0, AF_INET) != 0)  
                {  
                    if (pTcpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return 0;  
                }  

                for (UINT i = 0; i < pTcpExTable->dwNumEntries; i++)  
                {  
                    // ���˵����ݣ�ֻ��ѯ������Ҫ�Ľ�������  
                    if(dwPort == ntohs(0x0000FFFF & pTcpExTable->table[i].dwLocalPort))  
                    {  
                        DWORD dwProcessId = pTcpExTable->table[i].dwProcessId;  
                        if (pTcpExTable)  
                        {  
                            HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                        }  

                        FreeLibrary(hModule);  
                        hModule = NULL;  

                        return dwProcessId;  
                    }  
                }  

                if (pTcpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return 0;  
            }  
            else  
            {  
                // ����Ϊ Vista ���� 7 ����ϵͳ  
                PMIB_TCPEXTABLE_VISTA pTcpExTable = NULL;  
                PFNInternalGetTcpTable2 pInternalGetTcpTable2 =   
                    (PFNInternalGetTcpTable2)GetProcAddress(hModule, "InternalGetTcpTable2");  
                if (pInternalGetTcpTable2 == NULL)  
                {  
                    if (pTcpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return 0;  
                }  

                if (pInternalGetTcpTable2(&pTcpExTable, GetProcessHeap(), 1))  
                {  
                    if (pTcpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return 0;  
                }  

                for (UINT i = 0;i < pTcpExTable->dwNumEntries; i++)  
                {  
                    // ���˵����ݣ�ֻ��ѯ������Ҫ�Ľ�������  
                    if(dwPort == ntohs(0x0000FFFF & pTcpExTable->table[i].dwLocalPort))  
                    {  
                        DWORD dwProcessId = pTcpExTable->table[i].dwProcessId;  
                        if (pTcpExTable)  
                        {  
                            HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                        }  

                        FreeLibrary(hModule);  
                        hModule = NULL;  

                        return dwProcessId;  
                    }  
                }  

                if (pTcpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return 0;  
            }  
        }  
        else if(type == UdpType)  
        {  
            // ������ѯ���� UDP ��Ϣ  
            PMIB_UDPEXTABLE pUdpExTable = NULL;  
            PFNAllocateAndGetUdpExTableFromStack pAllocateAndGetUdpExTableFromStack;  
            pAllocateAndGetUdpExTableFromStack =   
                (PFNAllocateAndGetUdpExTableFromStack)GetProcAddress(hModule,"AllocateAndGetUdpExTableFromStack");  
            if (pAllocateAndGetUdpExTableFromStack != NULL)  
            {  
                // ����Ϊ XP ���� Server 2003 ����ϵͳ  
                if (pAllocateAndGetUdpExTableFromStack(&pUdpExTable, TRUE, GetProcessHeap(), 0, AF_INET) != 0)  
                {  
                    if (pUdpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return 0;  
                }  

                for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)  
                {  
                    // ���˵����ݣ�ֻ��ѯ������Ҫ�Ľ�������  
                    if (dwPort == ntohs(0x0000FFFF & pUdpExTable->table[i].dwLocalPort))  
                    {  
                        DWORD dwProcessId = pUdpExTable->table[i].dwProcessId;  
                        if (pUdpExTable)  
                        {  
                            HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                        }  

                        FreeLibrary(hModule);  
                        hModule = NULL;  

                        return dwProcessId;  
                    }  
                }  

                if (pUdpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return 0;  
            }  
            else  
            {  
                // ����Ϊ Vista ���� 7 ����ϵͳ  
                PFNInternalGetUdpTableWithOwnerPid pInternalGetUdpTableWithOwnerPid;  
                pInternalGetUdpTableWithOwnerPid =   
                    (PFNInternalGetUdpTableWithOwnerPid)GetProcAddress(hModule, "InternalGetUdpTableWithOwnerPid");  
                if (pInternalGetUdpTableWithOwnerPid != NULL)  
                {  
                    if (pInternalGetUdpTableWithOwnerPid(&pUdpExTable, GetProcessHeap(), 1))  
                    {  
                        if (pUdpExTable)  
                        {  
                            HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                        }  

                        FreeLibrary(hModule);  
                        hModule = NULL;  

                        return 0;  
                    }  

                    for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)  
                    {  
                        // ���˵����ݣ�ֻ��ѯ������Ҫ�Ľ�������  
                        if (dwPort == ntohs(0x0000FFFF & pUdpExTable->table[i].dwLocalPort))  
                        {  
                            DWORD dwProcessId = pUdpExTable->table[i].dwProcessId;  
                            if (pUdpExTable)  
                            {  
                                HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                            }  

                            FreeLibrary(hModule);  
                            hModule = NULL;  

                            return dwProcessId;  
                        }  
                    }  
                }  

                if (pUdpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return 0;  
            }  
        }  
        else  
        {  
            FreeLibrary(hModule);  
            hModule = NULL;  

            return -1;  
        }  
    }  


    //=====================================================================================//  
    //Name: DWORD GetAllPortByProcessId()                                                  //  
    //                                                                                     //  
    //Descripion: ���ݽ��� ID ������ý������򿪵����еĶ˿ںţ������� dwAllPort �����з������ж˿ں�    //  
    //            ���� dwMaxLen Ϊ����ĳ��ȣ������ķ���ֵΪ�������򿪵Ķ˿ڵ���Ŀ                    //  
    //            (֧�� XP��Server 2003��Vista��Win7)                                           //  
    //                                                                                     //  
    //=====================================================================================//  
    DWORD GetAllPortByProcessId(TcpOrUdp type, DWORD dwProcessId, DWORD * dwAllPort, DWORD dwMaxLen)  
    {
        DWORD dwPortCount = 0;  
        HMODULE hModule = LoadLibraryW(L"iphlpapi.dll");  
        if (hModule == NULL)  
        {  
            return dwPortCount;  
        }  

        if(type == TcpType)  
        {  
            // ������ѯ���� UDP ��Ϣ  
            PFNAllocateAndGetTcpExTableFromStack pAllocateAndGetTcpExTableFromStack;  
            pAllocateAndGetTcpExTableFromStack = (PFNAllocateAndGetTcpExTableFromStack)GetProcAddress(hModule, "AllocateAndGetTcpExTableFromStack");  
            if (pAllocateAndGetTcpExTableFromStack != NULL)  
            {  
                // ����Ϊ XP ���� Server 2003 ����ϵͳ  
                PMIB_TCPEXTABLE pTcpExTable = NULL;  
                if (pAllocateAndGetTcpExTableFromStack(&pTcpExTable, TRUE, GetProcessHeap(), 0, AF_INET) != 0)  
                {  
                    if (pTcpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return dwPortCount;  
                }  

                for (UINT i = 0; i < pTcpExTable->dwNumEntries; i++)  
                {  
                    // ���˵����ݣ�ֻ��ȡ����Ҫ��ѯ�Ľ��̵� Port ��Ϣ  
                    if(dwProcessId == pTcpExTable->table[i].dwProcessId)  
                    {  
                        if(dwPortCount < dwMaxLen)  
                        {  
                            dwAllPort[dwPortCount] = ntohs(0x0000FFFF & pTcpExTable->table[i].dwLocalPort);  
                            dwPortCount++;  
                        }  
                    }  
                }  

                if (pTcpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return dwPortCount;  
            }  
            else  
            {  
                // ����Ϊ Vista ���� 7 ����ϵͳ  
                PMIB_TCPEXTABLE_VISTA pTcpExTable = NULL;  
                PFNInternalGetTcpTable2 pInternalGetTcpTable2 = (PFNInternalGetTcpTable2)GetProcAddress(hModule, "InternalGetTcpTable2");  
                if (pInternalGetTcpTable2 == NULL)  
                {  
                    if (pTcpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return dwPortCount;  
                }  

                if (pInternalGetTcpTable2(&pTcpExTable, GetProcessHeap(), 1))  
                {  
                    if (pTcpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return dwPortCount;  
                }  

                for (UINT i = 0;i < pTcpExTable->dwNumEntries; i++)  
                {  
                    // ���˵����ݣ�ֻ��ȡ����Ҫ��ѯ�Ľ��̵� TCP Port ��Ϣ  
                    if(dwProcessId == pTcpExTable->table[i].dwProcessId)  
                    {  
                        if(dwPortCount < dwMaxLen)  
                        {  
                            dwAllPort[dwPortCount] = ntohs(0x0000FFFF & pTcpExTable->table[i].dwLocalPort);  
                            dwPortCount++;  
                        }  
                    }  
                }  

                if (pTcpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return dwPortCount;  
            }  
        }  
        else if(type == UdpType)  
        {  
            // ������ѯ���� UDP ��Ϣ  
            PMIB_UDPEXTABLE pUdpExTable = NULL;  
            PFNAllocateAndGetUdpExTableFromStack pAllocateAndGetUdpExTableFromStack;  
            pAllocateAndGetUdpExTableFromStack = (PFNAllocateAndGetUdpExTableFromStack)GetProcAddress(hModule,"AllocateAndGetUdpExTableFromStack");  
            if (pAllocateAndGetUdpExTableFromStack != NULL)  
            {  
                // ����Ϊ XP ���� Server 2003 ����ϵͳ  
                if (pAllocateAndGetUdpExTableFromStack(&pUdpExTable, TRUE, GetProcessHeap(), 0, AF_INET) != 0)  
                {  
                    if (pUdpExTable)  
                    {  
                        HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                    }  

                    FreeLibrary(hModule);  
                    hModule = NULL;  

                    return dwPortCount;  
                }  

                for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)  
                {  
                    // ���˵����ݣ�ֻ��ȡ����Ҫ��ѯ�Ľ��̵� UDP Port��Ϣ  
                    if(dwProcessId == pUdpExTable->table[i].dwProcessId)  
                    {  
                        if(dwPortCount < dwMaxLen)  
                        {  
                            dwAllPort[dwPortCount] = ntohs(0x0000FFFF & pUdpExTable->table[i].dwLocalPort);  
                            dwPortCount++;  
                        }  
                    }  
                }  

                if (pUdpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return dwPortCount;  
            }  
            else  
            {  
                // ����Ϊ Vista ���� 7 ����ϵͳ  
                PFNInternalGetUdpTableWithOwnerPid pInternalGetUdpTableWithOwnerPid;  
                pInternalGetUdpTableWithOwnerPid = (PFNInternalGetUdpTableWithOwnerPid)GetProcAddress(hModule, "InternalGetUdpTableWithOwnerPid");  
                if (pInternalGetUdpTableWithOwnerPid != NULL)  
                {  
                    if (pInternalGetUdpTableWithOwnerPid(&pUdpExTable, GetProcessHeap(), 1))  
                    {  
                        if (pUdpExTable)  
                        {  
                            HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                        }  

                        FreeLibrary(hModule);  
                        hModule = NULL;  

                        return dwPortCount;  
                    }  

                    for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)  
                    {  
                        // ���˵����ݣ�ֻ��ȡ����Ҫ��ѯ�Ľ��̵� UDP Port��Ϣ  
                        if(dwProcessId == pUdpExTable->table[i].dwProcessId)  
                        {  
                            if(dwPortCount < dwMaxLen)  
                            {  
                                dwAllPort[dwPortCount] = ntohs(0x0000FFFF & pUdpExTable->table[i].dwLocalPort);  
                                dwPortCount++;  
                            }  
                        }  
                    }  
                }  

                if (pUdpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pUdpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return dwPortCount;  
            }  
        }  
        else  
        {  
            FreeLibrary(hModule);  
            hModule = NULL;  

            return dwPortCount;  
        }  
    }  
};




}}



#endif
