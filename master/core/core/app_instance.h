#ifndef _klib_app_instance_h
#define _klib_app_instance_h

#include <windows.h>


/**
 * @brief �ж��Ƿ���ǰһ��ʵ������
 */

namespace klib {
namespace core {


class app_instance
{
public:
    app_instance(LPCTSTR pszMetuxName) : is_exists_prev_instance_(false)
    {
        HANDLE hMetux = CreateMutex(NULL, FALSE, pszMetuxName);
        if (GetLastError() == ERROR_ALREADY_EXISTS) 
        {
            CloseHandle(hMetux);
            is_exists_prev_instance_ = true;
        }
    }

    /**
     * @brief �����Ƿ���ǰһ��ʵ������
     */
    bool is_exist_prev_instance()
    {
        return is_exists_prev_instance_;
    }

    /**
     * @brief ͬ�ϣ������ж��Ƿ����ǰһ��ʵ��
     */
    operator bool ()
    {
        return is_exist_prev_instance();
    }

protected:
    bool  is_exists_prev_instance_;
};


}}


#endif
