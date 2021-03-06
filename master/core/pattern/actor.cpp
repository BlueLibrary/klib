#include "actor.h"
#include "../core/scope_guard.h"

using namespace klib::kthread;
using namespace klib::core;

namespace klib {
namespace pattern{
namespace actor{


engine::engine() : is_stop_(false)
{

}

engine::~engine() 
{
    this->stop();
}

bool engine::init()
{
    Thread::thread_func_type f = std::bind(&engine::engine_loop, this);
    work_thread_.start(f);

    return true;
}

bool engine::regist(passive_actor* actr) 
{
    if (actor_list_.find_item(actr))
        return false;

    return actor_list_.push_item(actr);
}

bool engine::stop()
{
    if (is_stop_) 
    {
        return true;
    }

    is_stop_ = true;

    thread_pool_.stop();

    work_thread_.exit(true);
    work_thread_.wait();

    return true;
}

void engine::add_task(passive_actor* act, size_t num)
{
    kthread_pool::func_type func = std::bind(&engine::actor_task, 
        this, 
        act, 
        num);

    thread_pool_.add_task(func);
};

void engine::engine_loop()
{
    size_t exec_count = 0;
    bool need_sleep = true;

    auto f = [&](passive_actor* act) -> bool
    {
        size_t queued_msg_cout = act->msg_count();
        if (0 == queued_msg_cout)
        {
            act->handle();
            return true;
        }

        if (act->get_is_dispatching()) 
            return true;

        act->set_is_dispatching(true);
        exec_count = queued_msg_cout > 10 ? 10 : queued_msg_cout;
        add_task(act, exec_count);

        need_sleep = false;

        return true;
    };

    while (!this->is_stop_)
    {
        need_sleep = true;
        actor_list_.for_each(f);

        if (need_sleep) 
        {
            Sleep(1000);
        }
    }
}

void engine::actor_task(passive_actor* act, size_t exec_num)
{
    ON_SCOPE_EXIT( act->set_is_dispatching(false); );

    size_t counter = 0;
    bool ret = false;
    while (counter < exec_num && (ret = act->handle()))
    {
        ++ counter;
    }

    while (thread_pool_.get_idle_thread_count() > 0 && 
        ret &&
        act->handle()) ;
}

}}}


