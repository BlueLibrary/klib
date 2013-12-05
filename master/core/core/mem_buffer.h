#pragma once

#include "..\algo\bm_wrapper.h"
#include <vector>

namespace klib {
namespace mem{

using klib::algo::bm_wrapper;


/**
 * @brief   �ڴ滺����(�ֶ��ڴ�)
 *          ��ҪΪ��Լ�ڴ�
 *          
 * @code
 *          mem_buffer<1024, 20> buff;
 *          buff.Append("contents", 8);
 *      
 * @endcode
 * 
 * @tparam  MAX_BUFFER_SIZE the maximum buffer size.
 * @tparam  MIN_BLOCK_SIZE  the minimum block size.
 * @tparam  bUseStaticMemory �Ƿ�ʹ�þ�̬�ڴ棨���ʹ�ã���Ҫ�ȳ�ʼ��InitStaticMemory��
 */
template<int MAX_BUFFER_SIZE, int MIN_BLOCK_SIZE, BOOL bUseStaticMemory>
class mem_buffer
{
public:
    #define  MAX_BUCKER_SIZE    ((MAX_BUFFER_SIZE / MIN_BLOCK_SIZE) + ((MAX_BUFFER_SIZE % MIN_BLOCK_SIZE) > 0))          ///< ����Ͱ�ĸ���
    
    typedef std::list<char*> MemoryListType;
    typedef size_t           size_type;
    typedef mem_buffer<MAX_BUFFER_SIZE, MIN_BLOCK_SIZE, bUseStaticMemory>   self_type;

public:
    mem_buffer(void) :
        m_ncontent_size(0),
        m_nbuffer_size(0),
        m_ncur_bucket_pos(0),
        m_ncur_bucket_index(0)
    {
        static_assert(MAX_BUFFER_SIZE >= MIN_BLOCK_SIZE, "buffer size must big than block");
    }

    ~mem_buffer(void)
    {
        clear();
    }

    /**
     * @brief   ���ͷ�һ�����ڴ棬�����Ὣ�ö���ɾ��.
     */
    void clear()
    {
        for (auto itr=m_buckets_vec.begin(); itr != m_buckets_vec.end(); ++itr) 
        {
            reuse_memory(*itr);
        }
        m_buckets_vec.clear();

        InterlockedExchangeAdd64(&get_whole_buffer_size(), - (INT64)m_nbuffer_size);

        m_ncontent_size = 0;
        m_nbuffer_size = 0;
        m_ncur_bucket_pos = 0;
        m_ncur_bucket_index = 0;
    }

    /**
     * @brief   �жϻ������Ƿ�Ϊ��
     */
    bool empty() const
    {
        return m_ncontent_size == 0;
    }

    /**
     * @brief   �жϻ������Ƿ����
     */
    bool operator == (self_type& other) const
    {
        if (size() != other.size()) 
        {
            return false;
        }
        if (empty())
        {
            return true;
        }

        size_t nbucket_index = size() / MIN_BLOCK_SIZE;
        size_t nbucket_pos   = size() % MIN_BLOCK_SIZE;

        size_type npos = 0;
        for (; npos < nbucket_index; ++ npos)
        {
            if (memcmp(m_buckets_vec[npos], other.m_buckets_vec[npos], MIN_BLOCK_SIZE) != 0) 
            {
                return false;
            }
        }

        return  (memcmp(m_buckets_vec[npos], other.m_buckets_vec[npos], nbucket_pos) == 0) ;
    }
    
