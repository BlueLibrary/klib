// kl_ex.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "../kl/kl.h"
#include "../kl/variable.h"

int _tmain(int argc, _TCHAR* argv[])
{
    variable var;
    var = 10;
    var = "hello";
    var = 1.1;
    
    variable str_val = "today is a good day";

    var["hello"] = "world";
    var["key"] = str_val;
    var["key"]["key2"] = "value";

	return 0;
}

