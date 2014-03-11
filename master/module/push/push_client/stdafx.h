// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>



// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

#include <aom/iframework.h>
#include <aom/iframework_imp.h>
#include <klib_link.h>

#include <net/udp_client.h>
#include <net/winsock_init.h>
#include <net/winsock_link.h>
#include <net/proto/net_archive.h>
#include <net/proto/command_header.h>

#include <util/stop_watch.h>
#include <core/singleton.h>
#include <debuglog.h>
#include <pattern/fsm.h>
#include <kthread/thread.h>

using namespace klib::core;
using namespace klib::aom;

using namespace klib::net;
using namespace klib::util;
using namespace klib::debug;
using namespace klib::pattern::fsm;
