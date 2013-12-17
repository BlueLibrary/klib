#ifndef _kl_lex_h_
#define _kl_lex_h_

#include <functional>

typedef std::function<bool (char)> parser_callback;

/* �ʷ������� */
class lex_parser
{
public:
    typedef char char_type;

public:
    lex_parser(const char_type* script, size_t len) : 
            script_(script), 
            len_(len), 
            cur_ptr_(script) 
    {}


public:
    size_t parser_digital();            // �����Ƿ��Ƿ���
    size_t parser_name();               // �����Ƿ�������
    size_t parser_double();             // �����Ƿ��Ǹ�����
    size_t parser_function();           // �ж��Ƿ��Ǻ���
    size_t parser_class();              // �ж��Ƿ���һ����

    size_t parser_(const char_type* pos, const parser_callback& call);
    size_t parser_(const char_type* pos, char_type ch);
    size_t parser_(const char_type* pos, char_type* str, size_t len);

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
    static bool is_space(char_type ch);

protected:
    const char_type* cur_ptr_;
    const char_type* script_;
    size_t   len_;
};



#endif // _kl_lex_h_