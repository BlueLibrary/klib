#ifndef _kl_lex_h_
#define _kl_lex_h_

#include <vector>
#include <functional>

#include "token.h"

// �����ص���������
typedef std::function<bool (char)> parser_callback;

// ����������Ϣ
struct parser_func_info
{
    token_type      type_;
    size_t          len;
    std::function<size_t()> func;
};
typedef std::vector<parser_func_info> parser_func_list;

/* �ʷ������� */
class lex_parser
{
public:
    typedef char char_type;

public:
    lex_parser(const char_type* script, size_t len) : 
            script_(script), 
            len_(len), 
            cur_ptr_(script), 
            line_no_(1)
    {}


public:
    bool init_keyword_list();
    bool get_parser_funcs(parser_func_list& thelist);       // ��ȡ���������б�

    const char_type* get_pos() const {  return cur_ptr_;}  
    size_t get_line_no() const { return line_no_; }

    size_t parser_comment();            // ����ע��
    size_t parser_int();                // �����Ƿ��Ƿ���
    size_t parser_name();               // �����Ƿ�������
    size_t parser_keyword();            // �ж��Ƿ��Ǻ���
    size_t parser_double();             // �����Ƿ��Ǹ�����
    size_t parser_class();              // �ж��Ƿ���һ����
    size_t parser_logic();              // �����߼������
    size_t parser_operator();           // ���������
    size_t parser_assign();             // ������ֵ��
    size_t parser_line();               // ����һ�н���
    size_t parser_statement();          // ����������

    size_t parser_(const char_type* pos, const parser_callback& call);
    size_t parser_(const char_type* pos, char_type ch);
    size_t parser_(const char_type* pos, char_type* str, size_t len);
    size_t parser_until(const char_type* pos, char_type ch);
    size_t parser_until(const char_type* pos, char_type* str, size_t len);
    size_t parser_until(const char_type* pos, const parser_callback& call);

    size_t skip_space();
    size_t skip(size_t len);

    std::string read(size_t len);

    // ת������
    double to_double(size_t len);
    int    to_int(size_t len);

public:
    static bool is_digital(char_type ch);
    static bool is_letter(char_type ch);
    static bool is_digi_letter(char_type ch);
    bool is_space(char_type ch);

protected:
    const char_type* cur_ptr_;
    const char_type* script_;
    size_t   len_;
    size_t   line_no_;
    keyword_list_type keyword_list_;
};



#endif // _kl_lex_h_