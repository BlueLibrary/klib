#ifndef _klib_str_enc_h_
#define _klib_str_enc_h_

namespace klib{
namespace encrypt{


typedef unsigned char* stream_ptr;


void enc_str(unsigned char* lpszText, 
    unsigned char *lpszReturn, 
    unsigned char* lpszKey)
{
    int nTextLen = 0;
    char *cPos = NULL;
    char *pDest = NULL;
    if(lpszReturn)	// ����
    {
        nTextLen = ::strlen(lpszText);
        pDest = new char[nTextLen + 3];	// ==\0
    }
    else	// ����
    {
        // �����Զ�����ֹ���
        cPos = (stream_ptr)lpszText;
        while(true)	// ��������Կ��������������������Զ�����ֹ��ǣ������һֱ������ȥ
        {
            if(*cPos == '=')
                if(cPos[1] == '=')
                    if(cPos[2] == '\0')
                        break;
            cPos++;
        }

        if(!cPos)	// û���ҵ�������ǣ�Ҳ���Ǽ���
            return;

        nTextLen = cPos - lpszText;
        pDest = (LPTSTR)lpszText;
    }

    int nKeyLen = ::strlen(lpszKey);
    int i = 0;
    int k = 0;
    int t = nTextLen;
    int cn = 0;
    for(int a = 0; a < nKeyLen; a++)	// ����Կ����ֵ������
        cn += lpszKey[a];

    for(; i < nTextLen; i++)
    {
        if(lpszReturn)	// ����
        {
            pDest[i] = lpszText[i] + t;
            pDest[i] = pDest[i] ^ lpszKey[k];
            pDest[i] = pDest[i] ^ cn;
        }
        else	// ���ܣ�˳�������ʱ�෴
        {
            pDest[i] = lpszText[i] ^ cn;
            pDest[i] = pDest[i] ^ lpszKey[k];
            pDest[i] = pDest[i] - t;
        }
        k++;
        t--;
        if(k >= nKeyLen)
            k = 0;
        if(t <= 0)
            t = nTextLen;
    }

    if(!cPos)
    {
        memcpy(pDest + nTextLen, _T("==\0"), 3 * sizeof(unsigned char));
        *lpszReturn = pDest;
    }
    else 
    {
        memset(pDest  + nTextLen, _T('\0'), sizeof(unsigned char));
    }
}



}}



#endif // _klib_str_enc_h_

