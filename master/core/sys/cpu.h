
#include "../istddef.h"
#include "../inttype.h"


namespace klib {
namespace sys {


// cpu��ز����ķ�װ
class cpu
{
public:

    ///< ��ȡcpu����
    UINT32 get_cpu_number();

    ///< ��ȡcpuʹ����
    INT32 get_cpu_usage();

    // У��͵ļ���
    // ��16λ����Ϊ��λ����������������ӣ��������������Ϊ������
    // ���ټ���һ���ֽڡ����ǵĺʹ���һ��32λ��˫����
    USHORT check_sum(USHORT* buff, int iSize);

};




}}