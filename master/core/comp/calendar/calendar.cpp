
#include "calendar.h"

namespace klib {
namespace core {



DWORD LunarInfo[] = 
{
    0x04bd8,0x04ae0,0x0a570,0x054d5,0x0d260,0x0d950,0x16554,0x056a0,0x09ad0,0x055d2,
    0x04ae0,0x0a5b6,0x0a4d0,0x0d250,0x1d255,0x0b540,0x0d6a0,0x0ada2,0x095b0,0x14977,
    0x04970,0x0a4b0,0x0b4b5,0x06a50,0x06d40,0x1ab54,0x02b60,0x09570,0x052f2,0x04970,
    0x06566,0x0d4a0,0x0ea50,0x06e95,0x05ad0,0x02b60,0x186e3,0x092e0,0x1c8d7,0x0c950,
    0x0d4a0,0x1d8a6,0x0b550,0x056a0,0x1a5b4,0x025d0,0x092d0,0x0d2b2,0x0a950,0x0b557,
    0x06ca0,0x0b550,0x15355,0x04da0,0x0a5d0,0x14573,0x052d0,0x0a9a8,0x0e950,0x06aa0,
    0x0aea6,0x0ab50,0x04b60,0x0aae4,0x0a570,0x05260,0x0f263,0x0d950,0x05b57,0x056a0,
    0x096d0,0x04dd5,0x04ad0,0x0a4d0,0x0d4d4,0x0d250,0x0d558,0x0b540,0x0b5a0,0x195a6,
    0x095b0,0x049b0,0x0a974,0x0a4b0,0x0b27a,0x06a50,0x06d40,0x0af46,0x0ab60,0x09570,
    0x04af5,0x04970,0x064b0,0x074a3,0x0ea50,0x06b58,0x055c0,0x0ab60,0x096d5,0x092e0,
    0x0c960,0x0d954,0x0d4a0,0x0da50,0x07552,0x056a0,0x0abb7,0x025d0,0x092d0,0x0cab5,
    0x0a950,0x0b4a0,0x0baa4,0x0ad50,0x055d9,0x04ba0,0x0a5b0,0x15176,0x052b0,0x0a930,
    0x07954,0x06aa0,0x0ad50,0x05b52,0x04b60,0x0a6e6,0x0a4e0,0x0d260,0x0ea65,0x0d530,
    0x05aa0,0x076a3,0x096d0,0x04bd7,0x04ad0,0x0a4d0,0x1d0b6,0x0d250,0x0d520,0x0dd45,
    0x0b5a0,0x056d0,0x055b2,0x049b0,0x0a577,0x0a4b0,0x0aa50,0x1b255,0x06d20,0x0ada0
};


BOOL clendar::LeapYear(int year)
{
	return ((year%4==0 && year%100!=0) || year%400==0);
}

int clendar::GetMonthDays(int year, int month)
{
	switch(month)
	{
	case 1:	case 3:	case 5:	case 7:	case 8:	case 10:case 12: return 31;
	case 4:	case 6:	case 9:	case 11: return 30;
	case 2:
		if(LeapYear(year))
			return 29;
		else
			return 28;
	}
	return 0;
}


LONG clendar::DaysFrom1900(custom_date date)
{
	LONG days;
	days = 365 * date.year_ + (date.year_ - 1) / 4 - (date.year_ - 1) / 100 + (date.year_ - 1) / 400-
		(365*1900+(1900-1)/4-(1900-1)/100+(1900-1)/400);
	for(int i=0;i<date.month_-1;i++)
	{
		days += GetMonthDays(date.year_,i+1);
	}
	days += date.day_;
	return days;
}

int clendar::YearDays(int year)
{
	UINT i, sum = 348; 
	for(i=0x8000; i>0x8; i>>=1)
	{
		sum += (LunarInfo[year-1900] & i) ? 1 : 0;
	}
	return(sum + LeapDays(year));
}

int clendar::LeapDays(int year)
{
	if(LeapMonth(year))
	{
		return (LunarInfo[year-1900] & 0x10000) ? 30: 29;
	}
	else
	{
		return 0;
	}
}

int clendar::LeapMonth(UINT year)
{
	return LunarInfo[year-1900] & 0xf;
}

custom_date clendar::GetLunar(custom_date date)
{
	LONG offset;
	int i, leap=0, temp=0;
	custom_date retdate;
	offset=DaysFrom1900(date)-30;
	for(i=1900; i<2050 && offset>0; i++)
	{
		temp = YearDays(i);
		offset -= temp;
	}
	if(offset<0)
	{
		offset += temp;
		i--;
	}
	retdate.year_ = i;
	leap = LeapMonth(i);
	bool isLeap = false;
	for(i=1; i<13 && offset>0; i++)
	{
		if(leap>0 && i==(leap+1) && isLeap==false)
		{
			--i;
			isLeap = true;
			temp = LeapDays(retdate.year_);
		}
		else
		{
			temp = MonthDays(retdate.year_, i);
		}
		if(isLeap==true && i==(leap+1)) 
		{
			isLeap = false;
		}
		offset -= temp;
	}
	if(offset==0 && leap>0 && i==leap+1)
	{
		if(isLeap)
		{ 
			isLeap = false;
		}
		else
		{
			isLeap = true;
			--i;
		}
	}
	if(offset<=0)
	{
		offset += temp;
		--i;
	}
	retdate.month_ = i;
	retdate.day_ = offset ;
	retdate.is_leap_month_ = isLeap;
	return retdate;
}

int clendar::MonthDays(UINT year, UINT month)
{
	return (LunarInfo[year-1900] & (0x10000>>month))? 30: 29;
}

TCHAR* clendar::GetLunarString(custom_date date)
{
	TCHAR szNongli[30], szNongliDay[10], szShuXiang[10];
	const TCHAR *cTianGan[] = {_T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��")}; //���
	const TCHAR *cDiZhi[]    = {_T("��"), _T("��"), _T("��"), _T("î"), _T("��"), _T("��"), _T("��"), _T("δ"), _T("��"), _T("��"), _T("��"), _T("��")}; //��֧
	const TCHAR *cShuXiang[] = {_T("��"), _T("ţ"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��")}; //��Ф
	const TCHAR *cDayName[] = {_T( "*"), _T("��һ"), _T("����"), _T("����"), _T("����"), _T("����"),
			 _T("����"), _T("����"), _T("����"), _T("����"), _T("��ʮ"), _T("ʮһ"), _T("ʮ��"), _T("ʮ��"), _T("ʮ��"), _T("ʮ��"),
			 _T("ʮ��"), _T("ʮ��"), _T("ʮ��"), _T("ʮ��"), _T("��ʮ"), _T("إһ"), _T("إ��"), _T("إ��"), _T("إ��"), _T("إ��"),       
			 _T("إ��"), _T("إ��"), _T("إ��"), _T("إ��"), _T("��ʮ")}; //ũ������
	const TCHAR *cMonName[] = {_T("*"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("ʮ"), _T("ʮһ"), _T("��")};

	//����ũ����ɡ���֧������
	int nShuXiang = ((date.year_ - 4) % 60) % 12;
	if ( nShuXiang < 0 || nShuXiang >= sizeof(cShuXiang)/sizeof(cShuXiang[0]) )
	{
	   return NULL ;
	}
	wsprintf(szShuXiang, _T("%s"), cShuXiang[nShuXiang]);
	int nTianGan = ((date.year_ - 4) % 60) % 10;
	if ( nTianGan < 0 || nTianGan >= sizeof(cTianGan)/sizeof(cTianGan[0]) )
	{
	   return NULL;
	}
	int nDiZhi = ((date.year_ - 4) % 60) % 12;
	if ( nDiZhi < 0 || nDiZhi >= sizeof(cDiZhi)/sizeof(cDiZhi[0]) )
	{
	   return NULL;
	}

	wsprintf(szNongli, _T("%s(%s%s)��"), szShuXiang, cTianGan[nTianGan], cDiZhi[nDiZhi]);

	//����ũ���¡���
	if( date.month_ < 0 || date.month_ >= sizeof(cMonName)/sizeof(cMonName[0]) )
	{
		return NULL;
	}

	if(date.is_leap_month_)
	{
		wsprintf(szNongliDay, _T("��%s"), cMonName[date.month_]);
	}
	else
	{
		wcscpy(szNongliDay, cMonName[date.month_]);
	}

	_tcscat(szNongliDay, _T("��"));
	if( date.day_ < 0 || date.day_ >= sizeof(cDayName)/sizeof(cDayName[0]) )
	{
		return NULL;
	}

	_tcscat(szNongliDay, cDayName[date.day_]);
	_tcscat(szNongli, szNongliDay);
	_stprintf(m_slunar, szNongli);
	return m_slunar;
}

tstring clendar::GetWeekString(int week)
{
	switch(week)
	{
	case 1:
		return _T("����һ");
	case 2:
		return _T("���ڶ�");
	case 3:
		return _T("������");
	case 4:
		return _T("������");
	case 5:
		return _T("������");
	case 6:
		return _T("������");
	case 7:
		return _T("������");
	}

    _ASSERT(FALSE);
    return _T("");
}


}}
