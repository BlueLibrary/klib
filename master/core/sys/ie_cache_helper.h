#pragma once

#include <windows.h>

namespace klib{
namespace sys{


enum e_del_cache_type //Ҫɾ�������͡�
{
    e_cache_file,       //��ʾinternet��ʱ�ļ�
    e_cache_cookie,     //��ʾCookie
};

class ie_cache_helper
{
public:
    ie_cache_helper(void);
    ~ie_cache_helper(void);


    //����ΪһЩ����������.
    static BOOL delete_url_cache(e_del_cache_type type);

    static BOOL delete_ie_cache(BOOL bDeleteCache = TRUE, BOOL bDeleteCacheIndex = FALSE);

    static HRESULT delete_ie_history(BOOL bDeleteHistory = TRUE, 
        BOOL bDeleteHistoryIndex = FALSE) ;
};


}}