
#ifndef _klib_mem_check_h
#define _klib_mem_check_h

#include <map>
#include <set>
#include <string>
#include <windows.h>

namespace klib {
namespace mem {
namespace check {

 // ͬ������
class auto_cs
{
public:
    auto_cs() {  InitializeCriticalSection(&m_cs);  }
    ~auto_cs() {  DeleteCriticalSection(&m_cs);    }

    void lock() {  EnterCriticalSection(&m_cs);  }
    void unlock()  {  LeaveCriticalSection(&m_cs);  }

protected:
    auto_cs(const auto_cs&);
    auto_cs& operator = (const auto_cs& rhs);

protected:
    CRITICAL_SECTION m_cs;
};

// ͬ��������
class auto_lock
{
public:
    explicit auto_lock(auto_cs& cs) : m_cs(cs)  {  m_cs.lock();  }
    ~auto_lock()  {  m_cs.unlock();  }

protected:
    auto_cs& m_cs;
};

// �ڴ������
template<typename T>
class MemAlloc : public std::allocator<T>
{  
public:  
    typedef size_t   size_type;  
    typedef typename std::allocator<T>::pointer              pointer;  
    typedef typename std::allocator<T>::value_type           value_type;  
    typedef typename std::allocator<T>::const_pointer        const_pointer;  
    typedef typename std::allocator<T>::reference            reference;  
    typedef typename std::allocator<T>::const_reference      const_reference;  
   
    pointer allocate(size_type _Count, const void* _Hint = NULL)
    {
        (_Hint);
        return (pointer) malloc(_Count * sizeof (T));
    }
  
    void deallocate(pointer _Ptr, size_type _Count)
    {
        (_Count);
        free(_Ptr);
    }
  
    template<class _Other>  
    struct rebind  
    {   // convert this type to allocator<_Other>  
        typedef MemAlloc<_Other> other;  
    };  
  
    MemAlloc() throw()  {}   
  
    /*MyAllc(const MyAllc& __a) throw()  
        : allocator<T>(__a)  
    {}*/
  
    template<typename _Tp1>
    MemAlloc(const MemAlloc<_Tp1>&) throw()  {}
  
    ~MemAlloc() throw()  {}
};

// ͳ��
class addr_mgr
{
public:
    struct addr_info
    {
        addr_info() : nsize(0) {}

        size_t nsize;
    };

public:
    bool add_addr_info(void* p, size_t nsize);
    bool del_addr_info(void* p, size_t& nsize);

protected:
    std::map<void*, 
             addr_info, 
             std::less<void*>, 
             MemAlloc<std::pair<void*, addr_info>> >    m_addr_infos;
};

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
        global_mem_info() : ncurr_size(0), nmax_size(0), nalloc_count(0), nfree_count(0) {}

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


}}}


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



#define  CHECK_MEMORY_SWITCH(pMainService, ModuleID)            \
{                                                               \
    UINT64  uMemorySwitch;                                      \
    pMainService->GetInternalMemorySwitch(uMemorySwitch);       \
    if (!(uMemorySwitch & ModuleID))                            \
    {                                                           \
        ::mem::mem_imp::enable_stats(false);                    \
    }                                                           \
}

#endif
