
#ifndef _kl_symbols_h_
#define _kl_symbols_h_

#include "variable.h"
#include "../core/core/small_string.h"

using namespace klib::mem;

#include <map>
#include <string>

enum symbol_type
{
    symbol_operator,
    symbol_function,
    symbol_,
};

struct symbol_info
{
    symbol_type type_;

};

typedef std::map<std::string, symbol_info> symbols_info_table;

//----------------------------------------------------------------------
// ���Ź�����
class symbols_mgr 
{
public:
    bool add_symbol(const std::string& name, symbol_info& info);
    bool set_symbol(const std::string& name, symbol_info& info);
    bool get_symbol(const std::string& name, symbol_info& info);

protected:
    symbols_info_table symbol_table_;

};

//----------------------------------------------------------------------
// �﷨���Ľ��
class lex_node
{
public:

protected:
    std::list<lex_node*> siblings_;     // �ٽӽ��
    std::list<lex_node*> childs_;       // �ӽ��
    // ���ͣ���ʾ�Ǻ�����= ���ţ����߼Ӽ����㣬�������

};

#define max_function_name_len   (40)
class function_info
{
public:

    
protected:
    small_string<max_function_name_len> name_;
    lex_node* node_;
};

//----------------------------------------------------------------------
// ��������
class function_mgr
{
public:
    typedef std::map<std::string, function_info> function_info_table;



protected:
    function_info_table function_info_tbl_;
};


//----------------------------------------------------------------------
// �������
class plugin_mgr
{
public:


private:

};



#endif //   _kl_symbols_h_