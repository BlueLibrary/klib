
#include <stdlib.h>
#include <string>
#include <windows.h>

#include "datetime.h"

using namespace klib::core;

//�й���ʱ������Ϊ�������εĶ�����������Ҫһ��ʱ��ƫ����
const int TIMEZONE_8 = 8*60*60;

tdatetime::tdatetime()
{
    LoadCurrentDateTime();
    m_TimeZoneOffset = 0;

    //! ��ȡ�û��������õ�ʱ��
    TIME_ZONE_INFORMATION tzi;
    GetTimeZoneInformation(&tzi);
    int zone = tzi.Bias/ -60;
    m_TimeZoneOffset = zone * 60 * 60;
}

tdatetime::tdatetime(time_t t)
{
    FromUnixDatetime(t);
}    

tdatetime::TokenType 
tdatetime::GetTokenType(char c)
{
    if ((c>='0') && (c<='9'))
        return TT_Number;
    else if ('-'==c){
        return TT_Minus;
    }else if ('/'==c){
        return TT_Minus;
    }else if (' ' == c){
        return TT_Blank;
    }else if(':'==c){
        return TT_Colon;
    }else{
        return TT_Null;
    }
}

BOOL 
tdatetime::ParseDateTimeString(const char *szDateTime)
{
    int len = strlen(szDateTime) - 1;
    TimePart timePart = TP_Second;
    int pw =1;//��Ȩ
    signed short year=-1;
    signed char month=-1,day=-1,hour=-1,minute=-1,second=-1;
    //����β���ո�
    while((len>=0) && (' ' == szDateTime[len]))
        len--;
    if (len<0) return FALSE;


    while(len>=0){

        char c = szDateTime[len];
    
        switch( GetTokenType(c))
        {
        case TT_Null: goto ParseDateTimeString_FALSE;
        case TT_Number:
            switch(timePart)
            {
            case TP_Second:
                if (second<0 )
                    second = c-'0';
                else
                    second += (c-'0') * pw;
                pw *= 10;
                break;
            case TP_Minute:
                if (minute<0 )
                    minute = c-'0';
                else
                    minute += (c-'0') * pw;
                pw *= 10;
                break;
            case TP_Hour:
                if (hour<0 )
                    hour = c-'0';
                else
                    hour += (c-'0') * pw;
                pw *= 10;
                break;
            case TP_Day:
                if (day<0 )
                    day = c-'0';
                else
                    day += (c-'0') * pw;
                pw *= 10;
                break;
            case TP_Month:
                if (month<0 )
                    month = c-'0';
                else
                    month += (c-'0') * pw;
                pw *= 10;
                break;
            case TP_Year:
                if (year<0 )
                    year = c-'0';
                else
                    year += (c-'0') * pw;
                pw *= 10;
                break;
            default:
                return FALSE;
            }
            break;
        case TT_Minus:
            switch(timePart)
            {
            case TP_Second: //���û�и���ʱ����Ϣ����������ֱ������������
                pw =1;
                timePart = TP_Month; //����                
                day = second;
                second = -1;  //��������������Ϣ��Ϊ������Ϣ
                break;
            case TP_Minute: goto ParseDateTimeString_FALSE;                
            case TP_Hour: goto ParseDateTimeString_FALSE;
            case TP_Day:
                pw =1;
                timePart = TP_Month; //����                
                break;
            case TP_Month:
                pw =1;
                timePart = TP_Year; //����                
                break;
            case TP_Year: goto ParseDateTimeString_FALSE;                
            default: goto ParseDateTimeString_FALSE;
            }
            break;
        case TT_Colon:
            switch(timePart)
            {
            case TP_Second:
                pw =1;
                timePart = TP_Minute; //����                    
                break;
            case TP_Minute: 
                pw =1;
                timePart = TP_Hour; //����                    
                break;
            case TP_Hour: goto ParseDateTimeString_FALSE;
            case TP_Day: goto ParseDateTimeString_FALSE;                
            case TP_Month: goto ParseDateTimeString_FALSE;                
            case TP_Year: goto ParseDateTimeString_FALSE;                
            default: goto ParseDateTimeString_FALSE;
            }
            break;
        case TT_Blank:
            if (TP_Hour == timePart){
                timePart = TP_Day; //����
                pw =1;
            }else if (TP_Year == timePart){  //ǰ���ո�
                goto ParseDateTimeString_OK;//��Ϊ����                
            }else{//���������ֲ��ܳ��ֿո�
                goto ParseDateTimeString_FALSE;
            }
            break;
        }
        len -- ;
    }

ParseDateTimeString_OK:
    if (year>0)
        sYear = year;
    if (month>0)
        sMonth = month;
    if (day>0)
        sDay = day;
    if (hour>=0)
        sHour = hour;
    if (minute>=0)
        sMinute = minute;
    if (second>=0)
        sSecond = second;

    if (Validate()){
        return TRUE;
    }else{
        ZeroDateTime();
        return FALSE;
    }

    ParseDateTimeString_FALSE:
    ZeroDateTime();
    return FALSE;
}

