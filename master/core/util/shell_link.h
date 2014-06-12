#ifndef _klib_shell_link_h
#define _klib_shell_link_h


#include "../istddef.h"
#include "../inttype.h"

#include <Windows.h>

namespace klib {
namespace util {


// ��ݷ�ʽ��һЩ����
class shell_link
{
public:
    shell_link(void);
    ~shell_link(void);

    BOOL create_short_cut(const LPCTSTR FilePath,const LPCTSTR LinkPath, LPCTSTR pszDesc = NULL);
    tstring get_link_target(const tstring& src_file);
};


}}



#endif
