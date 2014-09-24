#ifndef _klib_system_info_h_
#define _klib_system_info_h_


#include <string>


namespace klib{
namespace sys {

class system_info
{
public:
    // ��ȡ����ϵͳλ��
    static int get_system_bits() ;

    //��ȡ����ϵͳ������  
    static void get_system_name(std::string& osname);

    //��ȡ����ϵͳ�İ汾����  
    static void get_version_mark(std::string& vmark);

protected:

};


}}




#endif // _klib_system_info_h_