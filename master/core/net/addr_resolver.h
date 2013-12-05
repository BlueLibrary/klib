
#if !defined(_Klib_IpAddressResolver_H)
#define _Klib_IpAddressResolver_H


#include <WinSock2.h>
#include <windows.h>


namespace klib {
namespace net {

///< �������ɽ�����ip����
#define  MAX_ADDRARR_NUM	5

///< ip��ַ������
class addr_resolver  
{
public:
	addr_resolver();
	addr_resolver(const char* ip);
	virtual ~addr_resolver();

    ///< ����ip��ַ
	BOOL resolve(const char* ip);

    ///< ��ȡIP��ַ�ĸ���
    inline size_t size() { return nsize_; }

    ///< ��ȡ���ĵ�ֵַ
    inline UINT at(UINT uIndex);

protected:
	size_t  nsize_;
	UINT32  m_addrArr[MAX_ADDRARR_NUM];
};

///< ��ȡָ��������ip��ַ
UINT addr_resolver::at(size_t uIndex)
{
    if (uIndex >=0 && uIndex < nsize_) 
    {
        return m_addrArr[uIndex];
    }

    return 0;
}


}}




#endif // !defined(_Klib_IpAddressResolver_H)
