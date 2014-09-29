#pragma once


#include "../../dll/include/lock.h"
#include "../../dll/include/mem_interface.h"
#include "dll_loader.h"


#ifdef _WIN32
#define library_dll_ame "mem_check_dll.dll"
#else
#define library_dll_ame "mem_check_dll.so"
#endif

/*
��Ҫ���� redefine_new.hͷ�ļ� 
#define GC_NEW new(__FILE__, __LINE__)  
#define new GC_NEW  
*/

class mem_lib
{
    mem_lib(void);
public:
    ~mem_lib(void);

    static mem_lib* instance();

    void* alloc_global(size_t type_size, const char* desc = nullptr);
    void  free_global(void*);
    
    void enable_stats(bool benable);
    void set_desc(const char* desc);

    static const char* to_str(char* buff, 
        size_t buf_size,
        const char* file, 
        size_t line, 
        size_t size_type);

protected:
    static mem_lib*     m_instance;
    addr_mgr*           m_addr_mgr;
    mem_interface*      m_mem_i;
    dll_loader          m_dll_loader;
};


// �����Ƿ������ڴ�ͳ��
#define  ENABLE_MEMORY_STATS   1

#ifdef ENABLE_MEMORY_STATS
    #include <new>


// ��������
void* operator new(size_t type_size, const char *file, int line ) ;

// ͳ��ȫ�ֵ��ڴ�ʹ�ã���һ���ط����弴��
#define  stats_global_memory_define()                                        \
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
    void* operator new(size_t type_size, const char *file, int line )        \
    {                                                                        \
        char buff[120];                                                       \
        const char* str = mem_lib::to_str(buff, sizeof(buff), file, line, type_size);             \
        return mem_lib::instance()->alloc_global(type_size, str);                 \
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
#define stats_global_memory_define()

#endif


#define  set_memory_desc(desc)      \
    mem_lib::instance()->set_desc(desc); 