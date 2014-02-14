
#ifndef _CSDCS_MULTI_BUFFER_QUEUE_H_
#define _CSDCS_MULTI_BUFFER_QUEUE_H_

#include <queue>

#include "..\kthread\auto_lock.h"


typedef klib::kthread::mutex MutexType;
typedef klib::kthread::guard MutexGuardType;

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
      write_index_(0),
      element_count_(0)
    {
        size_t max_ele = MaxElement;
        size_t queue_num = QueueNum;
        size_t tmp = (max_ele % queue_num) > 0;  // �˾䲻�ܺ�����һ��ϲ�
        max_queue_size_ = max_ele/queue_num + tmp;
        static_assert(QueueNum >= 2, "need more than 2 queue");
    }

    MutexType& mutex() { return mutex_; }

    size_t size() const { return element_count_; }

    bool push(const ElemType& elem)
    {
        MutexGuardType guard(mutex_);

        // ��ǰ�������û��д��
        if (container_[write_index_].size() < max_queue_size_) 
        {
            ++ element_count_;
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

            ++ element_count_;
            write_index_ = next_write_index;
            container_[write_index_].push(elem);
        }

        return true;
    }

    bool pop(ElemType& e)
    {
        if (0 == element_count_) 
        {
            return false;
        }

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

        // �������ݼ���
        MutexGuardType guard(mutex_);
        -- element_count_;

        return true;
    }

protected:
    MutexType       mutex_;                             // ������

    Container       container_[QueueNum];               // ���ж���
    size_t          max_queue_size_;                    // ÿ�����еĴ�С
    size_t          read_index_;                        // ������
    size_t          write_index_;                       // д����
    size_t          element_count_;                     // Ԫ�صĸ���
};


#endif  // _CSDCS_MULTI_BUFFER_QUEUE_H_
