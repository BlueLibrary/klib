#pragma once

#include <windows.h>


/**
 * @brief �ж��Ƿ���ǰһ��ʵ������
 */

namespace klib {
namespace core {


class app_instance
{
public:
    app_instance(LPCTSTR pszMetuxName) : m_bexist_prev_instances(false)
    {
        HANDLE hMetux = CreateMutex(NULL, FALSE, pszMetuxName);
        if (GetLastError() == ERROR_ALREADY_EXISTS) 
        {
            CloseHandle(hMetux);
            m_bexist_prev_instances = true;
        }
    }

    /**
     * @brief �����Ƿ���ǰһ��ʵ������
     */
    bool is_exist_prev_instance()
    {
        return m_bexist_prev_instances;
    }

    /**
     * @brief ͬ�ϣ������ж��Ƿ����ǰһ��ʵ��
     */
    operator bool ()
    {
        return is_exist_prev_instance();
    }

protected:
    bool  m_bexist_prev_instances;
};


}}
