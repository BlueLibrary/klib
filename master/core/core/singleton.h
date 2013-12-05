#pragma once

#include "klib_core.h"


namespace klib{
namespace core{
    

///< ����ģʽ
template<class T> 
class singleton : public klib::core::noncopyable
{   
public:
    ///< ��ȡΨһʵ������
    static T* instance()
    {   
        //T��һ��protectedĬ�Ϲ��캯��
        static T _instance; 
        return & _instance;
    }

};

// ����һ��Ϊ������
#define DECLARE_SINGLETON_CLASS(T)      \
private:                                \
    friend singleton<T>;               \
    T(){}                               \
    T(T&);                              \
public:


}}