    /**
     * @brief  ������ݵ���������
     * @param  [in]  pszBuff       Ҫ������ݵĻ�����
     * @param  [in]  nLen          Ҫ��ӻ������ĳ���
     * @return true  �ѽ�������ӵ�������
     *         false ���ʧ�ܣ��Ѵﵽ��󻺳������ߣ��ﵽ�趨������ֵ
     */
    bool append(const char* pszBuff, size_t nLen)
    {
        // ���ܳ��������Ļ�������С
        if (m_ncontent_size + nLen > MAX_BUFFER_SIZE) 
        {
            return false;
        }

        if (m_buckets_vec.empty()) 
        {
            char* p = alloc();
            if (NULL == p) 
            {
                return false;
            }

            m_buckets_vec.push_back(p);
            _ASSERT(m_buckets_vec.size() == 1);

            m_nbuffer_size   = MIN_BLOCK_SIZE;
            InterlockedExchangeAdd64(&get_whole_buffer_size(), MIN_BLOCK_SIZE);
        }

        size_t nCopyExistLen = nLen;
        size_t nBucketExists = MIN_BLOCK_SIZE - m_ncur_bucket_pos;
        const char* pCurPos = pszBuff;;

        while (nCopyExistLen >= nBucketExists)
        {
            size_t nCurBucketCopySize = nBucketExists;
            memcpy(&m_buckets_vec[m_ncur_bucket_index][m_ncur_bucket_pos], pCurPos, nCurBucketCopySize);
            pCurPos += nCurBucketCopySize;

            nCopyExistLen -= nBucketExists;
            m_ncontent_size += nCurBucketCopySize;
            m_ncur_bucket_pos += nCurBucketCopySize;
            nBucketExists = 0;

            if (m_buckets_vec.size() <= m_ncur_bucket_index + 1) 
            {
                auto pBuff = alloc();
                if (NULL == pBuff) 
                {
                    return false;
                }
                InterlockedExchangeAdd64(&get_whole_buffer_size(), MIN_BLOCK_SIZE);
                m_buckets_vec.push_back(pBuff);
                m_nbuffer_size += MIN_BLOCK_SIZE;
            }

            ++ m_ncur_bucket_index;
            m_ncur_bucket_pos = 0;
            nBucketExists = MIN_BLOCK_SIZE;
        }

        memcpy(&m_buckets_vec[m_ncur_bucket_index][m_ncur_bucket_pos], pCurPos, nCopyExistLen);
        m_ncur_bucket_pos += nCopyExistLen;
        m_ncontent_size += nCopyExistLen;
        
        return true;
    }

    /**
     * @brief  ��str������ӵ�������
     * @param  [in]  str   ����Ҫ��ӵ���������
     * @return true  ��ӵ���������
     *         false δ��ӳɹ�
     */
    bool append(const std::string& str)
    {
        return append(str.c_str(), str.size());
    }

    /**
     * @brief ����������������ӵ���ǰ��������
     */
    bool append(const mem_buffer& s)
    {
        if (s.empty()) 
        {
            return true;
        }

        size_t nbucket_index = s.size() / MIN_BLOCK_SIZE;
        size_t nbucket_pos   = s.size() % MIN_BLOCK_SIZE;

        size_t npos = 0;
        for (; npos < nbucket_index; ++ npos) 
        {
            this->append(s.m_buckets_vec[npos], MIN_BLOCK_SIZE);
        }
        return this->append(s.m_buckets_vec[npos], nbucket_pos);
    }


    mem_buffer& operator << (const char* psrc)
    {
        append(psrc);
        return *this;
    }

    /**
     * @brief  ���������е�������ȡ��pbuff��
     * @param  [in,out]  pbuff    Ҫ����ȡ�Ļ�����
     * @param  [in]      nPos     �ڻ������е���ʼλ��
     * @param  [in]      nLen     Ҫ��ȡ���ݵĳ���
     * @return true  ������ȡ�ɹ�
     *         false ������ȡʧ�ܣ��������е����ݳ��Ȳ���
     */
    bool copy(__inout char* pbuff, size_t nPos, size_t nLen) const
    {
        _ASSERT(pbuff);
        // ������֤
        if (NULL == pbuff || 0 == nLen) 
        {
            return false;
        }

        // �ж��Ƿ����㹻�Ŀռ�
        if ((nPos + nLen) > m_ncontent_size) 
        {
            return false;
        }

        char* pFetechPos = pbuff;
        size_t nbucket_index = nPos / MIN_BLOCK_SIZE;
        size_t nbucket_pos = nPos % MIN_BLOCK_SIZE;
        size_t nFetechExist = nLen;
        size_t nBucketExistLen = MIN_BLOCK_SIZE - nbucket_pos; 
        while (nFetechExist > nBucketExistLen)
        {
            memcpy(pFetechPos, &m_buckets_vec[nbucket_index][nbucket_pos], nBucketExistLen);

            pFetechPos += nBucketExistLen;
            nFetechExist -= nBucketExistLen;

            ++ nbucket_index;
            nbucket_pos = 0;
            nBucketExistLen = MIN_BLOCK_SIZE - nbucket_pos;
        }
        memcpy(pFetechPos, &m_buckets_vec[nbucket_index][nbucket_pos], nFetechExist);

        return true;
    }

