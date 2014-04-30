#ifndef _klib_ptr_list_h_
#define _klib_ptr_list_h_


namespace klib {
namespace pattern {


/*
T must include T* next member and T * prev
˫��������ɾ��ʹ��
����һ���������ӣ������������ڴ�Ŀ��ǣ��뱻ʹ�õĶ���T�г�Ա�ϵ����
*/

template<typename T>
class ptr_list
{
public:
    ptr_list() : 
        head_(nullptr),
        tail_(nullptr),
        size_(0)
    {
    }

    size_t size() const { return size_; }

    bool push_back(T* t)
    {
        if (nullptr == t)
            return false;

        if (nullptr == tail_)
        {
            t->prev = nullptr;
            t->next = nullptr;
            head_ = tail_ = t;
        }
        else 
        {
            t->prev = tail_;
            t->next = nullptr;
            tail_->next = t;
        }

        ++ size_;

        return true;
    }

    bool push_front(T* t)
    {
        if (nullptr == t)
            return false;

        if (nullptr == head_)
        {
            t->prev = head_;
            t->next = nullptr;
            head_ = tail_ = t;
        }
        else
        {
            t->prev = nullptr;
            t->next = head_;
            head_->prev = t;
            head_ = t;
        }

        ++ size_;
        return true;
    }

    // ����ͨ�����ص�ָ����б���
    T* front() const
    {
        if (0 == size())
            return nullptr;

        return head_;
    }

    void pop_front()
    {
        if (nullptr == head_)
            return;

        if (1 == size())
        {
            head_ = tail_ = nullptr;
        }
        else
        {
            head_->next->prev = nullptr;
            head_ = head_->next;
        }
        -- size_;
    }

    bool remove(T* t)
    {
        if (nullptr == head_)
            return false;

        if (1 == size())
        {
            if (head_ == t)
            {
                head_ = tail_ = nullptr;
                -- size_;
                return true;
            }
            return false;
        }
        else 
        {
            if (head_ == t)
            {
                head_->next->prev = nullptr;
                head_ = head_->next;
            }
            else if (tail_ == t)
            {
                tail_ = t->prev;
                tail_->next = nullptr;
            }
            else
            {
                t->next->prev = t->prev;
                t->prev->next = t->next;
            }
        }

        -- size_;
        return true;
    }

protected:
    T* head_;
    T* tail_;
    size_t size_;
};

}}



#endif  // _klib_ptr_list_h_