// Queue.h: interface for the Queue class.
//
//	1��n lock free����
//	����ģʽ��1��nд��n��1д
//	push��pop���Ӷȣ�O(1)
//////////////////////////////////////////////////////////////////////

#ifndef MDK_QUEUE_H
#define MDK_QUEUE_H


#include <stdint.h>
#include <windows.h>
#include <kthread/atomic.h>


#define  AtomDec  InterlockedDecrement
#define  AtomAdd  InterlockedIncrement  

namespace klib {
namespace stl {
namespace lockfree {

template<class T, int max_queue_size>
class one_many_queue
{
    typedef	struct QUEUE_NODE
    {
        T    object;
        bool IsEmpty;
    } QUEUE_NODE;

public:
    ~one_many_queue()
    {
    }

public:
    one_many_queue()
    {
        m_nSize = max_queue_size;
        m_nWriteAbleCount = m_nSize;
        m_nReadAbleCount = 0;
        m_push = 0;
        m_pop = 0;

        clear();
    }

    LONG size() { return m_nReadAbleCount; }

    bool push(const T& object )
    {
        if ( 0 >= m_nWriteAbleCount ) return false;//�ж�����
        if ( 0 > AtomDec(&m_nWriteAbleCount) ) //�Ѿ����㹻���push�������жӲ�ͬλ��д������
        {
            AtomAdd(&m_nWriteAbleCount);
            return false;
        }
        uint32_t pushPos = AtomAdd(&m_push);
        pushPos = pushPos % m_nSize;
        /*
        ֻ����NPop��������£���Pop������ɣ���һ��λ�õ�Popδ��ɣ�
        �����Pop���������ʾ�п�λ�������������һ��λ��ʱ����������δ��ȡ��
        ��Ϊ����ֻ����1��N�����Ա�Ȼ�ǵ��߳�Push( void *pObject )������������m_push����Ҫ��������
        */
        if (!m_queue[pushPos].IsEmpty ) 
        {
            m_push--;
            AtomAdd(&m_nWriteAbleCount);
            return false;
        }
        m_queue[pushPos].object = object;
        m_queue[pushPos].IsEmpty = false;
        AtomAdd(&m_nReadAbleCount);

        return true;
    }

    bool pop(T& t)
    {
        if ( 0 >= m_nReadAbleCount ) return NULL;//���ж�
        if ( 0 > AtomDec(&m_nReadAbleCount)) //�Ѿ����㹻���pop������ȡ�жӲ�ͬλ�õ�����
        {
            AtomAdd(&m_nReadAbleCount);
            return false;
        }
        uint32_t popPos = AtomAdd(&m_pop);
        popPos = popPos % m_nSize;
        /*
        ֻ����NPush��������£���Push������ɣ���һ��λ�õ�Pushδ��ɣ�
        �����Push���������ʾ�����ݣ������������һ��λ��ʱ������û�����ݿɶ�
        ��Ϊ����ֻ����1��N����Ȼ�ǵ��߳�Pop()������������m_pop����Ҫ��������
        */
        if ( m_queue[popPos].IsEmpty )
        {
            m_pop--;
            AtomAdd(&m_nReadAbleCount);
            return false;
        }

        t = m_queue[popPos].object;
        //m_queue[popPos].pObject = NULL;
        m_queue[popPos].IsEmpty = true;
        AtomAdd(&m_nWriteAbleCount);

        return true;
    }

    void clear()
    {
        if ( NULL == m_queue ) return;
        uint32_t i = 0;
        m_nWriteAbleCount = m_nSize;
        m_nReadAbleCount = 0;
        for ( i = 0; i < m_nSize; i++ )
        {
            //m_queue[i].object = NULL;
            m_queue[i].object.~T();
            m_queue[i].IsEmpty = true;
        }
    }

private:
    QUEUE_NODE m_queue[max_queue_size];

    uint32_t m_push;
    uint32_t m_pop;
    uint32_t m_nSize;
    volatile LONG m_nWriteAbleCount;
    volatile LONG m_nReadAbleCount;
};


}}}

#endif // MDK_QUEUE_H
