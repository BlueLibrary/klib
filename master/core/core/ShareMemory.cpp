#include "ShareMemory.h"
#include <Windows.h>
#include <tchar.h>

namespace klib{
namespace mem{


CShareMemory::CShareMemory(void) : m_hSharememoryHandle(NULL)
{
}

CShareMemory::~CShareMemory(void)
{
    Close();
}

void CShareMemory::Create(tstring name, size_t nSize )
{
	m_hSharememoryHandle = CreateFileMapping(INVALID_HANDLE_VALUE,NULL, PAGE_READWRITE, 0, nSize, name.c_str());
	if(m_hSharememoryHandle)
	{
		m_pShareMemoryAddress = MapViewOfFile(m_hSharememoryHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if(m_pShareMemoryAddress == NULL)
		{
			MessageBox(NULL, _T("ӳ���ļ�ӳ�䵽���ý��̵�ַʱ����"), _T("����"), MB_OK);
		}
	}
	else
	{
		MessageBox(NULL, _T("�����ļ�ӳ��ʧ��"), _T("����"), MB_OK);
	}
	m_buffer_size = nSize;
}

void CShareMemory::Close()
{
    CloseHandle(m_hSharememoryHandle);
}


std::string CShareMemory::ReadString()
{
    std::string str = (char*)m_pShareMemoryAddress;
    return std::move(str);
}

void CShareMemory::WriteString(string& content)
{
    size_t nCopySize = content.size() >= m_buffer_size ? m_buffer_size - 1 : content.size();

	memcpy(m_pShareMemoryAddress, (void*)(content.c_str()), nCopySize);
    ((char*)m_pShareMemoryAddress)[nCopySize] = '\0';
}

bool CShareMemory::Read(char* pszBuff, size_t nReadLen, size_t nStartPos, size_t* pReadedLen)
{
    if (NULL == pszBuff) 
    {
        return false;
    }

    // ȷ���ж������ݿ��Ա���ȡ
    size_t nCopySize = 0;
    if (m_buffer_size > (nStartPos + nReadLen)) 
    {
        nCopySize = nReadLen;
    }
    else
    {
        nReadLen = m_buffer_size - nStartPos;
    }

    // ��ȡ����
    memcpy(pszBuff, (const char*)m_pShareMemoryAddress + nStartPos, nCopySize);
    if (pReadedLen) {
        *pReadedLen = nCopySize;
    }

    return true;
}

bool CShareMemory::Write(size_t nWritePos, const char* pSrc, size_t nDataLen, size_t* pWritedLem)
{
    // �����ܹ�д�ĳ���
    size_t nWritedLen;
    if (m_buffer_size > (nWritePos + nDataLen)) 
    {
        nWritedLen = nDataLen;
    }
    else 
    {
        nWritedLen = m_buffer_size - nWritePos;
    }

    // д�빲���ڴ�
    memcpy((char*)m_pShareMemoryAddress + nWritePos, pSrc, nWritedLen);

    if (pWritedLem) 
    {
        * pWritedLem = nWritedLen;
    }

    return true;
}


}}