    /**
     * @brief  �����ַ���
     * @param  [in]  pszStr  Ҫ�����ҵ��ַ���
     * @param  [in]  nLen    Ҫ�����ַ����ĳ���
     * @param  [in]  nStartPos ���ҵ���ʼλ��
     * @param  [in]  bCaseSence ��Сд����
     * @return  -1   ����ʧ��
     *          >=0  ���ص����ڻ���������ʼλ��
     */
    int find(const char* pszStr, size_t nLen, size_t nStartPos, bool bCaseSense) const
    {
        if (nStartPos + nLen > m_ncontent_size) 
        {
            return -1;
        }
        if (nLen > m_ncontent_size) 
        {
            return -1;
        }
        if (nStartPos > m_ncontent_size) 
        {
            return -1;
        }

        size_t nbucket_index = nStartPos / MIN_BLOCK_SIZE;
        size_t nbucket_pos   = nStartPos % MIN_BLOCK_SIZE;
        if (0 == nLen) 
        {
            return -1;
        }

        bm_wrapper bm;
        bm.init_pattern((unsigned char*)pszStr, (int)nLen, bCaseSense);
        int nPosFind = -1;
        auto cmpFun = strncmp;
        if (!bCaseSense) 
        {
            cmpFun = _strnicmp;
        }

        while (nbucket_index < m_ncur_bucket_index)
        {
            // ���ڲ���
            nPosFind = bm.search((const unsigned char*) &m_buckets_vec[nbucket_index][nbucket_pos], (int)(MIN_BLOCK_SIZE - nbucket_pos));
            if (nPosFind != -1) 
            {
                return (int)(nbucket_index * MIN_BLOCK_SIZE) + nPosFind + (int)nbucket_pos;
            }
            else 
            {
                // ��2���β���
                if (nLen <= MIN_BLOCK_SIZE) 
                {
                    size_t nSearchLen = nLen - 1;  ///< ��һ������Ҫ�����ĳ���

                    for (;nSearchLen > 0; -- nSearchLen) 
                    {
                        // ��һ����ʣ��Ŀռ�Ҫ���ڴ������ĳ���
                        if (MIN_BLOCK_SIZE - nbucket_pos >= nSearchLen) 
                        {
                            size_t nTmpPos = MIN_BLOCK_SIZE - nSearchLen;   ///< ��ʱ�Ҳ��λ��
                            
                            // ��һ���αȽ�nSearchLen���ȵ��ַ���,�ڶ����β���nLen - nSearchLen���ַ���
                            if (cmpFun(&m_buckets_vec[nbucket_index][nTmpPos], pszStr, nSearchLen) == 0 && 
                                cmpFun(m_buckets_vec[nbucket_index+1], pszStr + nSearchLen, nLen - nSearchLen) == 0) 
                            {
                                return (int)(nbucket_index * MIN_BLOCK_SIZE) + (int)nTmpPos;;
                            }
                        }
                    }
                }
                else
                {
                    // ��Խ3�������϶εĲ���
                    size_t nFirstSearchLen = (MIN_BLOCK_SIZE - nbucket_pos);
                    size_t nLeftSearchLen = nLen;       ///< ʣ�������ĳ���
                    size_t nSearchIndex = 0;
                    size_t nExistContentLen = 0;        ///< ʣ�µ����ݳ���
                    size_t nSkipedBlockSize = (nbucket_index * MIN_BLOCK_SIZE); ///< �����Ŀ�Ĵ�С���Ż�������
                    auto pCurPos = pszStr;

                    nExistContentLen = m_ncontent_size - (nSkipedBlockSize + (MIN_BLOCK_SIZE - nFirstSearchLen));
                    
                    // ��һ�������ĳ���Ҫ����0������ʣ��Ŀռ��㹻��������,
                    while (nFirstSearchLen > 0 
                           && nbucket_index < m_ncur_bucket_index ///< ��������������ҪС�ڵ�ǰ������������
                           && nExistContentLen >= nLen
                           )
                    {
                        size_t ntmp = MIN_BLOCK_SIZE - nFirstSearchLen;
                        nLeftSearchLen = nLen - nFirstSearchLen;

                        // ��һ�������ʣ�೤��Ҫ���ڵ�һ�������ĳ���
                        if (MIN_BLOCK_SIZE - nbucket_pos >= nFirstSearchLen) 
                        {
                            pCurPos = pszStr;
                            if (cmpFun(&m_buckets_vec[nbucket_index][ntmp], pCurPos, nFirstSearchLen) == 0) 
                            {
                                // ��һ������ƥ��ɹ�����Ҫƥ�����������
                                pCurPos += nFirstSearchLen;         ///< ���Ƚϵ��ַ���˳��
                                nSearchIndex = nbucket_index + 1;   ///< ������һ����

                                // С�ڵ�ǰ��������ʱ������Ƚϣ�ʣ�µĳ��ȴ�������,����������С�ڵ�ǰ�������,����Խ�磩
                                while (nLeftSearchLen > MIN_BLOCK_SIZE && nSearchIndex < m_ncur_bucket_index)
                                {
                                    if (cmpFun(&m_buckets_vec[nSearchIndex][0], pCurPos, MIN_BLOCK_SIZE) != 0) 
                                    {
                                        break;
                                    }

                                    nLeftSearchLen -= MIN_BLOCK_SIZE;
                                    pCurPos += MIN_BLOCK_SIZE;
                                    ++ nSearchIndex;
                                }

                                _ASSERT(nLeftSearchLen > 0);
                                // ��� nLeftSearchLen > MIN_BLOCK_SIZE��ô�϶�û���ҵ�
                                if (nLeftSearchLen <= MIN_BLOCK_SIZE) 
                                {
                                    // �ҵ����ҵ��ַ���
                                    if (cmpFun(&m_buckets_vec[nSearchIndex][0], pCurPos, nLeftSearchLen) == 0) 
                                    {
                                        return (int)(nbucket_index * MIN_BLOCK_SIZE) + (int)ntmp;
                                    }
                                }
                            }
                        }

                        -- nFirstSearchLen;
                        nExistContentLen = m_ncontent_size - (nSkipedBlockSize + (MIN_BLOCK_SIZE - nFirstSearchLen));
                    }
                }
            }

            nbucket_pos = 0;
            ++ nbucket_index;
        }

        _ASSERT(nbucket_index == m_ncur_bucket_index);
        nPosFind = bm.search((const unsigned char*) &m_buckets_vec[nbucket_index][nbucket_pos], (int)(m_ncur_bucket_pos - nbucket_pos));
        if (nPosFind == -1) 
        {
            return -1;
        }
        return (int)(m_ncur_bucket_index * MIN_BLOCK_SIZE) + (int)nbucket_pos + nPosFind;
    }

