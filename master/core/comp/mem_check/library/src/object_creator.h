#ifndef _klib_object_creator_h_
#define _klib_object_creator_h_


#include "mem_lib.h"


template<class T>
class object_creator
{
public:
    typedef object_creator<T>       self_type;
    typedef unsigned long long *    pointer_type;

    object_creator() 
        : m_file_name(nullptr) 
        , m_type_name(nullptr)
        , m_file_line(0)
    {}

    object_creator(const char* file_name, const char* tp_name, size_t line)
        : m_file_name(file_name)
        , m_type_name(tp_name)
        , m_file_line(line)
    {}

#define  check_get_ptr(Type)  \
    T* ptr = (T*)mem_lib::instance()->alloc_global(sizeof(T), to_desc()); \
    if (nullptr == ptr)         \
    {                           \
        return ptr;             \
    }

public:
    T * create()
    {
        check_get_ptr(T);

        new (ptr) T;
        return ptr;
    }

    template <class tr1>
    T * create(tr1 t1)
    {
        check_get_ptr(T);

        new (ptr) T(t1);
        return ptr;
    }

    template <class tr1, class tr2>
    T * create(tr1 t1, tr2 t2)
    {
        check_get_ptr(T);

        new (ptr) T(t1, t2);
        return ptr;
    }

    template <class tr1, class tr2, class tr3>
    T * create(tr1 t1, tr2 t2, tr3 t3)
    {
        check_get_ptr(T);

        new (ptr) T(t1, t2, t3);
        return ptr;
    }

    template <class tr1, class tr2, class tr3, class tr4>
    T * create(tr1 t1, tr2 t2, tr3 t3, tr4 t4)
    {
        check_get_ptr(T);

        new (ptr) T(t1, t2, t3, t4);
        return ptr;
    }

    template <class tr1, class tr2, class tr3, class tr4, class tr5>
    T * create(tr1 t1, tr2 t2, tr3 t3, tr4 t4, tr5 t5)
    {
        check_get_ptr(T);

        new (ptr) T(t1, t2, t3, t4, t5);
        return ptr;
    }

    template <class tr1, class tr2, class tr3, class tr4, class tr5, class tr6>
    T * create(tr1 t1, tr2 t2, tr3 t3, tr4 t4, tr5 t5, tr6 t6)
    {
        check_get_ptr(T);

        new (ptr) T(t1, t2, t3, t4, t5, t6);
        return ptr;
    }

protected:
    const char* to_desc()
    {
        if (nullptr == m_file_name)
        {
            return nullptr;
        }

        char* cur_pos = (char*) strrchr(m_file_name, '\\');
        if (nullptr == cur_pos)
        {
            cur_pos = strncpy(desc_buff, m_file_name, _countof(desc_buff));
        }
        else 
        {
            ++ cur_pos;
            cur_pos = strncpy(desc_buff, cur_pos, _countof(desc_buff));
        }

        cur_pos = strcat(cur_pos, ":");
        cur_pos = strcat(cur_pos, m_type_name);
        cur_pos = strcat(cur_pos, ":");
        
        char tmp[20];
        _itoa(m_file_line, tmp, 10);
        cur_pos = strcat(cur_pos, tmp);

        return desc_buff;
    }

protected:
    const char*  m_file_name;
    const char*  m_type_name;
    size_t       m_file_line;
    char         desc_buff[1024];
};


//@todo ��Ҫ��¼������������������С
#define Type(t)  object_creator<t>(##__FILE__, #t, ##__LINE__)



#endif // _klib_object_creator_h_