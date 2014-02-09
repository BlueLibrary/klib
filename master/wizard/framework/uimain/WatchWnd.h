#pragma once

#include "UIWindow.h"

class CWatchWnd : public DuiLib::CWindowUI
{
public:
    CWatchWnd(CPaintManagerUI* pPaintManager);
    CWatchWnd();
    ~CWatchWnd(void);

public:
    LPCTSTR GetWindowClassName() const { return _T("CWatchWnd"); }
    UINT    GetClassStyle()      const { return CS_DBLCLKS; }

public:
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    //----------------------------------------------------------------------
    // Summary;:
    //      ������Ϣ������
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
    CPaintManagerUI* m_pMainWndManager;
};

