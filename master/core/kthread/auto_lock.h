
#if !defined(_Klib_AutoLock_H)
#define _Klib_AutoLock_H

#ifdef _MSC_VER
#include <windows.h>
#endif


namespace klib {
namespace kthread {

#ifdef _MSC_VER

class auto_cs
{
protected:
    auto_cs(const auto_cs&other);
    auto_cs operator = (const auto_cs& other);

public:
    auto_cs () { InitializeCriticalSection(&m_cs); }
    ~auto_cs () { DeleteCriticalSection(&m_cs); }

    CRITICAL_SECTION* get() {return &m_cs;}
    operator CRITICAL_SECTION* () { return &m_cs;}

    void lock()   { EnterCriticalSection(&m_cs); }
    void unlock() { LeaveCriticalSection(&m_cs); }

protected:
    CRITICAL_SECTION  m_cs;
};
    
//�ٽ����߳�ͬ��������RAII
//�ڶ������ʱ���Զ��������ڶ���������ʱ���Զ�����
//�÷���
//  auto_lock guard(m_auto_cs);
class auto_lock 
{
public:
	auto_lock (auto_cs& cs) : m_auto_cs(cs) 
    {
		m_auto_cs.lock();
	}
	
	~auto_lock() {
		m_auto_cs.unlock();
	}
	
protected:
    auto_cs& m_auto_cs;           ///< �ٽ�������
};


// ���廥����������
typedef auto_cs   mutex;
typedef auto_lock guard;


#endif


}}


#endif //