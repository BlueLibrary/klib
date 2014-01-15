#ifndef _klib_cmd_header_h_
#define _klib_cmd_header_h_

#include <windows.h>

namespace klib {
namespace net {


// �������
class cmd_header_builder
{
    cmd_header_builder() : pkt_no_(0) 
    {
        InitializeCriticalSection(&mutex_);
    }
    ~cmd_header_builder() { DeleteCriticalSection(&mutex_); }
public:
    static cmd_header_builder* instance()
    {
        static cmd_header_builder _instance;
        return & _instance;
    }

    UCHAR ver() { return 1;}
    UINT  gen_pkt_no() 
    {
        UINT no_ = 0;
        EnterCriticalSection(&mutex_);
        no_ =  pkt_no_ ++;
        LeaveCriticalSection(&mutex_);
        return no_;
    } 

protected:
    UINT32   pkt_no_;
    CRITICAL_SECTION  mutex_;
};


// ����ṹ
class cmd_header {
public:
	//USHORT   cbSize;	 //�ṹ�ܳ���(��������,�ڽṹ���в����ֳ���)
	UCHAR		ver;		 //�汾
	USHORT		cmd;		 //��������
	UCHAR		encrypt;	 //ʹ�õļ�������
	UINT		pktNo;	     //������

public:
	cmd_header(USHORT uCmd = 0) 
	{
        static cmd_header_builder* builder_ = cmd_header_builder::instance();

        pktNo   = builder_->gen_pkt_no();
        ver     = builder_->ver();
		encrypt = 0;
		cmd     = uCmd;
	}

	friend net_archive& operator << (net_archive& ar, cmd_header& pt) {
		//ar << pt.cbSize;
		ar << pt.ver;
		ar << pt.cmd;
		ar << pt.encrypt;
		return ar;
	}

	friend net_archive& operator >> (net_archive& ar, cmd_header& pt) {
		//ar >> pt.cbSize;
		ar >> pt.ver;
		ar >> pt.cmd;
		ar >> pt.encrypt;
		return ar;
	}
} ;


}}


#endif