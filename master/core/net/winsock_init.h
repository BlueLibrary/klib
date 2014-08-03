// UdpHandler.h: interface for the CUdpHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_Klib_WinSockInit_H)
#define _Klib_WinSockInit_H

#include <WinSock2.h>
#include <windows.h>


/*
* @brief	��ʼ��winsock���ֻ࣬��Ҫ������ͷ�ļ�����
*/

namespace klib {
namespace net {
    

class winsock_init 
{
public:
	winsock_init() 
    {
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2,2), &wsaData);
	}

    ~winsock_init()
    {
        WSACleanup();
    }
};


}}


#endif 
