#pragma once

#include <string>
using std::string;

// {27FD496C-2135-4930-BB1A-B7BDFCA8DF9F}
static const GUID IID_IPinying = 
{ 0x27fd496c, 0x2135, 0x4930, { 0xbb, 0x1a, 0xb7, 0xbd, 0xfc, 0xa8, 0xdf, 0x9f } };


struct IPinying
{
public:
  /*
    ���ã���һ�����ַ�ת����ƴ��
    chinese�������ַ���
    len:  �ַ�������
    strSpell: ת�����ƴ��ƴд
  */
  virtual bool GetPinyin(const char* chinese, int len, string& strSpell) = 0;

  /*
    ���ã�������ĵ�����ĸ
    chinese: ������ĸ
    len: ������ĸ�ĳ���
    strFirstLetters: ���ɵ�����ĸ
  */
	virtual bool GetFirstLetters(const char* chinese, int len, string& strFirstLetters) = 0;
};

