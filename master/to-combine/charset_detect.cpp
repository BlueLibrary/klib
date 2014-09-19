// charset_detect.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "uchardet-master/src/uchardet.h"
#include <string>
#include <stdint.h>
using namespace std;

/*

int uchardet_handle_data(uchardet_t ud, const char * data, size_t len);

void uchardet_data_end(uchardet_t ud);

void uchardet_reset(uchardet_t ud);

const char * uchardet_get_charset(uchardet_t ud);

*/

int isutf8(const char *s, size_t ns)
{
    uint8_t x = 0, i = 0, j = 0, nbytes = 0, n = 0;

    for(i = 1; i < 7; i++)
    {
        x = (uint8_t)(255 << i); 
        if(((uint8_t)*s & x) == x)
        {
            n = nbytes = (8 - i);
            for(j = 0; (j < nbytes && j < ns); j++)
            {
                if((uint8_t)s[j] <= 0x80 && (uint8_t)s[j] >= 0xc0)break;
                else n--;
            }
            if(n == 0) return nbytes;
        }
    }
    return 0;
}

bool IsUTF8(const void* pBuffer, long size)  
{  
    bool IsUTF8 = true;  
    unsigned char* start = (unsigned char*)pBuffer;  
    unsigned char* end = (unsigned char*)pBuffer + size;  
    while (start < end)  
    {  
        if (*start < 0x80) // (10000000): ֵС��0x80��ΪASCII�ַ�  
        {  
            start++;  
        }  
        else if (*start < (0xC0)) // (11000000): ֵ����0x80��0xC0֮���Ϊ��ЧUTF-8�ַ�  
        {  
            IsUTF8 = false;  
            break;  
        }  
        else if (*start < (0xE0)) // (11100000): �˷�Χ��Ϊ2�ֽ�UTF-8�ַ�  
        {  
            if (start >= end - 1)   
                break;  
            if ((start[1] & (0xC0)) != 0x80)  
            {  
                IsUTF8 = false;  
                break;  
            }  
            start += 2;  
        }   
        else if (*start < (0xF0)) // (11110000): �˷�Χ��Ϊ3�ֽ�UTF-8�ַ�  
        {  
            if (start >= end - 2)   
                break;  
            if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80)  
            {  
                IsUTF8 = false;  
                break;  
            }  
            start += 3;  
        }   
        else  
        {  
            IsUTF8 = false;  
            break;  
        }  
    }  
    return IsUTF8;  
}  


bool IsTextUTF8(const char* str, size_t length)
{
    size_t i = 0;
    unsigned int nBytes=0;//UFT8����1-6���ֽڱ���,ASCII��һ���ֽ�
    unsigned char chr;
    bool  bAllAscii = true; //���ȫ������ASCII, ˵������UTF-8
    for(i=0; i<length; i++)
    {
        chr= *(str+i);
        if( (chr&0x80) != 0 ) // �ж��Ƿ�ASCII����,�������,˵���п�����UTF-8,ASCII��7λ����,����һ���ֽڴ�,���λ���Ϊ0,o0xxxxxxx
        {
            bAllAscii= false;
        }

        if(nBytes == 0) //�������ASCII��,Ӧ���Ƕ��ֽڷ�,�����ֽ���
        {
            if(chr >= 0x80)
            {
                if(chr>=0xFC&&chr<=0xFD)
                    nBytes=6;
                else if(chr>=0xF8)
                    nBytes=5;
                else if(chr>=0xF0)
                    nBytes=4;
                else if(chr>=0xE0)
                    nBytes=3;
                else if(chr>=0xC0)
                    nBytes=2;
                else
                {
                    return false;
                }
                nBytes--;
            }
        }
        else //���ֽڷ��ķ����ֽ�,ӦΪ 10xxxxxx
        {
            if((chr&0xC0) != 0x80)
            {
                return false;
            }
            nBytes--;
        }
    }

    if( nBytes > 0 ) //Υ������
    {
        return false;
    }

    if( bAllAscii ) //���ȫ������ASCII, ˵������UTF-8
    {
        return true;
    }

    return true;
}

int isgbk(const char *s, size_t ns)
{
    if(ns > 2 && (uint8_t)*s >= 0x81 && (uint8_t)*s <= 0xfe && ( ((uint8_t)*(s+1) >= 0x80 && (uint8_t)*(s+1) <= 0x7e) || ((uint8_t)*(s+1) >= 0xa1 && (uint8_t)*(s+1) <= 0xfe) ) )
    {
        return 1;
    }
    return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
    std::string strutf8 = "test utf8 charset";
    std::string str = "dbgbghh";
    std::wstring wstr = L"���Ĳ������ݣ����������� �ļ��༭";
    std::string strCharset;

    uchardet_t handle = uchardet_new();

    _ASSERT(IsUTF8(strutf8.c_str(), strutf8.size()));
    _ASSERT(IsTextUTF8(strutf8.c_str(), strutf8.size()));


//     _ASSERT(isgbk(str.c_str(), str.size()));
//     _ASSERT(isutf8(strutf8.c_str(), strutf8.size()));

    
    if (0 == uchardet_handle_data(handle, (char*)str.c_str(), str.size())) 
    {
    }
    
    uchardet_data_end(handle);

    strCharset = uchardet_get_charset(handle);

    fprintf(stdout, "charset is %s:%s\n", "", strCharset.c_str());

    uchardet_delete(handle);

	return 0;
}

