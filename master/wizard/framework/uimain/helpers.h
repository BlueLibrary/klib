#pragma once

#include <core/singleton.h>
#include <core/lock_stl.h>

#include "..\common\ifchatdef.h"

using namespace klib::core;
using namespace klib::stl;
using namespace klib::kthread;

// ������б�
class CPlugList : 
    public lock_list<IPlug*>,
    public singleton<CPlugList>
{
public:
};

// ���������
class CTaskMgr : 
    public singleton<CTaskMgr>,
    public klib::core::task_mgr
{
    DECLARE_SINGLETON_CLASS(CTaskMgr);

};
