
#ifndef _Klib_Calendar_H
#define _Klib_Calendar_H

#include "..\..\istddef.h"
#include <windows.h>
#include <tchar.h>


namespace klib {
namespace core {

//@todo ��ȡһ�������ڼ������ڼ���, ���� TDateTime��ʵ��

struct custom_date
{
    int     year_;		        //
    int     month_;			    //
    int     day_;			    //
    BOOL    is_leap_month_;		//
};

class clendar
{
public:
    ///< �ж��Ƿ�������
	BOOL LeapYear(int year);

    ///< ���һ���µ�����
	int GetMonthDays(int year, int month);

    ///< ��ȡ��1900�������
	LONG DaysFrom1900(custom_date Date);

    ///< ���һ�������
	int YearDays(int year);

    ///< ���������
	int LeapDays(int year);

    ///< 
	int LeapMonth(UINT year);

    ///< ��ö�Ӧ��ũ������
	custom_date GetLunar(custom_date date);

    ///< ���һ���µ�����
	int MonthDays(UINT year, UINT month);

    ///< ���ũ����Ϣ
	TCHAR* GetLunarString(custom_date date);

    ///< ������ڵı�ʾ
	tstring GetWeekString(int week);
	
protected:
	TCHAR  m_slunar[100];//用于存储农历信息
};




}}



#endif
