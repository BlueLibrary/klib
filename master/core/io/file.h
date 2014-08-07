
#include "../istddef.h"
#include "../inttype.h"

#include <Windows.h>
#include <tchar.h>

namespace klib{
namespace io{


class file
{
public:
    static UINT64 get_file_size(tstring strFilePath);      ///< ��ȡ�ļ���С
    static bool is_file_exists(LPCTSTR pszFilePath);       ///< �ж��ļ��Ƿ����

    static bool write_file(const tstring& src_file, 
        const LPCTSTR lpszContent, 
        size_t nlen);

    static bool write_file_append(const tstring& src_file, 
        const LPCTSTR lpszContent, 
        size_t nlen);

    static bool get_file_content(const tstring& src_file,
        std::string& content);
};



}}