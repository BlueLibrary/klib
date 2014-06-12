#ifndef _klib_path_h
#define _klib_path_h

#include "../inttype.h"
#include "../istddef.h"

#include <string>
#include <tchar.h>
#include <Windows.h>


namespace klib {
namespace io {


/// ·����ز���
class path
{
public:
    static bool is_file(const tstring& file)  ;
    static bool is_directory(const tstring& dir) ;
    static bool is_exists(const tstring& file) ;

    static bool has_slash(const tstring& lpszPath);
    static tstring add_slash(tstring& lpszPath );
    static tstring del_slash(tstring& lpszPath );

    static tstring extract_file_name(const tstring& szFile);
    static tstring extract_file_ext(const tstring& szFile);
    static tstring extract_path(const tstring& szFile);
    static tstring del_file_ext(const tstring& szFile);

    static tstring get_system_path();
    static tstring get_specify_path(int folderid);
    static tstring get_temp_path(const LPCTSTR  lpszPrefixString = _T("klib_"));    ///< ��ȡ��ʱ�ļ�������

    static BOOL get_app_path(tstring& strAppPath);      ///< ��ȡӦ�ó����ļ���·��
    static BOOL get_app_file(tstring& strAppFile);      ///< ��ȡ�ļ���·��
    static BOOL get_app_file_name(tstring& strFileName);   ///< ��ȡӦ�ó��������

public:
    static BOOL create_directorys(const tstring& szPath);   ///< �ݹ鴴��Ŀ¼

protected:
    tstring path_;
};

}}


#endif
