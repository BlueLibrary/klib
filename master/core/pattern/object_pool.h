#ifndef _klib_object_pool_h
#define _klib_object_pool_h

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,3)
#pragma push_macro("new")
    #undef new


#include <vector>
#include "..\kthread\auto_lock.h"

namespace klib {
namespace pattern {

using namespace klib::kthread;


template<class T>
class CMemBlock
{
public:
    CMemBlock() : bFixed(FALSE), uIndex(0), pNext(NULL) {}

    T               obj;        // ���صĶ���

    BOOL            bFixed;     // �̶��Ļ���ʾ����Ҫ��delete[]����
    size_t          uIndex;     // ��vector�е�����
    CMemBlock*      pNext;      // ָ����һ���ڴ��
};

//Ĭ�ϱ�����Ͱ�Ĵ�С
#define  DEFAULT_MEM_BLOCK_RESERVE      (50)

/**
 * @brief  �����
 * @tparam  INIT_SIZE ��ʼ��С
 * @tparam  INC_SIZE  ������С
 */
template<class T, INT INIT_SIZE, INT INC_SIZE>
class CObjectPool
{
public:
    //ȷ���ڴ�صĳ�ʼ����С�Լ������Ĵ�С
    CObjectPool() 
    {
        m_vecMemBlock.reserve(DEFAULT_MEM_BLOCK_RESERVE);

        AllocBlocks();
    };

    ~CObjectPool()
    {
        FreeAll();
    }

    T*      Alloc();
    void    Free(T* pobj);                       //�������Ķ���ָ�����ͷ�
    void    FreeAll();                           //�ͷ����е��ڴ�

    void    ReleaseSome();                       //�鿴���ͷ��ܹ��ͷŵ��ڴ�,�ⲿ����

private:
    void    AllocBlocks(int iCount = 0);     //�����ڴ��
    void    ReAllocBlocks();                 //�ڴ�鲻��ʱ���´���
    
    bool    Empty();                         //�ж��ڴ���Ƿ�Ϊ��

private:
    struct stMemBlockInfo 
    {
        stMemBlockInfo() : uWholeBlock(0), uFreeBlock(0), pHead(NULL), pFixedBlock(NULL) {}

        UINT    uWholeBlock;
        UINT    uFreeBlock;
        CMemBlock<T> * pHead;               // �ڴ������Ŀ�ʼλ�ã���һ���ǿ��ͷ��
        CMemBlock<T> * pFixedBlock;         // ���ڿ����ͷ��ڴ��
    };

    auto_cs         m_obj_auto_cs;
    
    typedef std::vector<stMemBlockInfo> MemBlockVecType;
    MemBlockVecType         m_vecMemBlock;
};

template<class T, INT INIT_SIZE, INT INC_SIZE>
void CObjectPool<T, INIT_SIZE, INC_SIZE>::AllocBlocks(int iCount/* = initSize*/)
{
    if (iCount == 0) {
        iCount = INIT_SIZE;
    }
    
    CMemBlock<T>* pBuff = new CMemBlock<T>[iCount];
    if (NULL == pBuff) {
        return;
    }

    MemBlockVecType::size_type uIndex = 0;
    for (; uIndex < m_vecMemBlock.size(); ++ uIndex) {
        if (0 == m_vecMemBlock[uIndex].uWholeBlock) {
            break;
        }
    }
    if (uIndex == m_vecMemBlock.size()) {
        m_vecMemBlock.resize(uIndex + 1);
        _ASSERT(m_vecMemBlock.size() == uIndex + 1);
    }

    CMemBlock<T>* pHead = NULL;
    CMemBlock<T>* pPos = pBuff;
    for (int i=0; i < iCount - 1; ++ i) {
        pPos->uIndex = uIndex;

        pPos->pNext = &pBuff[i+1];
        pPos = pPos->pNext;
    }
    pPos->uIndex = uIndex;
    pPos->pNext = pHead;

    pHead = pBuff;
    pHead->bFixed = TRUE;

    stMemBlockInfo info;
    info.uFreeBlock = info.uWholeBlock = iCount;
    info.pHead = pHead;
    info.pFixedBlock = pHead;
    m_vecMemBlock[uIndex] = info;
}

template<class T, INT INIT_SIZE, INT INC_SIZE>
void CObjectPool<T, INIT_SIZE, INC_SIZE>::ReAllocBlocks()
{
    AllocBlocks(INC_SIZE);
}

template<class T, INT INIT_SIZE, INT INC_SIZE>
bool CObjectPool<T, INIT_SIZE, INC_SIZE>::Empty()
{
    MemBlockVecType::const_iterator itr;
    itr = m_vecMemBlock.begin();
    for (; itr != m_vecMemBlock.end(); ++ itr) {
        if (itr->pHead != NULL) {
            return false;
        }
    }
    
    return true;
}

template<class T, INT INIT_SIZE, INT INC_SIZE>
T*  CObjectPool<T, INIT_SIZE, INC_SIZE>::Alloc()
{
    auto_lock lock(m_obj_auto_cs);

    if (Empty()) {
        ReAllocBlocks();
    }

    CMemBlock<T>* pTmp = NULL;
    MemBlockVecType::iterator itr = m_vecMemBlock.begin();
    for (; itr != m_vecMemBlock.end(); ++ itr) {
        if (NULL != itr->pHead) {
            pTmp = itr->pHead;
            itr->pHead = itr->pHead->pNext;

            -- itr->uFreeBlock;
            break;
        }
    }
    
    return (T*) pTmp;
}

template<class T, INT INIT_SIZE, INT INC_SIZE>
void CObjectPool<T, INIT_SIZE, INC_SIZE>::Free(T* pobj)
{
    _ASSERT(pobj);

    // ���¹���һ���ṹ

    CMemBlock<T>* pos = (CMemBlock<T>*) ( pobj);
    pos->~CMemBlock<T>();
    new (pobj) CMemBlock<T>;

    auto_lock lock(m_obj_auto_cs);

    stMemBlockInfo& info = m_vecMemBlock[pos->uIndex];
    CMemBlock<T>* & pHead = info.pHead;

    pos->pNext = pHead;
    pHead = pos;

    ++ info.uFreeBlock;
}

template<class T, INT INIT_SIZE, INT INC_SIZE>
void CObjectPool<T, INIT_SIZE, INC_SIZE>::FreeAll()
{
    MemBlockVecType::const_iterator itr;
    itr = m_vecMemBlock.begin();

    for (; itr != m_vecMemBlock.end(); ++ itr) 
    {
        if (0 == itr->uWholeBlock) 
        {
            continue;
        }

        CMemBlock<T> *pFixedBlock = itr->pFixedBlock;
        delete[] pFixedBlock;
    }
}

template<class T, INT INIT_SIZE, INT INC_SIZE>
void CObjectPool<T, INIT_SIZE, INC_SIZE>::ReleaseSome()
{
    auto_lock lock(m_obj_auto_cs);

    MemBlockVecType::size_type uIndex = 1;
    while (uIndex < m_vecMemBlock.size())
    {
        stMemBlockInfo& info = m_vecMemBlock[uIndex];

        if (info.uWholeBlock == info.uFreeBlock) 
        {
            //�����ͷ�

            delete[] info.pFixedBlock;
            
            new (&info) stMemBlockInfo;
        }

        ++ uIndex;
    }
}



}}



#pragma pop_macro("new")
#pragma warning(pop)
#pragma pack(pop)


#endif
