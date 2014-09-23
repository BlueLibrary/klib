#pragma once


#include "../../dll/include/lock.h"
#include "../../dll/include/mem_interface.h"

#ifdef _WIN32
#define library_dll_ame "mem_check_dll.dll"
#else
#define library_dll_ame "mem_check_dll.so"
#endif


class mem_lib
{
    mem_lib(void);
public:
    ~mem_lib(void);

    static mem_lib* instance();

    void* alloc_global(size_t type_size, const char* desc = nullptr);
    void  free_global(void*);
    
    void enable_stats(bool benable);

protected:
    static mem_lib*     m_instance;
    addr_mgr*           m_addr_mgr;
    mem_interface*      m_mem_i;
    bool                m_enable_stats;
};


// �����Ƿ������ڴ�ͳ��
#define  ENABLE_MEMORY_STATS   1

#ifdef ENABLE_MEMORY_STATS

// ͳ��ȫ�ֵ��ڴ�ʹ�ã���һ���ط����弴��
#define  STATS_GLOBAL_MEMORY()                                               \
    void* operator new(size_t type_size)                                     \
    {                                                                        \
        return mem_lib::instance()->alloc_global(type_size);                 \
    }                                                                        \
                                                                             \
    void* operator new[](size_t type_size)                                   \
    {                                                                        \
        return mem_lib::instance()->alloc_global(type_size);                 \
    }                                                                        \
                                                                             \
    void * operator new(size_t type_size, const _STD nothrow_t&)             \
    {                                                                        \
        return mem_lib::instance()->alloc_global(type_size);                 \
    }                                                                        \
                                                                             \
    void * operator new[](size_t type_size, const _STD nothrow_t&)           \
    {                                                                        \
        return mem_lib::instance()->alloc_global(type_size);                 \
    }                                                                        \
                                                                             \
    void operator delete(void* obj_ptr)                                      \
    {                                                                        \
        mem_lib::instance()->free_global(obj_ptr);                           \
    }                                                                        \
                                                                             \
    void operator delete[](void* obj_ptr)                                    \
    {                                                                        \
        mem_lib::instance()->free_global(obj_ptr);                           \
    }                                                                        \
                                                                             \
    void  operator delete(void * obj_ptr, const _STD nothrow_t&)             \
    {                                                                        \
        mem_lib::instance()->free_global(obj_ptr);                           \
    }                                                                        \
                                                                             \
    void  operator delete[](void *obj_ptr, const _STD nothrow_t&)            \
    {                                                                        \
        mem_lib::instance()->free_global(obj_ptr);                           \
    }                                                                         



#else

#define STATS_CLASS_MEMORY(theClass)
#define STATS_GLOBAL_MEMORY()

#endif