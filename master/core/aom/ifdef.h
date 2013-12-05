#pragma once

#include "istddef.h"

// ����ӿڵ�
// {3ADB422F-65EF-4359-81E6-F26E23E760E8}
static const GUID IID_IUtil = 
{ 0x3adb422f, 0x65ef, 0x4359, { 0x81, 0xe6, 0xf2, 0x6e, 0x23, 0xe7, 0x60, 0xe8 } };

// {8CC8506C-6AB9-4406-8DD9-9CAD995BBD11}
static const GUID IID_INetwork = 
{ 0x8cc8506c, 0x6ab9, 0x4406, { 0x8d, 0xd9, 0x9c, 0xad, 0x99, 0x5b, 0xbd, 0x11 } };

// {21297091-D456-4F1D-879F-729205D492F1}
static const GUID IID_ICalendar = 
{ 0x21297091, 0xd456, 0x4f1d, { 0x87, 0x9f, 0x72, 0x92, 0x5, 0xd4, 0x92, 0xf1 } };



struct IUtil 
{
    //���Ӧ�ó����·�����з�б��
    virtual std::string GetAppPath() = 0;
    //���Ӧ�ó����Ŀ¼���޷�б��
    virtual std::string GetAppDir() = 0;

    //���Ӧ�ó�������,���� ox.exe
    virtual std::string GetAppName() = 0;

    //����ļ���չ��
    virtual bool GetFileExt(std::string& strExt, const char* filepath) = 0;
    virtual bool GetFileExtW(std::wstring& strExt, const wchar_t* filepath) = 0;
    virtual bool GetFileName(std::string& strFileName, const char* filepath) = 0;
    virtual bool GetFileNameW(std::wstring& strFileName, const wchar_t* filepath) = 0;
    virtual bool GetFilePath(std::string& strPath, const char* filepath) = 0;
    virtual bool GetFilePathW(std::wstring& strPath, const wchar_t* filepath) = 0;
    virtual bool GetFileDir(std::string& strDir, const char* filepath) = 0;
    virtual bool GetFileDirW(std::wstring& strDir, const wchar_t* filepath) = 0;

    virtual bool ExpandEnvironment(std::string& deststr, const char* strVariable) = 0;

    /*
    ʹ��ie��url��ע�����ʹ��Ĭ�ϵ�������򲻿���ʱ��Ҫ��ע������.htm����������������ҳ
    */
    virtual bool OpenURL(LPCTSTR pszURL) = 0;

    virtual bool HasFullScreenProgram() = 0;
    //�����Զ�����
    virtual bool SetAppRunBoot(LPCTSTR pszStartTitle, bool bBoot = true) = 0;

    virtual bool ReduceMemory() = 0; //�����ڴ�ʹ��

    virtual bool CreateDesktopUrl(LPCTSTR pszName, const char* url, DWORD hotKey) = 0; //����url����
    virtual bool CreateUrl(LPCTSTR pszPath, const char* url, DWORD hotKey) = 0; //����url����
    //----------------------------------------------------------------------
    // Summary:
    //		�����༶Ŀ¼
    // Parameters:
    //		pszDirPath		-	
    // Returns:
    //		
    //----------------------------------------------------------------------
    virtual BOOL MakeSureDirectoryPathExists(LPCTSTR pszDirPath) = 0;

    virtual bool GetExeNameFromWnd(HWND hWnd, LPTSTR  pszBuf, UINT nSize) = 0; //���ݴ��ھ������ȡ��Ӧ�ó���·��

    virtual bool RefreshDesktop() = 0; //ˢ������
    virtual HWND FixOnDesktop(HWND hWndToFix, bool bFix = true) = 0;  //��������

    //��ȡ����ϵͳ��Ϣ
    virtual bool GetOsInfo(OSVERSIONINFOEX& osvi) = 0;

    //��ȡ����ϵͳ�汾
    virtual bool GetOsVersion(std::string &version) = 0;

    //�ж��Ƿ�����Ŀ¼�µ�
    virtual bool IsChildDir(const char* strMain, const char* strSub, bool bCheckSub) = 0;

    //�ж�һ���ַ����Ƿ�����ַ
    virtual bool IsWebSite(const char *str) = 0;
    virtual bool SetDirectoryAliasName(LPCTSTR pszDir, LPCTSTR pszAliasName) = 0; //�����ļ��еı���
    //������·��ת��Ϊ���·��
    virtual bool AbsolutePath2RelativePath(std::string& strPath, const char* strBasePath, const char* target, const char* PathVar="{AppPath}") = 0;

