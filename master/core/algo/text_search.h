#pragma once

namespace klib{
namespace algo{


#define  MIN_VISIABLE_TEXT_LEN          (5)
class text_search
{
public:
    template<typename CharT>
    struct stFindItem {
        CharT* pSzPos;
        UINT   uLen;
    };

    /*
    * @brief �����ɼ��ַ�������items�з��ؿɼ����ַ���
    * pBuff       :       �ֽ���������
    * uBuffLen    :       �������ĳ���
    * CharT       :       ��ʾҪ���ҿɼ��ַ������ͣ�char��wchar_t�������ͣ�
    */
    template<typename CharT, UINT uMinVisiableTextLen>
    bool search(void* pBuff, UINT uBuffLen, std::vector<stFindItem<CharT>>& items)
    {
        UINT uVisiableCount = 0;
        stFindItem<CharT> item;
        CharT* pszFindPos = NULL;

        CharT* pszEndPos = (CharT*) ((char*)pBuff + uBuffLen);
        CharT* pos = (CharT*) pBuff;
        while (pos + 1 <= pszEndPos) 
        {
            if (IsPrint<CharT>(*pos)) 
            {
                if (uVisiableCount == 0) 
                {
                    pszFindPos = pos;
                }
                ++ uVisiableCount;
            }
            else 
            {
                if (uVisiableCount >= uMinVisiableTextLen) 
                {
                    item.pSzPos = pszFindPos;
                    item.uLen   = uVisiableCount;
                
                    items.push_back(item);
                }
                uVisiableCount = 0;
            }
            
            ++ pos;
        }

        if (uVisiableCount >= uMinVisiableTextLen) 
        {
            item.pSzPos = pszFindPos;
            item.uLen   = uVisiableCount;
                
            items.push_back(item);
        }

        return !items.empty();
    }

    /*
    * @brief �ж��Ƿ��ǿɼ��ַ�
    */
    template<typename CharT>
    BOOL IsPrint(CharT ch) 
    {
        if (sizeof(CharT) == sizeof(wchar_t)) 
        {
            return my_iswprint(ch);
        }
        
        return _ismbcprint(ch);
    }

    /*
    * @brief �ж��Ƿ��ǿɼ��ַ���unicode�汾
    *      �ǿɼ��ַ�����TRUE���ǿɼ��ַ�����FALSE
    */
    static BOOL my_iswprint(wchar_t ch) 
    {
        return ch >= 0x20 && ch <= 0x7E;
    }

    /*
    * @brief �������ֶ���������ֵ���ֵ�λ��(�ɹ�����TRUE��ʧ�ܷ���FALSE)
    * index : Ϊ���ֶ����Ƶ���λ��
    */
    template<typename CharT>
    BOOL search(const char* pszBuff, int iLen, int& index)
    {
        CharT ch;
        for (int ipos=0; ipos<iLen; ++ipos) 
        {
            ch = (CharT) pszBuff[ipos];

            // ��ASCII��
            if (IsPrint<CharT>(ch) || ch == 0x0A || ch == 0x0D) 
            {
                continue;
            }
            else 
            {
                index = ipos;
                return TRUE;
            }
        }

        return FALSE;
    }

    /*
    * @brief �ж��Ƿ��Ǳ�����
    */
    template<typename CharT>
    static BOOL IsPunctuation(CharT ch)
    {
        if (ch >= 33 && ch <= 47) 
        {
            return TRUE;
        }

        if (ch >= 58 && ch <= 64) 
        {
            return TRUE;
        }

        if (ch >= 91 && ch <= 96) 
        {
            return TRUE;
        }

        if (ch >= 123 && ch <= 126) 
        {
            return TRUE;
        }

        return FALSE;
    }
};



}}

