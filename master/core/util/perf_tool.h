#ifndef _klib_perf_tool_h_
#define _klib_perf_tool_h_

#include "../kthread/thread.h"
#include "../core/singleton.h"
#include "../core/perf_counter.h"
#include "../pattern/multi_buffer_queue.h"
#include <string>

using namespace klib::pattern;

namespace klib{
namespace util{


// ���ܼ��
class perf_mgr : public klib::core::singleton<perf_mgr>
{
    DECLARE_SINGLETON_CLASS(perf_mgr);
public:
    struct perf_tool_t
    {
        perf_tool_t(const char* mod_)
            : mod(mod_)
        {
        }

        ~perf_tool_t()
        {
            auto cost = counter_.delta();
            perf_mgr::instance()->post(mod, cost);
        }
        const char*    mod;
        klib::core::perf_counter counter_;
    };

    struct perf_item_t
    {
        perf_item_t() {}
        perf_item_t(perf_item_t&& other)
        {
            this->mod = std::move(other.mod);
            this->cost = other.cost;
        }
        std::string mod;
        size_t      cost;
    };

public:
    ~perf_mgr();

    // �����̣߳������ļ�
    int start(const std::string& path_, int seconds_);

    // �ر��߳�
    int stop();

    // �������ܼ������
    void post(const std::string& mod_, long us);

protected:
    void worker();

protected:
    ::klib::kthread::Thread thread_;
    multi_buffer_queue<10000, 5000, perf_item_t> buffer_;
};


#define AUTO_PERF() perf_mgr::perf_tool_t __tmp__(__FUNCTION__);
#define PERF(m)     perf_mgr::perf_tool_t __tmp__(m);



}}



#endif