    virtual DWORD GetCPUUse() = 0; //���cpu��ʹ���ʣ�0~100
    virtual BOOL EnablePrivilege(LPCTSTR lpszPrivilegeName, BOOL bEnable) = 0;
    virtual bool IsSystemDir(LPCTSTR pszPath) = 0;    //�Ƿ���ϵͳĿ¼
    virtual BOOL IsEnableUAC() = 0;       //�Ƿ�������UAC
    virtual BOOL IsAdmin() =0;            //�Ƿ���admin�û�
    virtual BOOL ShutdownSys(BOOL bReboot = FALSE) = 0;       //�رռ�����������������
    virtual BOOL IsProcessRun(LPCTSTR pszExeName) = 0; //�鿴ָ���Ľ����Ƿ�������

    virtual BOOL FilterSqlParam(std::string& str) = 0;
    virtual BOOL FilterQueryParam(std::string& str, const char chEscape = '\\') = 0;

    virtual BOOL IsTextUTF8(const char* str,ULONGLONG length) = 0;      //�ж��Ƿ���utf8�����
};



typedef std::map<std::string, std::string> PostParam;

enum emTestNetworkType {
    //----------------------------------------------------------------------
    // �������������ж�
    //----------------------------------------------------------------------
    emTestHostName,      

    //----------------------------------------------------------------------
    // ����pingָ��������
    //----------------------------------------------------------------------
    emTestPing,

    //----------------------------------------------------------------------
    // ���������������ж�
    //----------------------------------------------------------------------
    emTestBoth
};

//----------------------------------------------------------------------
// Summary:
//      ���������ؽӿ�
//----------------------------------------------------------------------
struct INetwork
{
public:
    //----------------------------------------------------------------------
    // Summary:
    //		�ж������Ƿ����ӵ�
    // Parameters:
    //		type		-       �������
    // Returns:
    //		�������˷���true,ʧ�ܷ���false
    //----------------------------------------------------------------------
    virtual bool IsConnectedByTest(emTestNetworkType type) = 0; 

    //----------------------------------------------------------------------
    // Summary:
    //		 �ж��Ƿ��������ˣ�ֱ�ӷ��ؽ��
    // Parameters:
    // Returns:
    //		���������ӵĻ�����true��û�����ӵĻ�����false
    //----------------------------------------------------------------------
    virtual bool GetIsConnected() = 0;

    //----------------------------------------------------------------------
    // Summary:
    //		��ȡurl������
    // Parameters:
    //		url		-	
    //		content		-	
    // Returns:
    //		�ɹ�����true��ʧ�ܷ���false
    //----------------------------------------------------------------------
    virtual bool GetUrlString(const char* url, std::string& strContent) = 0;//���url������

    //----------------------------------------------------------------------
    // Summary:
    //      ����url�����ļ�
    //----------------------------------------------------------------------
    virtual bool URLDownloadFile(const char* url, const char* destpath) = 0; 
};

//----------------------------------------------------------------------
// Summary:
//      ����ģ��
//----------------------------------------------------------------------
struct ICalendar
{
public:
    //----------------------------------------------------------------------
    // Summary:
    //      �õ������� ����0~6
    //----------------------------------------------------------------------
    virtual bool  GetWeeks(int& week,int year, int month, int day) =0;

    //----------------------------------------------------------------------
    // Summary:
    //      ָ���·�(����)�õ������ж�����
    //----------------------------------------------------------------------
    virtual bool GetDaysAccordingMonth(int& days,int year,int month) =0;

    //----------------------------------------------------------------------
    // Summary:
    //      �ô�ũ���ַ��� �磺���� �����³�һ
    //----------------------------------------------------------------------
    virtual bool  GetLunarStr(std::string& szLunar,int year,int month,int day)=0;

    //----------------------------------------------------------------------
    // Summary:
    //      �õ���ɵ�֧  �磺 ��î�� ������ �ҳ���
    //----------------------------------------------------------------------
    virtual bool  GetGanZhiStr(std::string& szGanZhi,int year,int month,int day)=0;

    //�˼� szShould������  szAvoid���ؼɣ���Ҫ���ݿ�֧��
    //virtual bool GetShouldAvoid(std::string& szShould,std::string& szAvoid,int year,int month,int day) = 0;

    //----------------------------------------------------------------------
    // Summary:
    //      ���� �磺 �����ʮ���� ����ĸ�׽�
    //----------------------------------------------------------------------
    virtual bool GetFestival(std::string& szFestival,int year,int month,int day) = 0;
};
