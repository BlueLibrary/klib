
#pragma once

#include <iostream>
#include <algorithm>
#include <sstream>
#include <tchar.h>

namespace klib{
namespace debug{

using namespace std;
template<class _Elem, class _Traits = char_traits<_Elem> >
class CDebugStreamBuf : public basic_stringbuf<_Elem, _Traits >
{    // �ص���������һ��׼��������豸���ַ���������
public:    // ����ص���������false����������_Mystate������ΪBAD�������ټ���д�롣
    typedef bool (__stdcall *_Myof)( const _Elem* );
    explicit inline CDebugStreamBuf( _Myof _Outfunc )
        : _Myoutfunc( _Outfunc ){}    // ��ʼ���ص�����
    virtual ~CDebugStreamBuf( void ){}    // ����������
protected:    // ÿ��ͬ������ˢ�»����е���������
    virtual int sync( void )    // ����sync������������Ҫ���뵽�豸��ͬ����ʱ������
    {    // ����޻ص������򻺳�δ��ʼ�������ش���_Mystate������ΪBAD
        if ( _Myoutfunc == 0 || pptr() == 0 ) return -1;
        else if ( _Mysb::pbase() >= _Mysb::epptr() ) return 0;
        else    // ȷ���������д��
        {    // ����ǰдָ��ĺ�һλ��дΪ0��������泤�Ȳ��㽫����overflowִ�����
            sputc( _Traits::to_char_type( 0 ) );
            setg( pbase(), pbase(), pptr() );    // ����ָ����Ϊ�������
            seekpos( 0, ios_base::out );    // ��дָ����0�Ա��´δ����д��
            for ( ; gptr() < egptr(); )    // ����
            {    //���԰�����ǰָ�����ں��������0ֵ
                for ( ; !sbumpc(); );
                if ( !_Myoutfunc( gptr() - 1 ) ) return -1;    // ����ַ���
                gbump( (int)( find( gptr(), egptr(),    // �ҵ���һ��0ֵ
                    _Traits::to_char_type( 0 ) ) - gptr() + 1 ) );
            }
        }
        return 0;
    }
protected:
    _Myof _Myoutfunc;    // �ص�������Ա�������ڹ���ʱ��ʼ��
};




}}
