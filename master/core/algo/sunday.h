/* 
* @desc    : Sunday String pattern matching algorithm (also support binary buf pattern match) 
* @author  : nemozhang 
* 
*/  
#ifndef _Klib_Sunday_H_  
#define _Klib_Sunday_H_  

#include <stdio.h>  
#include <vector>  

namespace klib{
namespace algo{


#ifndef u_char  
#define u_char unsigned char  
#endif  

class sunday  
{  
public:  
    enum
    {
        JUMP_TABLE_LEN                  = 256    // ��ת���С  
    };

    enum 
    {
        MATCH_RULE_STEP_ONE_CHAR        = 0,    // ƥ����ʱ, ����һ���ַ����ȼ���ƥ��  
        MATCH_RULE_STEP_ONE_PATTEN      = 1,    // ƥ����ʱ, ����һ��ģʽ�����ȼ���ƥ��  
    };  

public:  

    sunday():  
      _jump_table_inited(false),   
          _pat_start(0),  
          _pat_end(0),  
          _match_rule(MATCH_RULE_STEP_ONE_CHAR)  
      {}    

public:  
    // Դ��     [text_start, text_end)  
    // ģʽ��   [pat_start, pat_end)  
    // @return -1 û�ҵ��� else Ŀ���������ʼ����ƫ��  
    int search(const char *text_start, const char *text_end)  
    {     
        if (text_start >= text_end)  
        {     
            return -1;   
        }     

        if (!_jump_table_inited)  
        {     
            return -1;   
        }     

        int text_len = text_end - text_start;  
        int pat_len = _pat_end - _pat_start;  

        for (int i=0; i<text_len-pat_len+1; )  
        {     
            bool finded = true;  
            // �Ӻ���ǰƥ��  
            for (int j=0; j<pat_len; ++j)  
            {     
                // ƥ�䲻��, ��  
                if (text_start[i+pat_len-j-1] != _pat_start[pat_len-j-1])  
                {     
                    //printf("i:%d, j:%d\n", i, j);  
                    //printf("text:%d [%c], pat:%d [%c] \n", i+pat_len-j-1, text_start[i+pat_len-j-1],  pat_len-j-1, _pat_start[pat_len-j-1]);  
                    //printf("i:%d [%c], j:%d [%c] \n", i, text_start[i],  j, _pat_start[j]);  
                    u_char next_c = (u_char)text_start[i + pat_len];  
                    //printf("next c:%d, [%c], jmp:%d\n", i+pat_len, next_c, _jump_table[next_c]);  

                    i += _jump_table[next_c];  

                    finded = false;  
                    break;  
                }     
            }     

            if (finded)  
            {     
                // ƥ������   
                return i;  
            }  
        }  

        return -1;  
    }  

    // ��ÿһ��ƥ�����ƫ�ƴ���pos_vec  
    void search(const char *text_start, const char *text_end, std::vector<int> &pos_vec)  
    {  
        int pos = 0;  

        const char *text_start_new = (const char*)text_start;  

        int pat_len = _pat_end - _pat_start;  

        while(pos != -1)  
        {  
            pos = search(text_start_new, text_end);  

            if (pos != -1)  
            {  
                pos_vec.push_back(pos + text_start_new - text_start);  

                if (MATCH_RULE_STEP_ONE_CHAR == _match_rule)  
                {  
                    text_start_new += (1 + pos);  
                }  
                else  
                {  
                    text_start_new += (pat_len + pos);  
                }  

            }  
            else  
            {  
                break;  
            }  
        }  
    }  

    // ����ģʽ��  
    // [pat_start, pat_end) ����pat_end.  
    void set_pattern(const char* pat_start, const char* pat_end)  
    {  
        _pat_start = pat_start;  
        _pat_end = pat_end;  
        pre_compute(pat_start, pat_end);  
    }  

    // ����ƥ�����  
    // �����ı���Ϊ "aaaaaa", ģʽ��Ϊ "aaa"  
    // ���rule:MATCH_RULE_STEP_ONE_CHAR, ������ 4��ƥ��  
    // ���rule:MATCH_RULE_STEP_ONE_PATTERN, ������ 2��ƥ��  
    void set_match_rule(int rule)  
    {  
        _match_rule = rule;  
    }  

private:  
    // ������ת��  
    void pre_compute(const char* pat_start, const char* pat_end)  
    {  
        if (pat_start >= pat_end)  
        {  
            return;  
        }  

        int pat_len = pat_end - pat_start ;  

        // ��ʼ��  
        for (int i=0; i<JUMP_TABLE_LEN; ++i)  
        {  
            _jump_table[i] = pat_len + 1; // pat����+1  
        }  

        const char* p = pat_start;  
        for (; p!=pat_end; ++p)  
        {  
            _jump_table[(u_char)(*p)] = pat_end - p;  
        }  

        _jump_table_inited = true;  
    }  

private:  
    u_char  _jump_table[JUMP_TABLE_LEN];  
    bool    _jump_table_inited;  

    const char    *_pat_start;  
    const char    *_pat_end;  

    int             _match_rule;  
};


}}


#endif  