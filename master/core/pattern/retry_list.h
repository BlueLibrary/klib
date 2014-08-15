#ifndef _klib_retry_list_h_
#define _klib_retry_list_h_


#include <functional>
#include <list >
#include <memory>
#include <kthread/mutex.h>
#include <kthread/auto_lock.h>


namespace klib{
namespace pattern{


template<class T, class Ctx>
class retry_list
{
public:
    struct retry_info
    {
        retry_info()
            : retry_times(0)
            , last_retry_time(0)
        {}

        uint64_t      retry_times;        // �����ԵĴ���
        uint64_t      last_retry_time;    // �ϴ����Ե�ʱ��

        T           t;                  // ����
        Ctx         ctx;                // ������������
    };
    typedef std::shared_ptr<retry_info> retry_info_ptr;

    typedef retry_list<T, Ctx> self_type;
    typedef std::function<bool(const T&, Ctx& ctx)>  retry_func;
    typedef std::function<void(T&, Ctx& ctx)>        timeout_func;

public:
    retry_list()
        : m_retry_interval(60)
        , m_max_retry_times(3)
    {
    }

    self_type& on_retry(retry_func f);
    self_type& on_timeout(timeout_func f);
    self_type& set_retry_times(size_t retry_tms);
    self_type& set_retry_interval(size_t retry_interval);

    bool add_item(T t);             // ���һ��
    bool exec();                    // ִ��(�����Ƿ���Ҫ����ִ��)

protected:
    std::list<std::shared_ptr<retry_info>>    m_retry_list;
    klib::kthread::mutex                      m_retry_lst_lock;
    size_t                  m_max_retry_times;          // ��Ҫ���ԵĴ�����0��ʾ��һֱ���ԣ�û�г�ʱʱ����
    size_t                  m_retry_interval;           // ���Եļ������λΪ�룩

    retry_func              m_retry_func;               // ����Ҫִ�еĶ���
    timeout_func            m_timeout_func;             // ��ʱʱִ�еĶ���
};


template<class T, class Ctx>
retry_list<T, Ctx>& retry_list<T, Ctx>::on_retry(typename retry_list<T, Ctx>::retry_func f)
{
    m_retry_func = f;
    return *this;
}

template<class T, class Ctx>
retry_list<T, Ctx>& retry_list<T, Ctx>::on_timeout(typename retry_list<T, Ctx>::timeout_func f)
{
    m_timeout_func = f;
    return *this;
}

template<class T, class Ctx>
retry_list<T, Ctx>& retry_list<T, Ctx>::set_retry_times(size_t retry_tms)
{
    m_max_retry_times = retry_tms;
    return *this;
}

template<class T, class Ctx>
retry_list<T, Ctx>& retry_list<T, Ctx>::set_retry_interval(size_t retry_interval)
{
    m_retry_interval = retry_interval;
    return *this;
}

template<class T, class Ctx>
bool retry_list<T, Ctx>::add_item(T t)
{
    retry_info_ptr ptr = retry_info_ptr(new retry_info);
    if (nullptr == ptr)
    {
        return false;
    }
    ptr->t = t;

    klib::kthread::guard guard(m_retry_lst_lock);
    m_retry_list.push_front(ptr);
    return true;
}

template<class T, class Ctx>
bool retry_list<T, Ctx>::exec()
{
    if (m_retry_list.empty())
    {
        return false;
    }

    retry_info_ptr ptr;
    klib::kthread::lock_exec(m_retry_lst_lock, [&](){
    
        if (!m_retry_list.empty())
        {
            ptr = m_retry_list.front();
            m_retry_list.pop_front();
        }
    }); 
    
    if (nullptr == ptr)
    {
        return false;
    }

    uint64_t tm_now = time(nullptr);
    
    // Ϊ0��ʾ�ǵ�һ�ν�������
    // �ж��Ƿ�����һ�����Լ����
    if (tm_now - ptr->last_retry_time < this->m_retry_interval)
    {
        klib::kthread::guard guard(m_retry_lst_lock);
        m_retry_list.push_back(ptr);

        return false;
    }
    else if (0 == this->m_max_retry_times || ptr->retry_times < this->m_max_retry_times)
    {
        ptr->retry_times ++;
        ptr->last_retry_time = tm_now;

        if (!m_retry_func(ptr->t, ptr->ctx))
        {
            klib::kthread::guard guard(m_retry_lst_lock);
            m_retry_list.push_back(ptr);
        }
    }
    else 
    {
        if (m_timeout_func)
            m_timeout_func(ptr->t, ptr->ctx);
    }

    return true;
}


}}


#endif // _klib_retry_list_h_

