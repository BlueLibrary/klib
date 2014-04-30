#pragma once

#include "../kthread/auto_lock.h"


namespace klib {
namespace pattern {
    


/**
 * @brief ��������
 * @tparam  T  ��������
 * @tparam  max_object_size ��󳤶ȣ����Ϊ0�����ʾ������
 */
template<class T, int max_object_size = 0>
class object_reuser
{
public:
    typedef object_reuser<T, max_object_size>       self_type;
    typedef unsigned long long *    pointer_type;
    
public:
    object_reuser()
    {
        m_pHead = new char[sizeof(pointer_type) + sizeof(T)];
        m_pTail = new char[sizeof(pointer_type) + sizeof(T)];
        *((unsigned long long *)m_pHead) = (unsigned long long)m_pTail;
        *((unsigned long long *)m_pTail) = NULL;

        m_nused_size = 0;
        m_nfree_size = 0;
        m_ntotal_size = 0;
    }

    /**
     * @brief ������ʱ����Ҫ�����ͷ��ڴ�
     */
    virtual ~object_reuser()
    {
        while (m_nfree_size > 0)
        {
            char *p = (char *)m_pHead;
            m_pHead = (void *)*((unsigned long long *)m_pHead);

            T* ptype = (T*)(p + sizeof(pointer_type));
            ptype->~T();

            delete p;

            -- m_nfree_size;
        }
    }

    bool empty() const
    {
        return  m_nused_size == 0;
    }

    size_t size() const
    {
        return m_nused_size;
    }

    size_t free_size() const
    {
        return m_nfree_size;
    }

    size_t total_size() const
    {
        return m_ntotal_size;
    }

    T * create_object()
    {
        if ( m_nfree_size == 0)
        {
            if (!add_to_pool()) return NULL;
        }

        char *p = (char *)m_pHead;
        m_pHead = (void *)*((unsigned long long *)m_pHead);
        ++ m_nused_size;
        -- m_nfree_size;
        return new (p + sizeof(pointer_type)) T;
    }

    template <class tr1>
    T * create_object(tr1 t1)
    {
        if ( m_nfree_size == 0)
        {
            if (!add_to_pool()) return NULL;
        }

        char *p = (char *)m_pHead;
        m_pHead = (void *)*((unsigned long long *)m_pHead);
        ++ m_nused_size;
        -- m_nfree_size;
        return new (p + sizeof(pointer_type)) T(t1);
    }

    template <class tr1, class tr2>
    T * create_object(tr1 t1, tr2 t2)
    {
        if ( m_nfree_size == 0)
        {
            if (!add_to_pool()) return NULL;
        }

        char *p = (char *)m_pHead;
        m_pHead = (void *)*((unsigned long long *)m_pHead);
        ++ m_nused_size;
        -- m_nfree_size;
        return new (p + sizeof(pointer_type)) T(t1, t2);
    }

    template <class tr1, class tr2, class tr3>
    T * create_object(tr1 t1, tr2 t2, tr3 t3)
    {
        if ( m_nfree_size == 0)
        {
            if (!add_to_pool()) return NULL;
        }

        char *p = (char *)m_pHead;
        m_pHead = (void *)*((unsigned long long *)m_pHead);
        ++ m_nused_size;
        -- m_nfree_size;
        return new (p + sizeof(pointer_type)) T(t1, t2, t3);
    }

    template <class tr1, class tr2, class tr3, class tr4>
    T * create_object(tr1 t1, tr2 t2, tr3 t3, tr4 t4)
    {
        if ( m_nfree_size == 0)
        {
            if (!add_to_pool()) return NULL;
        }
        char *p = (char *)m_pHead;
        m_pHead = (void *)*((unsigned long long *)m_pHead);
        ++ m_nused_size;
        -- m_nfree_size;
        return new (p + sizeof(pointer_type)) T(t1, t2, t3, t4);
    }

    template <class tr1, class tr2, class tr3, class tr4, class tr5>
    T * create_object(tr1 t1, tr2 t2, tr3 t3, tr4 t4, tr5 t5)
    {
        if ( m_nfree_size == 0)
        {
            if (!add_to_pool()) return NULL;
        }
        char *p = (char *)m_pHead;
        m_pHead = (void *)*((unsigned long long *)m_pHead);
        ++ m_nused_size;
        -- m_nfree_size;
        return new (p + sizeof(pointer_type)) T(t1, t2, t3, t4, t5);
    }
    
    /**
     * @brief ����ö���
     */
    void recyle_object(T *p)
    {
        p->~T();

        -- m_nused_size;
        add_to_pool((char *)p - sizeof(pointer_type));
    }

protected:
    void * m_pHead;
    void * m_pTail;

    size_t m_nused_size;
    size_t m_nfree_size;
    size_t m_ntotal_size;
};


using klib::kthread::auto_cs;
using klib::kthread::auto_lock;

/**
 * @brief ͨ��װ��ģʽ�������
 */
template<class T, int max_object_size = 0>
class object_reuser_sync : public object_reuser<T, max_object_size>
{
public:
    typedef object_reuser<T, max_object_size>  parent_type;

public:
    T * create_object()
    {
        auto_lock lock(m_obj_cs);

        return parent_type::create_object();
    }

    template <class tr1>
    T * create_object(tr1 t1)
    {
        auto_lock lock(m_obj_cs);

        return parent_type::create_object(t1);
    }

    template <class tr1, class tr2>
    T * create_object(tr1 t1, tr2 t2)
    {
        auto_lock lock(m_obj_cs);

        return parent_type::create_object(t1, t2);
    }

    template <class tr1, class tr2, class tr3>
    T * create_object(tr1 t1, tr2 t2, tr3 t3)
    {
        auto_lock lock(m_obj_cs);

        return parent_type::create_object(t1, t2, t3);
    }

    template <class tr1, class tr2, class tr3, class tr4>
    T * create_object(tr1 t1, tr2 t2, tr3 t3, tr4 t4)
    {
        auto_lock lock(m_obj_cs);

        return parent_type::create_object(t1, t2, t3, t4);
    }

    template <class tr1, class tr2, class tr3, class tr4, class tr5>
    T * create_object(tr1 t1, tr2 t2, tr3 t3, tr4 t4, tr5 t5)
    {
        auto_lock lock(m_obj_cs);

        return parent_type::create_object(t1, t2, t3, t4, t5);
    }
    
    /**
     * @brief ����ö���
     */
    void recyle_object(T *p)
    {
        auto_lock lock(m_obj_cs);

        return parent_type::recyle_object(p);
    }
    
protected:    
    /**
     * @brief ���ڴ���ӵ����ӱ���ȥ
     * @param p   Ҫ���õ�ָ�룬���Ϊ������Ҫ�����ڴ�
     */
    bool add_to_pool(void *p = NULL)
    {
        if (p == NULL)
        {
            // ����ܵĴ�С�����趨����󳤶ȵĻ��Ͳ��������������
            if (max_object_size > 0 && max_object_size < m_ntotal_size) 
            {
                return false;
            }

            // ����Ҫ���������ڴ�ռ�
            p = new char[sizeof(pointer_type) + sizeof(T)];
            if (NULL == p) 
            {
                return false;
            }

            // �����ܵĶ������
            ++ m_ntotal_size;
        }

        *(unsigned long long *)p = NULL;
        *(unsigned long long *)m_pTail = (unsigned long long)p;
        m_pTail = p;
        ++ m_nfree_size;

        return true;
    }

public:    
    klib::kthread::auto_cs  m_obj_cs;
};



}}

