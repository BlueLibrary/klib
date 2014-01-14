#ifndef _Advertisement_COMMAND_HEADER_H_
#define _Advertisement_COMMAND_HEADER_H_

// ����ṹ
typedef class _COMMAND_HEADER {
public:
	//USHORT   cbSize;	 //�ṹ�ܳ���(��������,�ڽṹ���в����ֳ���)
	UCHAR		ver;		 //�汾
	USHORT		cmd;		 //��������
	UCHAR		encrypt;	 //ʹ�õļ�������
	UINT		pktNo;	     //������

public:
	_COMMAND_HEADER(USHORT uCmd = 0) 
	{
		static UINT  pktNoCounter = 0;
		static CRITICAL_SECTION     cs;
		if (0 == pktNoCounter) 	
		{
			InitializeCriticalSection(&cs);
		}

		EnterCriticalSection(&cs);
		{
			pktNo = ++ pktNoCounter;
		}
		LeaveCriticalSection(&cs);
		encrypt = 1;
		ver = ADVERTISE_VERSION;
		cmd = uCmd;
	}

	friend net_archive& operator << (net_archive& ar, _COMMAND_HEADER& pt) {
		//ar << pt.cbSize;
		ar << pt.ver;
		ar << pt.cmd;
		ar << pt.encrypt;
		//ar << pt.dwOffset;
		return ar;
	}

	friend net_archive& operator >> (net_archive& ar, _COMMAND_HEADER& pt) {
		//ar >> pt.cbSize;
		ar >> pt.ver;
		ar >> pt.cmd;
		ar >> pt.encrypt;
		//ar >> pt.dwOffset;
		return ar;
	}
} COMMAND_HEADER, *PCOMMAND_HEADER;

#endif