    /**
     * @brief  �Ƿ�����ָ���ַ�����ʼ
     * @param  [in]  pszStr   Ҫ�жϵ��ַ���
     * @param  [in]  nLen     Ҫ�жϵ��ַ�������
     * @param  [in]  bCaseSence �Ƿ��Сд����
     * @return  true ����ָ�����ַ�����ʼ
     *          false ����
     */
    bool is_begin_with(const char* pszStr, size_t nLen, bool bCaseSence = true) const 
    {
        _ASSERT(pszStr);
        if (NULL == pszStr) 
        {
            return false;
        }

        auto cmpFun = strncmp;
        if (!bCaseSence) 
        {
            cmpFun = _strnicmp;
        }

        if (cmpFun(m_buckets_vec[0], pszStr, nLen) == 0) 
        {
            return true;
        }
        return false;
    }

    /**
     * @brief 
     */
    bool is_begin_with(const char* pszStr, bool bCaseSence = true) const 
    {
        return is_begin_with(pszStr, strlen(pszStr));
    }

    /**
     * @brief  �Ƿ�����ָ�����ַ�������
     * @param  pszStr   Ҫ�жϵ��ַ���
     * @param  nLen     Ҫ�жϵ��ַ�������
     * @return true     ���Ը��ַ��н���
     *         false    ����
     */
    bool is_end_with(const char* pszStr, size_t nLen) const 
    {
        _ASSERT(pszStr);
        if (NULL == pszStr || size() < nLen) 
        {
            return false;
        }

        size_t nMatched = 0;
        size_t nPos = size() - 1;
        auto pszCur = pszStr + nLen - 1;
        while (pszCur >= pszStr)
        {
            if (this->operator[](nPos) == (unsigned char) *pszCur) 
            {
                -- nPos;
                -- pszCur;
                ++ nMatched;
            }
            else
            {
                break;
            }
        }

        if (nMatched == nLen) 
        {
            return true;
        }
        return false;
    }

