
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
    static BOOL is_file_exists(LPCTSTR pszFilePath);       ///< �ж��ļ��Ƿ����



};



}}