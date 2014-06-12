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
    WrappedType* m_wrappedPtr; //��֯��Ķ���

    //��ȡ�������������
    DerivedAspect* GetDerived() 
    {
        return static_cast<DerivedAspect*>(this);
    }

    //��֯������ɾ�����������Զ����������е�After����
    struct AfterWrapper
    {
        DerivedAspect* m_derived;
        AfterWrapper(DerivedAspect* derived): m_derived(derived) {};
        void operator()(WrappedType* p)
        {
            m_derived->After(p);
        }
    };
public:
    explicit BaseAspect(WrappedType* p) :  m_wrappedPtr(p) {};


    void Before(WrappedType* p) {
        // Default does nothing
    };

    void After(WrappedType* p) {
        // Default does nothing
    }

    //����ָ�����������֯�����棨Before��After��
    std::shared_ptr<WrappedType> operator->() 
    {
        GetDerived()->Before(m_wrappedPtr);
        return std::shared_ptr<WrappedType>(m_wrappedPtr, AfterWrapper(GetDerived()));
    }
};

template <typename WrappedType, typename DerivedAspect>
class InterceptAspect
{
protected:
    WrappedType* m_wrappedPtr; //��֯��Ķ���

    //��ȡ�������������
    DerivedAspect* GetDerived() 
    {
        return static_cast<DerivedAspect*>(this);
    }

    //��֯������ɾ�����������Զ����������е�After����
    struct AfterWrapper
    {
        WrappedType* m_wrappered;
        AfterWrapper(WrappedType* wrapperd): m_wrappered(wrapperd) {};
        void operator()(DerivedAspect* p)
        {
            p->After(m_wrappered);
        }
    };
public:
    explicit InterceptAspect(WrappedType* p) :  m_wrappedPtr(p) {};


    void Before(WrappedType* p) {
        // Default does nothing
    };

    void After(WrappedType* p) {
        // Default does nothing
    }

    //����ָ�����������֯�����棨Before��After��
    std::shared_ptr<DerivedAspect> operator->() 
    {
        GetDerived()->Before(m_wrappedPtr);
        return std::shared_ptr<DerivedAspect>(GetDerived(), AfterWrapper(m_wrappedPtr));
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

void Before(WrappedType* p)
{
std::cout << typeid(*p).name() <<  "---->> entering" << std::endl;
}

void After(WrappedType* p)
{
std::cout << typeid(*p).name() << "---->> exiting" << std::endl << std::endl;
}

void g()
{
m_wrappedPtr->g();
cout << "g()" << endl;
}
};

template <typename WrappedType>
class LoggingAspectRaw : public BaseAspect<WrappedType, LoggingAspectRaw<WrappedType> >
{
typedef  BaseAspect<WrappedType, LoggingAspectRaw<WrappedType> > BaseAspect;
public:
LoggingAspectRaw(WrappedType* p): BaseAspect(p) {}

void Before(WrappedType* p) 
{
std::cout << typeid(*p).name() <<  "---->> entering" << std::endl;
}

void After(WrappedType* p) 
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
