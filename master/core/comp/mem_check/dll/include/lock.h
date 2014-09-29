#ifndef _klib_lock_h_
#define _klib_lock_h_

#include <Windows.h>

 // ͬ������
class critical_section
{
public:
    critical_section() {  InitializeCriticalSection(&m_cs);  }
    ~critical_section() {  DeleteCriticalSection(&m_cs);    }

    void lock() {  EnterCriticalSection(&m_cs);  }
    void unlock()  {  LeaveCriticalSection(&m_cs);  }

protected:
    critical_section(const critical_section&);
    critical_section& operator = (const critical_section& rhs);

protected:
    CRITICAL_SECTION m_cs;
};

// ͬ��������
class auto_lock
{
public:
    explicit auto_lock(critical_section& cs) : m_cs(cs)  {  m_cs.lock();  }
    ~auto_lock()  {  m_cs.unlock();  }

protected:
    critical_section& m_cs;
};



#endif // _klib_lock_h_