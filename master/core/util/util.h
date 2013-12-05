#pragma once

#include <string>
#include <windows.h>

#include "../istddef.h"
#include "../inttype.h"

namespace klib{
namespace core{


///< ��ȡӦ�ó����ļ���·��
BOOL get_app_path(tstring& strAppPath);

///< ��ȡ�ļ���·��
BOOL get_app_file(tstring& strAppFile);

///< ��ȡӦ�ó��������
BOOL get_app_file_name(tstring& strFileName);

///< ��ȡ�ļ���С
UINT64 get_file_size(tstring strFilePath);

///< �ж��ļ��Ƿ����
BOOL is_file_exists(LPCTSTR pszFilePath);

// У��͵ļ���
// ��16λ����Ϊ��λ����������������ӣ��������������Ϊ������
// ���ټ���һ���ֽڡ����ǵĺʹ���һ��32λ��˫����
USHORT check_sum(USHORT* buff, int iSize);

///< ��ȡcpu����
UINT32 get_cpu_number();

///< �ļ�ʱ��ת��Ϊ����ʱ��
UINT64 FileTimeToUtc(const FILETIME* ftime);

///< ��ȡcpuʹ����
INT32 get_cpu_usage();


///< ����crc32ֵ
size_t crc32(unsigned long crc, const unsigned char* buf, size_t len);


}}

