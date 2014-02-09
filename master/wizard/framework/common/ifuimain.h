
#pragma once

#include "wrappercall.h"
#include <functional>


// {79FFDBC0-C326-4745-A559-EA12E4DB03DC}
static const GUID IID_UIMAIN = 
{ 0x79ffdbc0, 0xc326, 0x4745, { 0xa5, 0x59, 0xea, 0x12, 0xe4, 0xdb, 0x3, 0xdc } };


typedef std::function<void()>  SyncFuncType;

struct IUIMain {
    // �������� 
    virtual HWND GetMainWnd() = 0;

    // ͬ�����ã�ί�е���
	virtual BOOL SyncCall(SyncFuncType& fun) = 0;

    // �����Ƿ���ʾͼ��
    virtual BOOL ShowIcon(BOOL bShow) = 0;

    // ����ͼ��
    virtual BOOL SetIcon(HICON) = 0;

    // ��ʼ��˸ͼ��
    virtual BOOL StartFlashIcon() = 0;

    // ֹͣ��˸ͼ��
    virtual BOOL StopFlashIcon() = 0;

    // �����Ƿ�����˸ͼ��
    virtual BOOL IsFlashIcon() = 0;

    // ���������ʾ
    virtual BOOL SetTopMost(BOOL bTopMost) = 0;

    // ��ȡ�Ƿ������
    virtual BOOL GetTopMost() = 0;

    //@todo ��������ӿ�
};