    /**
     * @brief  �ж��Ƿ�����pstStr����
     */
    bool is_end_with(const char* pszStr) const 
    {
        return is_end_with(pszStr, strlen(pszStr));
    }

    /**
     * @brief  ɾ�������nLen�ַ�
     * @param  [in]  nLen  Ҫɾ���ĳ���
     * @return true ɾ���ɹ�
     *         false ɾ��ʧ��
     */     
    bool erase_back(size_t nLen)
    {
        // m_ncontent_size, m_ncur_bucket_pos,m_ncur_bucket_index
        _ASSERT(nLen > 0);
        if (nLen > m_ncontent_size) 
        {
            return false;
        }

        m_ncontent_size -= nLen;
        m_ncur_bucket_index = m_ncontent_size / MIN_BLOCK_SIZE;
        m_ncur_bucket_pos   = m_ncontent_size % MIN_BLOCK_SIZE;
        return true;
    }
    
    /**
     *  @brief ��ȡ���л������Ĵ�С
     *  @note ��������bufferʹ�õĿռ��ܺ�
     *        (ʹ�þ�̬�ֲ�������Ҫ��Ϊ�˷���ʹ��,�����������ʼ��̬��Ա)
     */
    static volatile LONGLONG&  get_whole_buffer_size()
    {
        static volatile LONGLONG s_nwhole_buffer_size = 0;
        return s_nwhole_buffer_size;
    }

    /**
     * @brief  ��ȡ��ǰ�����������ݳ��� 
     */
    size_t  size() const
    {
        return m_ncontent_size;
    }

    /**
     * @brief  ��ȡ��ǰ����Ĵ�С
     * @note   �Ǿ�̬��Ա������ֻ���ǵ�ǰʹ�õĻ���������
     */
    size_t  capacity() const
    {
        return m_nbuffer_size;
    }

    /**
     * @brief   ��ȡָ��ƫ��λ�õ��ַ�
     * @param   [in]  nPos  ָ��λ�õ�ƫ��
     * @return  -1  ƫ�Ƴ�������,���ж��Ƿ�Խ�磨δԽ��������Ҳ���ܷ���-1��
     *          >=0 ָ��ƫ�Ƶ�����
     */
    __forceinline unsigned char operator[](size_t nPos) const
    {
        if (nPos > m_ncontent_size) 
        {
            return 0x0ff;
        }

        size_t nbucket_index = nPos / MIN_BLOCK_SIZE;
        size_t nbucket_pos   = nPos % MIN_BLOCK_SIZE;

        return *(unsigned char*) &m_buckets_vec[nbucket_index][nbucket_pos];
    }

