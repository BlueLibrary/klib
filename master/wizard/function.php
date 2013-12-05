<?php

function create_guid() 
{
	$charid = strtoupper(md5(uniqid(mt_rand(), true)));
	$hyphen = chr(45);// "-"
	$uuid = chr(123)// "{"
	.substr($charid, 0, 8).$hyphen
	.substr($charid, 8, 4).$hyphen
	.substr($charid,12, 4).$hyphen
	.substr($charid,16, 4).$hyphen
	.substr($charid,20,12)
	.chr(125);// "}"
	return $uuid;
}

// ����cpp����
function generate_module_hpp($module_name)
{
	$module_name_template = "{<Module_Name>}";

	$template_contents =<<<EOF
#pragma once
#include "stdafx.h"

#include <map>
#include <set>
#include <string>
#include <core/lock_stl.h>
#include <kthread/auto_cs.h>

using namespace klib::stl;

class {<Module_Name>} :
    public klib::aom::module_imp
{
public:
    {<Module_Name>}(void);
    virtual ~{<Module_Name>}(void);

    BEGIN_INTERFACE_ENTRY
        //INTERFACE_ENTRY(IID_IPlug, IPlug)
    END_INTERFACE_ENTRY

protected:
    virtual void on_pre_run();
    virtual BOOL on_run();
    virtual void on_post_run();
    virtual void on_post_stop();

private:
	
};
	
EOF;

	$template_contents = str_replace($module_name_template, $module_name, $template_contents);


	return $template_contents;
}

// ����header����
function generate_module_cpp($module_name)
{
	$module_name_template = "{<Module_Name>}";

	$template_contents =<<<EOF
#include "StdAfx.h"

#include "{<Module_Name>}.h"

{<Module_Name>}::{<Module_Name>}(void) 
{
}

{<Module_Name>}::~{<Module_Name>}(void)
{
}

void {<Module_Name>}::on_pre_run()
{
}

BOOL {<Module_Name>}::on_run()
{
	return TRUE;
}

void {<Module_Name>}::on_post_run()
{
}

void {<Module_Name>}::on_post_stop()
{
}

EOF;

	$template_contents = str_replace($module_name_template, $module_name, $template_contents);
	
	return $template_contents;
}

function generate_dllmain($module_name)
{
	$module_name_template = "{<Module_Name>}";
	
	$template_contents =<<<EOF
// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "{<Module_Name>}.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

BEGIN_REGISTER_MODULES
    REGISTER_MODULE({<Module_Name>})
END_REGISTER_MODULES	
EOF;


	$template_contents = str_replace($module_name_template, $module_name, $template_contents);
	
	return $template_contents;
}

function generate_stdafx_cpp()
{
	$template_contents =<<<EOF
// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// chat_base.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������
	
EOF;

	return $template_contents;
}

function generate_stdafx_hpp()
{
	$template_contents =<<<EOF
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>
#include <WinSock2.h>


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

#include <objbase.h>

#include <aom/iframework.h>
#include <third/duiLib/UIlib.h>

using namespace DuiLib;

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_ud.lib")
#   else
#       pragma comment(lib, "DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_u.lib")
#   else
#       pragma comment(lib, "DuiLib.lib")
#   endif
#endif
EOF;

	return $template_contents;
}
















