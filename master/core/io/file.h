
#include "../istddef.h"
#include "../inttype.h"

#include <Windows.h>
#include <tchar.h>

namespace klib{
namespace io{


class file
{
public:
    ///< ��ȡ�ļ���С
    UINT64 get_file_size(tstring strFilePath);

    ///< �ж��ļ��Ƿ����
    BOOL is_file_exists(LPCTSTR pszFilePath);





};



}}