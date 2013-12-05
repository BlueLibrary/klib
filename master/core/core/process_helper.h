#pragma once

#include <windows.h>

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

    static BOOL shell_execute(LPCTSTR pszExePath);

    ///< �ȴ����̽���
    static DWORD wait_process(HANDLE hProcess, DWORD dwMilliseconds);

    ///< ����˳��Ĵ���
    static BOOL get_process_exit_code(HANDLE hProcess, LPDWORD lpExitCode);
};

}}