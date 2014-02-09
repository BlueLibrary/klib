#pragma once

#include "core/singleton.h"

//----------------------------------------------------------------------
// ����ģ�飬��Ҫ�ṩ���ݽ�����صĲ���
//   �磺 �������ݿ⣬�������ݣ�������ʼ����
class CDataModule:
    public klib::aom::module_imp
{
public:
    CDataModule(void);
    ~CDataModule(void);


    BEGIN_INTERFACE_ENTRY
    END_INTERFACE_ENTRY

protected:
    void on_pre_run();
    BOOL on_run();
    void on_post_run();
    void on_pre_stop();
    void on_stop();
    void on_post_stop();

};

