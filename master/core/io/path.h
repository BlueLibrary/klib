#pragma once

#include "../inttype.h"
#include "../istddef.h"

#include <Windows.h>
#include <string>


namespace klib {
namespace io {



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
    

public:
    BOOL get_app_path(tstring& strAppPath); ///< ��ȡӦ�ó����ļ���·��
    BOOL get_app_file(tstring& strAppFile); ///< ��ȡ�ļ���·��
    BOOL get_app_file_name(tstring& strFileName);   ///< ��ȡӦ�ó��������


protected:
    tstring path_;
};

}}