    /**
     * @brief   ��̬���ݲ���
     */
    __forceinline static void init_static_memory(UINT64 uMaxBufferSize)
    {
        if (bUseStaticMemory) 
        {
            InitializeCriticalSection(&get_memory_cs());

            MemoryListType& memoryList = get_memory_list();

            UINT64 uBlockCount = (uMaxBufferSize / MIN_BLOCK_SIZE) ;
            uBlockCount += (uMaxBufferSize % MIN_BLOCK_SIZE) > 0;
            char* pBuff = new char[uBlockCount * MIN_BLOCK_SIZE];
            if (NULL != pBuff) 
            {
                _ASSERT(NULL == get_alloc_addr());
                get_alloc_addr() = pBuff;
                char* pCurBuff = pBuff;
                for (int i=0; i<uBlockCount; ++i) 
                {
                    memoryList.push_back(pCurBuff);
                    pCurBuff += MIN_BLOCK_SIZE;
                }
            }
        }
    }

    /**
     * @brief ����ʼ���ڴ�
     */
    __forceinline static void uninit_static_memory()
    {
        if (bUseStaticMemory) 
        {
            free_static_memory();

            DeleteCriticalSection(&get_memory_cs());
        }
    }

    /**
     * @brief  ����δʹ�õ��ڴ�Ĵ�С
     * @note   ָ�ѷ�����δʹ�õĴ�С
     * @return δʹ�õ��ڴ��С
     */
    __forceinline static UINT64 get_static_memory_size()
    {
        UINT64 uSize = 0;
        if (bUseStaticMemory) 
        {
            uSize = (MIN_BLOCK_SIZE * get_memory_list().size());
        }
        else 
        {
            _ASSERT(FALSE);
        }

        return uSize;
    }

    /**
     * @brief   ����̬�ڴ��ͷŵ� 
     */ 
    __forceinline static void free_static_memory()
    {
        if (bUseStaticMemory) 
        {
            EnterCriticalSection(&get_memory_cs());

            _ASSERT(get_alloc_addr() != NULL);

            if (NULL != get_alloc_addr()) 
            {
                delete get_alloc_addr();
            }
            get_memory_list().clear();

            LeaveCriticalSection(&get_memory_cs());
        }
    }

protected:
    /**
     * @brief  ��ȡ�����ڴ��ָ��
     * @return �����ڴ��ָ�룬�����ڴ��ᱻ��д
     */
    __forceinline static char* & get_alloc_addr()
    {
        static char* s_pAllocAddr = NULL;
        return s_pAllocAddr;
    }

    /**
     * @brief  ���ڴ���л�ȡָ����С���ڴ��ʹ��
     */
    __forceinline static char* alloc()
    {
        char* p = NULL;
        if (bUseStaticMemory) 
        {
            EnterCriticalSection(&get_memory_cs());

            // �ڴ���е��ڴ�ʹ�����Ͳ����ٽ���������
            MemoryListType& memoryList = get_memory_list();
            if (!memoryList.empty()) 
            {
                p = memoryList.front();
                memoryList.pop_front();
            }

            LeaveCriticalSection(&get_memory_cs());
        }
        else 
        {
            p = new char[MIN_BLOCK_SIZE];
        }

        return p;
    }

    /**
     * @brief  �����ڴ���ͷŵ��ڴ���� 
     */
    __forceinline static void reuse_memory(char* p)
    {
        if (bUseStaticMemory) 
        {
            _ASSERT(p);
            EnterCriticalSection(&get_memory_cs());

            if (p) 
            {
                get_memory_list().push_back(p);
            }
            
            LeaveCriticalSection(&get_memory_cs());
        }
        else
        {
            delete p;
        }
    }

