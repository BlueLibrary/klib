// push_test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <string>

#include "push_client_test.h"

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 2) {
        return -1;
    }

    if (_tcsicmp(argv[1], _T("client")) == 0) 
    {
        push_client_test::instance()->start();
    }
    else if (_tcsicmp(argv[1], _T("server")) == 0) 
    {

    }

	return 0;
}