BOOL
tdatetime::LoadCurrentDateTime()
{    
    time_t t;
    time(&t);
    FromUnixDatetime(t);    
    return TRUE;
}

void
tdatetime::FromUnixDatetime(time_t t)
{
    t += TIMEZONE_8;
    struct tm tt;
    errno_t err = gmtime_s(&tt, &t);
    sYear = tt.tm_year+1900;
    sMonth = tt.tm_mon+1;
    sDay = tt.tm_mday;
    sHour = tt.tm_hour;
    sMinute = tt.tm_min;
    sSecond = tt.tm_sec;    
    //free(tt);
}

void 
tdatetime::ZeroDateTime(void)
{
    sYear = 0;
    sMonth = 0;
    sDay = 0;
    sHour = 0;
    sMinute = 0;
    sSecond = 0;
}


/// <summary>
/// �ж�����������Ƿ�������
/// </summary>
/// <param name="year">��Ҫ�ж������</param>
/// <returns>true:��������������ꡣfalse:��������ݲ������ꡣ</returns>
BOOL 
tdatetime::IsLeapYear(int year)
{
    return ((year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0));                               
}

/// <summary>
/// �ж�����������Ƿ���Ч��
/// </summary>
/// <param name="year">���������</param>
/// <returns>true:��Ч,false:��Ч</returns>
BOOL 
tdatetime::ValidateDate(int year)
{
    return (year > 0) && (year <= 9999);
}

/// <summary>
/// �ж������������Ƿ���Ч
/// </summary>
/// <param name="year">���������</param>
/// <param name="month">�������·�</param>
/// <returns>true:��Ч��false:��Ч��</returns>
BOOL 
tdatetime::ValidateDate(int year,int month)
{
    if (!ValidateDate(year))
        return FALSE;
    return (month > 0) && (month < 13);
}

/// <summary>
/// �õ�һ���·ݵ�����
/// </summary>
/// <param name="year">��</param>
/// <param name="month">��</param>
/// <returns>���ظ�����µ�����������������Ĳ����д����򷵻�0</returns>
int 
tdatetime::GetDaysOfMonth(int year, int month)
{
    if (!ValidateDate(year, month))
    {
        return 0;
    }

    if (month == 4 || month == 6 || month == 9 || month == 11)
    {
        return 30;
    }
    else if (month == 1 || month == 3 || month == 5
        || month == 7 || month == 8 || month == 10 || month == 12)
    {
        return 31;
    }
    else if (2 == month)
    {
        if (IsLeapYear(year))//���������
        {
            return 29;
        }
        else
        {
            return 28;
        }
    }

    return 0;
}

/// <summary>
/// �õ�һ�������ڼ�
/// </summary>
/// <param name="year">��</param>
/// <param name="month">��</param>
/// <param name="day">��</param>
/// <returns>���ظ�����¸��������ڼ�����������Ĳ����д����򷵻�-1����0xFFFFFFFF</returns>
int 
tdatetime::GetDayOfWeek(int year, int month, int day)
{
  if (!ValidateDate(year, month, day)) {
    return -1;
  }
  
  int dayOfWeek = 0;
  if((month==1)||(month==2))//�����һ�»���½��л���
  {
    month += 12;
    year --;
  }
  
  dayOfWeek =(day+1 + 2*month + 3*(month+1)/5 + year + year/4 - year/100 + year/400)%7; //�õ������ڼ�������

  /*
  case 0:printf("���������Ϊ������\n");break;
  case 1:printf("���������Ϊ����һ\n");break;
  case 2:printf("���������Ϊ���ڶ�\n");break;
  case 3:printf("���������Ϊ������\n");break;
  case 4:printf("���������Ϊ������\n");break;
  case 5:printf("���������Ϊ������\n");break;
  case 6:printf("���������Ϊ������\n");break;
  */

  return dayOfWeek;
}

