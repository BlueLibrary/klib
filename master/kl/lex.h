#ifndef _kl_lex_h_
#define _kl_lex_h_

#include <vector>
#include <functional>

#include "token.h"


/* parser result */
class parser_result
{
public:
    parser_result() : len_(0), type_(token_none) {}
    parser_result(size_t len) : len_(len), type_(token_none)  {}
    parser_result(size_t len, token_type type) : len_(len), type_(type) {}

    size_t length() const { return len_; }
    token_type type() const { return type_; }

    size_t          len_;       // parsed length 
    token_type      type_;      // token type
    bool            error_;     // is error ?
};

// call back function type
typedef std::function<bool (char)> parser_callback;

// parser worker function define
typedef std::function<parser_result()> parser_worker;

// parser function information
struct parser_func_info
{
    parser_func_info() {}
    parser_func_info(token_type type, parser_worker f) : 
            type_(type), func_(f) {}

    token_type      type_;
    parser_worker   func_;
};
typedef std::vector<parser_func_info> parser_func_list;


/* lex parser */
class lex_parser
{
public:
    typedef char char_type;

public:
    lex_parser(const char_type* script, size_t len) : 
            script_buff_(script), 
            script_len_(len), 
            cur_ptr_(script), 
            line_no_(1)
    {
        end_ptr_ = script_buff_ + len;
        init_keyword_list(); 
    }


public:
    bool init_keyword_list();
    bool get_parser_funcs(parser_func_list& thelist);       // ��ȡ���������б�
    bool parser_script(token_list_type& thelist, const char* src_script = NULL, size_t len = 0);           // parser script strings
    bool is_finished() const ;

    const char_type* get_pos() const {  return cur_ptr_;}  
    size_t get_line_no() const { return line_no_; }

    parser_result parser_comment();            // ����ע��
    parser_result parser_int();                // �����Ƿ��Ƿ���
    parser_result parser_name();               // �����Ƿ�������
    parser_result parser_string();             // �����Ƿ����ַ���
    parser_result parser_keyword();            // �ж��Ƿ��Ǻ���
    parser_result parser_double();             // �����Ƿ��Ǹ�����
    parser_result parser_logic();              // �����߼������
    parser_result parser_operator();           // ���������
    parser_result parser_bracket();            // �������� () {}[]
    parser_result parser_assign();             // ������ֵ��
    parser_result parser_line();               // ����һ�н���
    parser_result parser_statement();          // ����������

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
    static bool is_underline(char_type ch);
    static bool is_digi_letter_underline(char_type ch);
    bool is_space(char_type ch);

protected:
    const char_type* cur_ptr_;
    const char_type* end_ptr_;
    const char_type* script_buff_;
    size_t   script_len_;
    size_t   line_no_;
    keyword_list_type keyword_list_;
};



#endif // _kl_lex_h_