// proxy_server.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "proxy_server_t.h"

int _tmain(int argc, _TCHAR* argv[])
{

    proxy_server_t* p = proxy_server_t::instance();
    p->start();


	return 0;
}