/// <summary>
/// �ж��������������Ƿ���һ����Ч������
/// </summary>
/// <param name="year">���������</param>
/// <param name="month">�������·�</param>
/// <param name="day">����������</param>
/// <returns>true:��������������һ����Ч�����ڡ�false:����һ����Ч�����ڡ�</returns>
BOOL 
tdatetime::ValidateDate(int year, int month, int day)
{
    if (!ValidateDate(year, month))
        return FALSE;

    if ((day < 1) || (day > GetDaysOfMonth(year, month)))
        return FALSE;

    return TRUE;                        
}

/// <summary>
/// �ж�������С���Ƿ���Ч
/// </summary>
/// <param name="hour">������Сʱ</param>
/// <returns>true:��Ч;false:��Ч</returns>
BOOL 
tdatetime::ValidateTime(int hour)
{
    return (hour >= 0) && (hour < 24);
}

/// <summary>
/// �ж�������Сʱ�ͷ����Ƿ���Ч��
/// </summary>
/// <param name="hour">������Сʱ</param>
/// <param name="minute">�����ķ���</param>
/// <returns>true:��Ч;false:��Ч</returns>
BOOL 
tdatetime::ValidateTime(int hour,int minute)
{
    if (!ValidateTime(hour))
        return FALSE;
    return (minute >= 0) && (minute < 60);
}

/// <summary>
/// �ж�������Сʱ�����ӡ���ʱ����Ч
/// </summary>
/// <param name="hour">������Сʱ</param>
/// <param name="minute">�����ķ���</param>
/// <param name="second">��������</param>
/// <returns>true:��Ч;false:��Ч</returns>
BOOL 
tdatetime::ValidateTime(int hour, int minute, int second)
{
    if (!ValidateTime(hour,minute))
        return FALSE;
    return (second >= 0) && (second < 60);
}

/// <summary>
/// �ж�������������ʱ�����Ƿ���һ����Ч������ʱ��
/// </summary>
/// <param name="year">���������</param>
/// <param name="month">�������·�</param>
/// <param name="day">����������</param>
/// <param name="hour">������Сʱ</param>
/// <param name="minute">�����ķ���</param>
/// <param name="second">��������</param>
/// <returns>true:��Ч;false:��Ч</returns>
BOOL 
tdatetime::ValidateDateTime(int year, int month, int day,
              int hour, int minute, int second)
{
    return ValidateDate(year, month, day)
        && ValidateTime(hour, minute, second);
}

BOOL 
tdatetime::Validate()
{
    return Validate(this);
}

int tdatetime::getResult(tdatetime& date)
{
  int result = 0;
  do {
    result = this->sYear - date.sYear;
    if (result != 0) break;

    result = this->sMonth - date.sMonth;
    if (result != 0) break;

    result = this->sDay - date.sDay;
    if (result != 0) break;

    result = this->sHour - date.sHour;
    if (result != 0) break;

    result = this->sMinute - date.sMinute;
    if (result != 0) break;

    result = this->sSecond - date.sSecond;
    if (result != 0) break;
  } while(false);

  return result;
}

bool 
tdatetime::operator > (tdatetime& date)
{
  int result = this->getResult(date);

  return (result>0 ? true : false);
}

bool tdatetime::operator >= (tdatetime& date)
{
  int result = this->getResult(date);

  return (result>=0 ? true : false);
}

bool tdatetime::operator < (tdatetime& date)
{
  int result = this->getResult(date);

  return (result<0 ? true : false);
}

bool tdatetime::operator <= (tdatetime& date)
{
  int result = this->getResult(date);

  return (result<=0 ? true : false);
}

tdatetime& tdatetime::AddSeconds(tdatetime& tm, UINT64 uSecondsToAdd)
{
  time_t tUnixTime = tm.ToUnixDatetime();
  tUnixTime += uSecondsToAdd;
  tm.FromUnixDatetime(tUnixTime);
  return tm;
}

BOOL 
tdatetime::Validate(tdatetime *obDateTime)
{
    return ValidateDateTime(obDateTime->sYear,obDateTime->sMonth, obDateTime->sDay,
        obDateTime->sHour, obDateTime->sMinute, obDateTime->sSecond);
}

time_t 
tdatetime::ToUnixDatetime()
{    
    tm tt;
    tt.tm_year = sYear - 1900;
    tt.tm_mon = sMonth -1;
    tt.tm_mday = sDay;
    tt.tm_hour = sHour;
    tt.tm_min = sMinute;
    tt.tm_sec = sSecond;
    return mktime(&tt); 
}

std::string
tdatetime::toString()
{
  char buff[50] = {0};
  sprintf_s(buff,
          sizeof(buff),
          "%d-%d-%d %d:%d:%d",
          this->sYear,
          this->sMonth,
          this->sDay,
          this->sHour,
          this->sMinute,
          this->sSecond);

  return string(buff);
}