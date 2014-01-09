#pragma once

#include "../istddef.h"
#include "../inttype.h"
#include "api_fetcher.h"

#include <windows.h>
#include <tchar.h>

namespace klib{
namespace core{

//----------------------------------------------------------------------
// ������ز�����
class process_helper
{
public:
    process_helper(void);
    ~process_helper(void);

    ///< ���ҽ���
    static BOOL find_process(LPCTSTR pszProcessName);

    ///< ��������
    static BOOL create_process(LPCTSTR pszExePath, LPTSTR pszCmdLine, HANDLE& hProcess);
    
    ///< ��������
    static BOOL create_process(LPCTSTR pszExePath);
    
    ///< �رս���
    static BOOL terminate_process(DWORD dwProcId);

    ///< �رս���
    static BOOL terminate_process(HANDLE handle);

    static BOOL shell_execute(LPCTSTR pszExePath, LPCTSTR pszCmd = _T(""));

    ///< ���û�Ȩ�޴�������
    static BOOL create_user_process(LPCTSTR pszExePath, LPTSTR pszCmd = _T(""));

    static BOOL create_user_process(LPTSTR pszCmdline);

    ///< �ȴ����̽���
    static DWORD wait_process(HANDLE hProcess, DWORD dwMilliseconds);

    ///< ����˳��Ĵ���
    static BOOL get_process_exit_code(HANDLE hProcess, LPDWORD lpExitCode);

    ///< ����Ȩ�� 
    static BOOL enable_privilege(LPCTSTR pszprivilege, BOOL beable) ;

    ///< ��ȡ����·��
    static tstring get_process_path(HANDLE handle);

    ///< ��ȡ����·��
    static tstring get_process_path(DWORD dwProcId);

    ///< ��ȡ��ǰ����·��
    static tstring get_curr_proc_path();

    ///< ��ȡ���̵�������
    static bool get_process_commandline(HANDLE handle, tstring&);
    
    // ��ȡ��������·��  
    static tstring get_process_full_path(DWORD dwPID);

    // ��ȡ���̵�����·��
    static tstring get_process_full_path(HANDLE handle);

protected:
    static tstring dos_path_to_ntpath(LPTSTR pszDosPath);
};

}}