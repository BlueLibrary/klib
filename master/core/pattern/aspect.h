#ifndef _klib_aspec_h
#define _klib_aspec_h


#include <memory>
#include <iostream>
using namespace std;


namespace klib{
namespace pattern{


//----------------------------------------------------------------------
// ��������ʾ��

template <typename WrappedType, typename DerivedAspect>
class BaseAspect
{
protected:
    WrappedType* wrapped_ptr_; //��֯��Ķ���

    //��ȡ�������������
    DerivedAspect* _derived() 
    {
        return static_cast<DerivedAspect*>(this);
    }

    //��֯������ɾ�����������Զ����������е�_after����
    struct AfterWrapper
    {
        DerivedAspect* m_derived;
        AfterWrapper(DerivedAspect* derived): m_derived(derived) {};
        void operator()(WrappedType* p)
        {
            m_derived->_after(p);
        }
    };
public:
    explicit BaseAspect(WrappedType* p) :  wrapped_ptr_(p) {};

    void _before(WrappedType* p) { };
    void _after(WrappedType* p) { }

    //����ָ�����������֯�����棨_before��_after��
    std::shared_ptr<WrappedType> operator->() 
    {
        _derived()->_before(wrapped_ptr_);
        return std::shared_ptr<WrappedType>(wrapped_ptr_, AfterWrapper(_derived()));
    }
};

template <typename WrappedType, typename DerivedAspect>
class InterceptAspect
{
protected:
    WrappedType* wrapped_ptr_; //��֯��Ķ���

    //��֯������ɾ�����������Զ����������е�_after����
    struct AfterWrapper
    {
        WrappedType* wrapped_;
        AfterWrapper(WrappedType* wrapperd): wrapped_(wrapperd) {};
        void operator()(DerivedAspect* p)
        {
            p->_after(wrapped_);
        }
    };
public:
    explicit InterceptAspect(WrappedType* p) :  wrapped_ptr_(p) {};

    //��ȡ�������������
    DerivedAspect* _derived() {  return static_cast<DerivedAspect*>(this);  }
    WrappedType* _wrapped()   {  return static_cast<WrappedType*>(wrapped_ptr_);  }

    void _before(WrappedType* p) {};
    void _after(WrappedType* p) {}

    //����ָ�����������֯�����棨_before��_after��
    std::shared_ptr<DerivedAspect> operator->() 
    {
        _derived()->_before(wrapped_ptr_);
        return std::shared_ptr<DerivedAspect>(_derived(), AfterWrapper(wrapped_ptr_));
    }
};


//֯������Ĺ�������, ���ذ�����֯����������
template <template <typename> class Aspect, typename WrappedType> 
Aspect<WrappedType> MakeAspect(WrappedType* p) 
{
    return Aspect<WrappedType>(p);
}



}} // namespace



/**
ex:


/// ��װ����
template <typename WrappedType>
class LoggingAspect : public InterceptAspect<WrappedType, LoggingAspect<WrappedType> >
{
typedef  InterceptAspect<WrappedType, LoggingAspect<WrappedType> > InterceptAspect;
public:
LoggingAspect(WrappedType* p): InterceptAspect(p) {}

void _before(WrappedType* p)
{
std::cout << typeid(*p).name() <<  "---->> entering" << std::endl;
}

void _after(WrappedType* p)
{
std::cout << typeid(*p).name() << "---->> exiting" << std::endl << std::endl;
}

void g()
{
wrapped_ptr_->g();
cout << "g()" << endl;
}
};

template <typename WrappedType>
class LoggingAspectRaw : public BaseAspect<WrappedType, LoggingAspectRaw<WrappedType> >
{
typedef  BaseAspect<WrappedType, LoggingAspectRaw<WrappedType> > BaseAspect;
public:
LoggingAspectRaw(WrappedType* p): BaseAspect(p) {}

void _before(WrappedType* p) 
{
std::cout << typeid(*p).name() <<  "---->> entering" << std::endl;
}

void _after(WrappedType* p) 
{
std::cout << typeid(*p).name() << "---->> exiting" << std::endl << std::endl;
}

};



// ����
class IX
{
public:
IX(){}
virtual ~IX(){}

virtual void g()=0;
private:

};

class X : public IX
{
public:
void g() 
{
std::cout << "it is a test" << std::endl;
}

void test_echo()
{
std::cout << "test_echo :: hello world" << std::endl;
}

void test_param(char* pszParam)
{
std::cout << "test_param :: " << pszParam << std::endl;
}
};

// ����
void TestAop()
{
std::shared_ptr<IX> p(new X());

//MakeAspect<LoggingAspect>(p.get())->g();

cout << endl;
X object;
MakeAspect<LoggingAspectRaw>(&object)->test_echo();

auto proxy = MakeAspect<LoggingAspectRaw>(&object);
proxy->test_echo();
proxy->g();
proxy->test_param("����!!!");

MakeAspect<LoggingAspect>(p.get())->g();
}



*/

#endif
