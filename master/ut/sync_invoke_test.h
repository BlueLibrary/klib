
#ifndef _klib_sync_invoke_test_h
#define _klib_sync_invoke_test_h


#include "pattern/sync_invoke.h"

using namespace klib::pattern;


TEST(sync_invoke_, 1)
{
    sync_invoke bgWorker;
    bgWorker.start();
    //Sleep(200); // �ȴ��̳߳�ʼ�����

    TCHAR szMsg[100];
    _stprintf(szMsg, _T("��ǰ���߳�ID: %d \r\n"), GetCurrentThreadId());
    _tprintf(szMsg);

    bgWorker.send(
    [&](){
        //MessageBox(NULL, _T("Ҫִ�еĲ���"), _T("����"), 0);
        printf("Ҫִ�еĲ���");
    });


    bgWorker.stop();
}

#endif
