#pragma once

#ifdef _MSC_VER
#include <string>
#include <tchar.h>
#include <Windows.h>
#else
#error "not support yet"
#endif

namespace klib
{

using namespace std;


// ���ÿ�������
bool SetAppRunBoot(const TCHAR* szAppName, const TCHAR* szCommandLine, size_t nLen, bool bSetBoot/* = true*/) ;
bool IsSetAppRunBoot(const TCHAR* szAppName);

// ����debugȨ��
BOOL DebugPrivilegeEnable(BOOL Enable=true);

// ����ڴ��ַ����
BOOL   AddProtect(void *addr, int memSize);
BOOL   AddProtect(HANDLE processHandle, void *addr, unsigned long &proctect);
// ȥ���ڴ��ַ������ʹ�ڴ�����ɶ�����д����ִ��
BOOL   RemoveProtect(void *addr, int memSize);
BOOL   RemoveProtect(HANDLE processHandle, void *addr, unsigned long &oldProctect);

// Զ��ע�뺯��,proHandleԶ�̽��̾��
void* InjectProcess(const char *dllName, HANDLE proHandle);

}