    /**
     * @brief  ��ȡ�ڴ�ر�
     */
    __forceinline static MemoryListType& get_memory_list()
    {
        static MemoryListType _memory;
        return _memory;
    }

    /**
     * ��ȡ�ڴ�صĻ������ 
     */
    __forceinline static CRITICAL_SECTION& get_memory_cs()
    {
        static CRITICAL_SECTION _cs;
        return _cs;
    }
    
protected:
    size_t m_ncontent_size;                     ///< ���ݵ��ܳ���
    size_t m_nbuffer_size;                      ///< �ڴ滺�������ܳ���

    size_t m_ncur_bucket_pos;                   ///< ��Ͱ�е�λ�� 
    size_t m_ncur_bucket_index;                 ///< Ͱ������

    typedef std::vector<char*>  bucket_vector_type;
    bucket_vector_type m_buckets_vec;           ///< ��Ż�����ָ�������
};


//#include <fstream>
using namespace std;

/**
 * @brief   �ڴ��ļ��ķ�װ.
 *
 * @tparam  MAX_BUFFER_SIZE  the maximum buffer size.
 * @tparam  MIN_BLOCK_SIZE   the minimum block size.
 * @tparam  bUseStaticMemory �Ƿ�ʹ�þ�̬�ڴ棨���ʹ�ã���Ҫ�ȳ�ʼ����
 */
template<int MAX_BUFFER_SIZE, int MIN_BLOCK_SIZE, BOOL bUseStaticMemory>
class mem_file_wrapper : public mem_buffer<MAX_BUFFER_SIZE, MIN_BLOCK_SIZE, bUseStaticMemory>
{
public:
    mem_file_wrapper() :
      m_nOpPos(0)
    {}

    typedef  size_t  streamsize;
    typedef  mem_file_wrapper<MAX_BUFFER_SIZE, MIN_BLOCK_SIZE, bUseStaticMemory> self_type;
    enum seek_dir {way_begin, way_cur, way_end};

public:
    mem_file_wrapper& read (char* s, streamsize n)
    {
        if (Fetech(s, m_nOpPos, n))
        {
            m_nOpPos += n;
        }

        return *this;
    }

    mem_file_wrapper& write (const char* s, streamsize n)
    {
        if (append(s, n))
        {
            m_nOpPos += n;
        }

        return *this;
    }
        
    size_t tellg()
    {
        return m_nOpPos;
    }

    mem_file_wrapper& seekg (size_t pos)
    {
        m_nOpPos = pos;
        return *this;
    }

    mem_file_wrapper& seekg (int off, seek_dir way)
    {
        if (way == way_begin) 
        {
            _ASSERT(off >= 0 && off < (int) size());
            if (off >= 0 && off < (int) size()) 
            {
                m_nOpPos = 0 + off;
            }
        }
        else if (way == way_cur) 
        {
            auto nNewPos = m_nOpPos + off;
            _ASSERT(nNewPos >=0 && nNewPos < size());
            if (nNewPos >=0 && nNewPos < size()) 
            {
                m_nOpPos += off;
            }
        }
        else if (way == way_end) 
        {
            _ASSERT(off <= 0);
            if (off <= 0) 
            {
                m_nOpPos = size() + off;
            }
        }
        else 
        {
            _ASSERT(0);
        }

        return *this;
    }

    mem_file_wrapper& getline (char* s, streamsize n)
    {
        _ASSERT(s);

        size_t nReadedCount = 0;
        auto pCurPos = s;
        unsigned char uCh = 0;

        while (m_nOpPos < size() -1 && (uCh = this->operator[](m_nOpPos)) && nReadedCount < (size_t) n - 1)
        {
            // �ж�\r\n�����
            if (uCh == '\r' && this->operator[](m_nOpPos + 1) == '\n') 
            {
                m_nOpPos += 2;      // ��Ҫ�����س����еĵط�
                break;
            }
            else if (uCh == '\n')  // �ж�ֻ�лس������
            {
                ++ m_nOpPos;
                break;
            }
            else
            {
                *pCurPos = uCh;
            }

            ++ pCurPos;
            ++ nReadedCount;
            ++ m_nOpPos;
        }
        *pCurPos = '\0';

        return *this;
    }

