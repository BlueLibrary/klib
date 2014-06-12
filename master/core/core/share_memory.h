#ifndef _klib_share_memory_h
#define _klib_share_memory_h
#include <string>
#include <wtypes.h>
#include <winbase.h>
using namespace std;

#include "../istddef.h"

namespace klib{
namespace mem{


using namespace klib;


/*
 �ڴ湲����
*/
class share_memory
{
public:
	share_memory(void);
	~share_memory(void);

public:
    /**
     * @brief ���������ڴ�
     * @param  name  �����ڴ������
     * @param  nSize �����ڴ�Ĵ�С
     */
	void create(tstring name, size_t nSize);

    /**
     * �رչ����ڴ�
     */
    void close();

    /**
     * @brief ��ȡ�����ڴ�ĳ���
     */
    size_t size() { return m_buffer_size; }

    /**
     * @brief ��ȡ�����ڴ��ַ
     */
    void*  get_addr() { return m_pShareMemoryAddress; }

    /**
     * @brief ��ȡ�ڴ棬��string����
     */
	string read_string();

    /**
     * @brief ��stringд�빲���ڴ�
     */
	void write_string(string& content);

    /**
     * @brief �ӻ������ж�ȡһ�����ȵ�����
     * @param  pszBuff    ��ȡ�����Ĵ�ŵĻ�����
     * @param  nReadLen   Ҫ���ĳ���
     * @param  nStartPos  ��ʼ��ȡ��λ��
     * @param  pReadedLen ����ʵ�ʶ�ȡ�ĳ���
     */
    bool read(char* pszBuff, size_t nReadLen, size_t nStartPos, size_t* pReadedLen);

    /**
     * @brief ��������������д�빲���ڴ�
     * @param  nWritePos   Ҫд�빲���ڴ����ݵ���ʼλ��
     * @param  pSrc        Ҫд�빲���ڴ�����ݵ�ַ
     * @param  nDataLen    Ҫд�빲���ڴ�ĳ���
     * @param  pWritedLem  ʵ�ʱ�д��ĳ���
     */
    bool write(size_t nWritePos, const char* pSrc, size_t nDataLen, size_t* pWritedLem);

private:
	HANDLE m_hSharememoryHandle;            ///< �����ڴ���
	void  *m_pShareMemoryAddress;           ///< �����ڴ��ȡ�ĵ�ַ

	size_t m_buffer_size;                   ///< �����ڴ��С
};



}}

#endif
