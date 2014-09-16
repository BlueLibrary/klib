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

    sunday();

public:  
    // Դ��     [text_start, text_end)  
    // ģʽ��   [pat_start, pat_end)  
    // @return -1 û�ҵ��� else Ŀ���������ʼ����ƫ��  
    int search(const char *text_start, const char *text_end) ;

    // ��ÿһ��ƥ�����ƫ�ƴ���pos_vec  
    void search(const char *text_start, const char *text_end, std::vector<int> &pos_vec)  ;
    // ����ģʽ��  
    // [pat_start, pat_end) ����pat_end.  
    void set_pattern(const char* pat_start, const char* pat_end);

    // ����ƥ�����  
    // �����ı���Ϊ "aaaaaa", ģʽ��Ϊ "aaa"  
    // ���rule:MATCH_RULE_STEP_ONE_CHAR, ������ 4��ƥ��  
    // ���rule:MATCH_RULE_STEP_ONE_PATTERN, ������ 2��ƥ��  
    void setmatch_rule_(int rule) ;

private:  
    // ������ת��  
    void pre_compute(const char* pat_start, const char* pat_end);

private:  
    u_char          _jump_table[JUMP_TABLE_LEN];  
    bool            jump_table_inited_;  

    const char    *pat_start_;  
    const char    *pat_end_;  

    int           match_rule_;  
};


}}


#endif  