    mem_file_wrapper& getline (char* s, streamsize n, char delim)
    {
        _ASSERT(s);

        size_t nReadedCount = 0;
        auto pCurPos = s;
        unsigned char uCh = 0;

        while (m_nOpPos < size() -1 && (uCh = this->operator[](m_nOpPos)) && nReadedCount < n - 1)
        {
            if (uCh != delim) 
            {
                *pCurPos = uCh;
            }
            else 
            {
                ++ m_nOpPos;
                break;
            }

            ++ pCurPos;
            ++ nReadedCount;
            ++ m_nOpPos;
        }
        *pCurPos = '\0';

        return *this;
    }

    mem_file_wrapper& getline (char* s, streamsize n, char* szdelim)
    {
        _ASSERT(s && szdelim);

        size_t nReadedCount = 0;
        auto pCurPos = s;
        unsigned char uCh = 0;

        auto pCompareDelim = szdelim;
        size_t nComparePos = 0;
        size_t nDelimLen = strlen(szdelim);

        while (m_nOpPos < size() -1 && (uCh = this->operator[](m_nOpPos)) && nReadedCount < n - 1)
        {
            pCompareDelim = szdelim;
            nComparePos = m_nOpPos;

            while (*pCompareDelim && this->operator[](nComparePos) == *pCompareDelim) 
            {
                ++ nComparePos;
                ++ pCompareDelim;
            }

            if (*pCompareDelim) 
            {
                *pCurPos = uCh;
            }
            else 
            {
                m_nOpPos += nDelimLen;
                break;
            }

            ++ pCurPos;
            ++ nReadedCount;
            ++ m_nOpPos;
        }
        *pCurPos = '\0';

        return *this;
    }

    mem_file_wrapper& getline(std::string& str)
    {
        str.clear();
        unsigned char uCh = 0;
        while (m_nOpPos < size() && (uCh = this->operator[](m_nOpPos)) )
        {
            if (uCh == '\r' && this->operator[](m_nOpPos + 1) == '\n') 
            {
                m_nOpPos += 2;
                break;
            }
            else if (uCh == '\n') 
            {
                ++ m_nOpPos;
                break;
            }
            else 
            {
                str.push_back(uCh);
            }

            ++ m_nOpPos;
        }

        return *this;
    }

    int get()
    {
        return this->operator[](m_nOpPos ++);
    }

    mem_file_wrapper& get (char& c)
    {
        c = get();
        return *this;
    }

    mem_file_wrapper& get (char* s, streamsize n)
    {
        _ASSERT(s);

        size_t nReadedCount = 0;
        auto pCurPos = s;

        while (m_nOpPos < size() && nReadedCount < n - 1)
        {
            *pCurPos = this->operator[](m_nOpPos);

            ++ pCurPos;
            ++ nReadedCount;
            ++ m_nOpPos;
        }
        *pCurPos = '\0';

        return *this;
    }

    mem_file_wrapper& get (char* s, streamsize n, char delim)
    {
        _ASSERT(s);

        size_t nReadedCount = 0;
        auto pCurPos = s;
        unsigned char uCh = 0;
        while (m_nOpPos < size() && nReadedCount < n - 1)
        {
            uCh = this->operator[](m_nOpPos);
            if (uCh != delim) 
            {
                *pCurPos = uCh;
            }
            else 
            {
                ++ m_nOpPos;
                break;
            }

            ++ pCurPos;
            ++ nReadedCount;
            ++ m_nOpPos;
        }
        *pCurPos = '\0';

        return *this;
    }

    bool eof()
    {
        _ASSERT(m_nOpPos <= size());
        return m_nOpPos >= size();
    }

    operator bool()
    {
        return ! eof();
    }


protected:
    size_t   m_nOpPos;              ///< �����е�λ��
};


}}
