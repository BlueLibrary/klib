

#ifndef _Klib_Mutex_H
#define _Klib_Mutex_H

#ifdef _MSC_VER
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace klib{
namespace kthread  {

    
///< win_mutex����
class  win_mutex
{
public:
    win_mutex();
    ~win_mutex();

    ///< �ж��Ƿ���ڸ����ֵ�mutex����
    static BOOL is_exist_mutex(LPCTSTR lpName);

    ///< ����mutex����
    BOOL create_mutex(BOOL bInitialOwner, LPCTSTR lpName);

    ///< ��metux����
    BOOL open_mutex(LPCTSTR lpName);

    ///< ����
    void lock();

    ///< ����
    void unlock();

private:
    win_mutex(const win_mutex&other);
    win_mutex& operator=( const win_mutex& );

    HANDLE  m_hMutex;
};


///< ����mutex����
class win_mutex_guard
{
public:
    win_mutex_guard(win_mutex& mutex) : m_mutex(mutex)
    {
        m_mutex.lock();
    }

    ~win_mutex_guard()
    {
        m_mutex.unlock();
    }

protected:
    win_mutex&  m_mutex;
};


}}

#endif // MUTEX_H__
