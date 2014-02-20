
#ifndef _CSDCS_MULTI_BUFFER_QUEUE_H_
#define _CSDCS_MULTI_BUFFER_QUEUE_H_

#include <queue>
#include <list>

#include "..\kthread\auto_lock.h"


typedef klib::kthread::mutex MutexType;
typedef klib::kthread::guard MutexGuardType;

namespace klib {
namespace pattern {


/*
 ��д��������
 MaxElement : ��������洢����
 QueueNum��   �ڲ����еĸ���
 ElemType:    Ҫ�洢������
*/
template<size_t MaxElement, size_t QueueNum, typename ElemType, typename Container = std::queue<ElemType>>
class  multi_buffer_queue
{
public:
    multi_buffer_queue() : 
      read_index_(0),
      write_index_(0)
    {
        size_t max_ele = MaxElement;
        size_t queue_num = QueueNum;
        size_t tmp = (max_ele % queue_num) > 0;  // �˾䲻�ܺ�����һ��ϲ�
        max_queue_size_ = max_ele/queue_num + tmp;
        static_assert(QueueNum >= 2, "need more than 2 queue");
    }

    MutexType& mutex() { return mutex_; }

    size_t size() const 
    {
        size_t num = 0;
        for (size_t index = 0; index<QueueNum; ++index) 
        {
            num += container_[index].size();
        }
        return num; 
    }

    bool push(ElemType elem)
    {
        MutexGuardType guard(mutex_);

        return this->_push(elem);
    }

    bool push(std::list<ElemType>& elst)
    {
        MutexGuardType guard(mutex_);

        for (auto itr=elst.begin(); itr != elst.end(); ++itr)
        {
            this->_push(*itr);
        }
        return true;
    }

    bool pop(ElemType& e)
    {
        // ��ǰ������Ϊ�յĻ�����Ҫ�ƶ�������
        if (container_[read_index_].empty()) 
        {
            ++ read_index_;
            read_index_ %= QueueNum;
            if (container_[read_index_].empty()) 
            {
                //CSASSERT(FALSE);
                return false;
            }
        }

        // ��ȡ������һ�µĻ�����Ҫ�ƶ�д������
        if (read_index_ == write_index_) 
        {
            MutexGuardType guard(mutex_);

            if (read_index_ == write_index_) 
            {
                write_index_ = (write_index_ + 1) % QueueNum;
            }
        }

        // ��ȡ����
        e = container_[read_index_].front();
        container_[read_index_].pop();
        
        return true;
    }

protected:
    bool _push(ElemType elem)
    {
        // ��ǰ�������û��д��
        if (container_[write_index_].size() < max_queue_size_) 
        {
            container_[write_index_].push(elem);
        }
        else
        {
            // д�����ƶ�����һ������
            size_t next_write_index = (write_index_ + 1) % QueueNum;
            if (next_write_index == read_index_) 
            {
                return false;
            }

            write_index_ = next_write_index;
            container_[write_index_].push(elem);
        }

        return true;
    }

protected:
    MutexType       mutex_;                             // ������

    Container       container_[QueueNum];               // ���ж���
    size_t          max_queue_size_;                    // ÿ�����еĴ�С
    size_t          read_index_;                        // ������
    size_t          write_index_;                       // д����
};



}}



#endif  // _CSDCS_MULTI_BUFFER_QUEUE_H_
