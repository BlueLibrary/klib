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

public:
    ///< ��ȡӦ�ó����ļ���·��
    BOOL get_app_path(tstring& strAppPath);

    ///< ��ȡ�ļ���·��
    BOOL get_app_file(tstring& strAppFile);

    ///< ��ȡӦ�ó��������
    BOOL get_app_file_name(tstring& strFileName);


protected:
    tstring path_;
};

}}
