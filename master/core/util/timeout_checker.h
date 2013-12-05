#ifndef _KLIB_TIMEOUT_CHECKER_H_
#define _KLIB_TIMEOUT_CHECKER_H_


#include <functional>

namespace klib{
namespace util {



/*
 ��ʱ�����
*/

template <typename KeyType>
class timeout_checker
{
public:
    struct st_timeout_info
    {
        KeyType key_;
        UINT64  time_stamp_;

        st_timeout_info() : time_stamp_(0) { }
        st_timeout_info(KeyType& key) : key_(key), time_stamp_(0) {}

        bool operator < (const st_timeout_info& other) const
        {
            return this->time_stamp_ < other.time_stamp_;
        }
    };

    typedef std::set<st_timeout_info> TimeoutSetType;
    typedef std::function<void(const KeyType& key)> CallBack;

    //------------------------------------------------------------------------
    // �ж��Ƿ�Ϊ��
    bool empty() const 
    {
        return timeout_set_.empty();
    }
    
    //------------------------------------------------------------------------
    // ���볬ʱ��
    UINT64 insert(KeyType key)
    {
        st_timeout_info tmout_info(key);
        tmout_info.time_stamp_ = GetTickCount64();
        TimeoutSetType::_Pairib itr_pair;

        itr_pair = timeout_set_.insert(tmout_info);
        while (!itr_pair.second) 
        {
            ++ tmout_info.time_stamp_;
            itr_pair = timeout_set_.insert(tmout_info);
        }

        return tmout_info.time_stamp_;
    }

    //------------------------------------------------------------------------
    // ͨ������ʱ���صĳ�ʱʱ�������ɾ����ʱ��
    bool remove(UINT64 utime_stamp)
    {
        st_timeout_info key;
        key.time_stamp_ = utime_stamp;
        timeout_set_.erase(key);
        return true;
    }

    //------------------------------------------------------------------------
    // ��ʱʱ��:tmout ��λΪ����
    void check(UINT64 tmout, const CallBack& fun)
    {
        if (timeout_set_.empty())  {  return;   }

        UINT64 utime_now = GetTickCount64();
        do 
        {
            // �����ʱ�ļ�¼
            auto itr_tmout = timeout_set_.begin();
            if (itr_tmout->time_stamp_ + tmout < utime_now) 
            {
                fun(itr_tmout->key_);
                timeout_set_.erase(itr_tmout);
            }
            else
            {
                break;
            }

        } while (!timeout_set_.empty());
    }

    //------------------------------------------------------------------------
    // ����Ƿ����ڼ���ʱ����
    bool is_limit(UINT64 check_interval) const 
    {
        static UINT64 ulast_check_time = 0;                         // �ϴμ���ʱ��
        UINT64 utime_now = GetTickCount64();                        // ��ǰϵͳ����ʱ��
        
        // �ж��Ƿ���Ҫ���г�ʱ���
        UINT64 unext_check_time = ulast_check_time + check_interval;
        if (utime_now < unext_check_time) 
        {
            return true;
        }

        // �����ϴμ��ʱ��
        ulast_check_time = utime_now;

        return false;
    }

protected:
    std::set<st_timeout_info> timeout_set_;         ///< ���泬ʱ��Ϣ

};


}}


#endif // _KLIB_TIMEOUT_CHECKER_H_