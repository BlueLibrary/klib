
#ifndef _klib_mem_check_h
#define _klib_mem_check_h

#include <map>
#include <set>
#include <string>
#include <windows.h>

#include "../include/allocator.h"
#include "../include/lock.h"
#include "addr_mgr.h"



/**
 1��֧������������
 2��֧���ڴ������(��Ҫ���ʹ��˵��)
 3�����ͳ����Ϣ 
*/

namespace klib {
namespace debug {



// ����ڴ������
class class_mem_mgr
{
public:
    struct class_mem_info
    {
        class_mem_info() : ncurr_size(0), nmax_size(0), nitem_size(0), nitem_count(0), nalloc_count(0), nfree_count(0)
           { class_name[0] = 0; }

        bool operator < (const class_mem_info& other)
        {
            if (strcmp(this->class_name, other.class_name) < 0) 
            {
                return true;
            }

            return false;
        }

        char class_name[50];
        size_t ncurr_size;
        size_t nmax_size;

        size_t nitem_size;
        size_t nitem_count;

        size_t nalloc_count;
        size_t nfree_count;
    };


    ~class_mem_mgr();

    static class_mem_mgr* instance();
    auto_cs& get_lock();

    void add_class_mem(const char* class_name, void* p, size_t nsize);
    void del_class_mem(const char* class_name, void* p);
    std::string summary();

protected:
    std::map<const char*,
            class_mem_info,
            std::less<const char*>, 
            MemAlloc<std::pair<const char*, class_mem_info> >> m_class_infos;

    addr_mgr addr_mgr_;
    auto_cs  m_cs;
};

// ȫ���ڴ�ͳ��
class global_mem_mgr
{
public:
    // global memory info
    struct global_mem_info
    {
        global_mem_info() 
            : ncurr_size(0)
            , nmax_size(0)
            , nalloc_count(0)
            , nfree_count(0) 
        {}

        size_t ncurr_size;           // ��ǰ��С
        size_t nmax_size;            // ����С

        size_t nalloc_count;          // ����Ĵ���
        size_t nfree_count;          // �ͷŵĴ���
    };

    static global_mem_mgr* instance();
    auto_cs& get_lock();

    void add_global_mem(void* p, size_t nsize);
    void del_global_mem(void* p);

    std::string summary() const;
    std::string summary_xml(const char* module_name) const; // ��ʽ����xml��ʽ
    
protected:
    addr_mgr addr_mgr_;
    mutable auto_cs  m_cs;
    global_mem_info mem_info_;
};

class mem_imp
{
public:
    static void* alloc_class(const char* class_name, size_t nsize); // �������С
    static void* alloc_global(size_t nsize); // ȫ�ֵ�

    static void free_class(const char* class_name, void* p);
    static void free_global(void* p);

    static void enable_stats(bool benable);
    static std::string summary();   // ���ܣ��������ڴ棬�������ʹ���ڴ�
};


}}


// �����Ƿ������ڴ�ͳ��
#define  ENABLE_MEMORY_STATS   1

#ifdef ENABLE_MEMORY_STATS

// ͳ������ڴ�ʹ�ã���class��������
#define  STATS_CLASS_MEMORY(theClass)                                    \
                                                                         \
    void* operator new(size_t obj_size)                                  \
    {                                                                    \
        return mem::mem_imp::alloc_class(#theClass, obj_size);           \
    }                                                                    \
                                                                         \
    void* operator new[](size_t arr_size)                                \
    {                                                                    \
        return mem::mem_imp::alloc_class(#theClass, arr_size);           \
    }                                                                    \
                                                                         \
    void operator delete(void* obj_addr)                                 \
    {                                                                    \
        mem::mem_imp::free_class(#theClass, obj_addr);                   \
    }                                                                    \
                                                                         \
    void operator delete[](void* arr_addr)                               \
    {                                                                    \
        mem::mem_imp::free_class(#theClass, arr_addr);                   \
    }



// ͳ��ȫ�ֵ��ڴ�ʹ�ã���һ���ط����弴��
#define  STATS_GLOBAL_MEMORY()                                               \
    void* operator new(size_t type_size)                                     \
    {                                                                        \
        return ::mem::mem_imp::alloc_global(type_size);                      \
    }                                                                        \
                                                                             \
    void* operator new[](size_t type_size)                                   \
    {                                                                        \
        return ::mem::mem_imp::alloc_global(type_size);                      \
    }                                                                        \
                                                                             \
    void * operator new(size_t type_size, const _STD nothrow_t&)             \
    {                                                                        \
        return ::mem::mem_imp::alloc_global(type_size);                      \
    }                                                                        \
                                                                             \
    void * operator new[](size_t type_size, const _STD nothrow_t&)           \
    {                                                                        \
        return ::mem::mem_imp::alloc_global(type_size);                      \
    }                                                                        \
                                                                             \
    void operator delete(void* obj_ptr)                                      \
    {                                                                        \
        ::mem::mem_imp::free_global(obj_ptr);                                \
    }                                                                        \
                                                                             \
    void operator delete[](void* obj_ptr)                                    \
    {                                                                        \
        ::mem::mem_imp::free_global(obj_ptr);                                \
    }                                                                        \
                                                                             \
    void  operator delete(void * obj_ptr, const _STD nothrow_t&)             \
    {                                                                        \
        ::mem::mem_imp::free_global(obj_ptr);                                \
    }                                                                        \
                                                                             \
    void  operator delete[](void *obj_ptr, const _STD nothrow_t&)            \
    {                                                                        \
        ::mem::mem_imp::free_global(obj_ptr);                                \
    }                                                                         

#else

#define STATS_CLASS_MEMORY(theClass)
#define STATS_GLOBAL_MEMORY()

#endif




#endif
