#ifndef _klib_timer_mgr_h
#define _klib_timer_mgr_h

#include <windows.h>
#include <set>
#include <map>


#include "../kthread/auto_lock.h"
#include "../kthread/thread.h"
using namespace klib::kthread;


namespace klib {
namespace core {

// ����false��ʾ��Ҫ��ɾ��
// ����true��ʾ��Ҫ����
typedef std::function<bool(void)> timer_func_type;

// ��ʱ����Ϣ
struct lessfun;
class  timer_mgr;
struct timer
{
    timer() : bdelete(false), utimer_id(0), wakeup_time(0), uSpan(0)
    {
        #ifdef _DEBUG
        invoke_times = 0;
        #endif
    }
    
    friend lessfun;
    friend timer_mgr;

    /// function
    #ifdef _DEBUG
    UINT32 invoked_times() const { return invoke_times; }
    #endif

    UINT32 id() const            { return utimer_id;    }
    
protected:
    bool            bdelete;        // ��ʾ�Ƿ�ɾ����
    UINT32          utimer_id;      // ID
    UINT64          uSpan;          // ����
    UINT64          wakeup_time;    // �´λ��ѵ�ʱ��
    
    timer_func_type    fun;            // fun

    #ifdef _DEBUG
    UINT32          invoke_times;   // ���д���
    #endif
};


// ��ʱ��ʵ��
class timer_mgr
{
public:

public:
    timer_mgr();
    ~timer_mgr();

    timer* add(UINT64 uSpan, timer_func_type fun);
    bool remove(timer* tmr);

    bool start();
    void stop();

    size_t size() const;

protected:
    void worker();

protected:
    struct lessfun
    {
        bool operator()(timer* left, timer* right)
        {
            if (left->wakeup_time < right->wakeup_time)
                return true;

            if (left->wakeup_time > right->wakeup_time)
                return false;

            return left->utimer_id < right->utimer_id;
        }
    };


protected:
    std::set<timer*, lessfun> timer_heap_;

    Thread          thread_;
    UINT64          lastTick_;
    UINT32          timer_id_;
    auto_cs         m_sec;
};

}}


#endif
