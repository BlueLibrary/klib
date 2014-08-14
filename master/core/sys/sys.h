#ifndef _klib_sys_h
#define _klib_sys_h


#include "../istddef.h"
#include "../inttype.h"

#ifdef _MSC_VER
#include <string>
#include <tchar.h>
#include <Windows.h>
#else
#error "not support yet"
#endif

namespace klib {
namespace sys {


using namespace std;
using namespace klib;


class system
{
public:
    // ���ÿ�������
    static bool SetAppRunBoot(const TCHAR* szAppName, const TCHAR* szCommandLine, size_t nLen, bool bSetBoot/* = true*/) ;
    static bool IsSetAppRunBoot(const TCHAR* szAppName);

    // ����debugȨ��
    static BOOL DebugPrivilegeEnable(BOOL Enable=true);

    // ����ڴ��ַ����
    static BOOL   AddProtect(void *addr, int memSize);
    static BOOL   AddProtect(HANDLE processHandle, void *addr, unsigned long &proctect);
    // ȥ���ڴ��ַ������ʹ�ڴ�����ɶ�����д����ִ��
    static BOOL   RemoveProtect(void *addr, int memSize);
    static BOOL   RemoveProtect(HANDLE processHandle, void *addr, unsigned long &oldProctect);

    // Զ��ע�뺯��,proHandleԶ�̽��̾��
    static void* InjectProcess(const char *dllName, HANDLE proHandle);

    // �ж��Ƿ��ǹ���Ա
    static BOOL is_admin();

    // �Թ���ԱȨ������
    static BOOL run_as_admin(LPCTSTR lpExeFile );

    static tstring fetch_cmd_result(const tstring strParamt);

    static void restart_system();
};




}}

#endif
