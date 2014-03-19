#include "code_convert.h"

#include<windows.h>

namespace klib {
namespace encode {



code_convert::code_convert(void)
{
}

code_convert::~code_convert(void)
{
}

std::wstring code_convert::utf8_2_unicode(const std::string& utf8string)  
{  
    int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);  
    if (widesize == 0)  
    {
		if (ERROR_NO_UNICODE_TRANSLATION == GetLastError())
			return std::wstring();
    }

    if (widesize == 0)
    {  
        return std::wstring();
    }  

    std::vector<wchar_t> resultstring(widesize);
    int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);  

    if (convresult != widesize)
    {  
        return std::wstring();
    }

    return std::wstring(&resultstring[0]);  
}  

std::string code_convert::unicode_2_gbk(const std::wstring& wstrcode)  
{  
    int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);  
    if (0 == asciisize)  
    {
		if (ERROR_NO_UNICODE_TRANSLATION == GetLastError())
			return std::string();
    }  

    if (asciisize == 0)  
    {  
        return std::string();
    }  

    std::vector<char> resultstring(asciisize);  
    int convresult =::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);  

    if (convresult != asciisize)  
    {  
        return std::string();
    }  

    return std::string(&resultstring[0]);  
}  

std::string code_convert::utf8_2_gbk(const std::string& strUtf8Code)  
{  
    std::string strRet;
    std::wstring wstr = utf8_2_unicode(strUtf8Code);
    return unicode_2_gbk(wstr);
}

std::wstring code_convert::gbk_2_unicode(const std::string& strascii)  
{  
    int widesize = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, NULL, 0);  
    if (0 == widesize)  
    {
		if (ERROR_NO_UNICODE_TRANSLATION == GetLastError())
			return std::wstring();
    }  
    if (widesize == 0)  
    {  
        return std::wstring();
    }  
    std::vector<wchar_t> resultstring(widesize);  
    int convresult = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, &resultstring[0], widesize);  

    if (convresult != widesize)  
    {  
        return std::wstring();
    }  

    return std::wstring(&resultstring[0]);  
}

std::string code_convert::unicode_2_utf8(const std::wstring& widestring)  
{  
    int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);
    if (utf8size == 0)
    {
        return std::string();
    }

    std::vector<char> resultstring(utf8size);

    int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);

    if (convresult != utf8size)
    {
        return std::string();
    }

    return std::string(&resultstring[0]);  
}

std::string code_convert::gbk_2_utf8(const std::string& strAsciiCode)  
{
    std::string strRet;
    std::wstring wstr = gbk_2_unicode(strAsciiCode);
    strRet = unicode_2_utf8(wstr);
    return std::move(strRet);
}  

bool code_convert::is_gbk(const char* gb, int length)
{
    for(int i=0; i<length; ++i){
        if(gb[i]<0)
        {
            return true;
        }
    }
    return false;
}

/*

//UTF-8 ������������6���ֽ�

//1�ֽ� 0xxxxxxx
//2�ֽ� 110xxxxx 10xxxxxx
//3�ֽ� 1110xxxx 10xxxxxx 10xxxxxx
//4�ֽ� 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
//5�ֽ� 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//6�ֽ� 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx


// ����ֵ˵��: 
// 0 -> �����ַ�������UTF-8�������
// -1 -> ��⵽�Ƿ���UTF-8�������ֽ�
// -2 -> ��⵽�Ƿ���UTF-8�ֽڱ���ĺ����ֽ�.

int IsTextUTF8(const char* pszSrc)
{
	const unsigned char* puszSrc = (const unsigned char*)pszSrc; // һ��Ҫ�޷��ŵ�,�з��ŵıȽϾͲ���ȷ��.
	// ������û��BOM��ʾ EF BB BF
	if( puszSrc[0] != 0 && puszSrc[0] == 0xEF && 
		puszSrc[1] != 0 && puszSrc[1] == 0xBB &&
		puszSrc[2] != 0 && puszSrc[2] == 0xBF)
	{
		return 0;
	}

	// ���û�� BOM��ʶ
	bool bIsNextByte = false;
	int nBytes = 0; // ��¼һ���ַ���UTF8�����Ѿ�ռ���˼����ֽ�.
	const unsigned char* pCur = (const unsigned char*)pszSrc; // ָ���α����޷����ַ���. ��Ϊ��λΪ1, ����� char ��, ���Ϊ����,�����ڱ��ʱ��ıȽϲ���.
	
	while( pCur[0] != 0 )
	{
		if(!bIsNextByte)
		{
			bIsNextByte = true;
			if ( (pCur[0] >> 7) == 0) { bIsNextByte = false; nBytes = 1; bIsNextByte = false; } // ���λΪ0, ANSI ���ݵ�.
			else if ((pCur[0] >> 5) == 0x06) { nBytes = 2; } // ����5λ���� 110 -> 2�ֽڱ����UTF8�ַ������ֽ�
			else if ((pCur[0] >> 4) == 0x0E) { nBytes = 3; } // ����4λ���� 1110 -> 3�ֽڱ����UTF8�ַ������ֽ�
			else if ((pCur[0] >> 3) == 0x1E) { nBytes = 4; } // ����3λ���� 11110 -> 4�ֽڱ����UTF8�ַ������ֽ�
			else if ((pCur[0] >> 2) == 0x3E) { nBytes = 5; } // ����2λ���� 111110 -> 5�ֽڱ����UTF8�ַ������ֽ�
			else if ((pCur[0] >> 1) == 0x7E) { nBytes = 6; } // ����1λ���� 1111110 -> 6�ֽڱ����UTF8�ַ������ֽ�
			else
			{
				nBytes = -1; // �Ƿ���UTF8�ַ���������ֽ�
				break;
			}
		}
		else
		{
			if ((pCur[0] >> 6) == 0x02) // ����,�����ֽڱ����� 10xxx ��ͷ
			{
				nBytes--;
				if (nBytes == 1) bIsNextByte = false; // �� nBytes = 1ʱ, ˵����һ���ֽ�Ӧ�������ֽ�.
			}
			else
			{
				nBytes = -2;
				break;
			}
		}

		// ����һ���ַ�
		pCur++;
	}

	if( nBytes == 1) return 0;
	else return nBytes;
}
*/

bool code_convert::is_utf8(const char* str, int length)
{
    int i;
    long nBytes=0;//UFT8����1-6���ֽڱ���,ASCII��һ���ֽ�
    unsigned char chr;
    bool bAllAscii=true; //���ȫ������ASCII, ˵������UTF-8
    for(i=0;i<length;i++)
    {
        chr= *(str+i);
        if( (chr&0x80) != 0 ) // �ж��Ƿ�ASCII����,�������,˵���п�����UTF-8,ASCII��7λ����,����һ���ֽڴ�,���λ���Ϊ0,o0xxxxxxx
            bAllAscii= false;
        if(nBytes==0) //�������ASCII��,Ӧ���Ƕ��ֽڷ�,�����ֽ���
        {
            if(chr>=0x80)
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
            if( (chr&0xC0) != 0x80 )
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
        return false;
    }
    return true;
}


}}