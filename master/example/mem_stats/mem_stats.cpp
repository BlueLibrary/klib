// mem_stats.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <comp/mem_check/library/src/mem_lib.h>
#include <comp/mem_check/library/src/object_creator.h>

#pragma comment(lib, "E:/project/git/klib/master/core/lib/Debug/mem_check_library.lib")

int _tmain(int argc, _TCHAR* argv[])
{

    Type(int).create();

	return 0;
}

