
#ifndef _kl_variable_h_
#define _kl_variable_h_

#include <string>
#include <map>
#include <list>



/* ����������� */
enum var_type
{
    type_int,               // ������
    type_double,            // ������
    type_string,            // �ַ���
    type_array,             // ����
    type_function,          // ����
    type_object,            // ����
    type_refrence,          // ���ã������Ҫʵ��
};

/* �������� */
class variable
{
public:
    var_type type_;                             // ��������
    union
    {
        double      double_val_;                // ʵ��
        int         int_val_;                   // ����
    };
    typedef std::map<variable, variable> array_type;
    array_type      array_;                     // ��������
    std::string     str_val_;                   // �ַ���

public:
    variable() : 
    type_(type_int), 
        int_val_(0) 
    {}

    variable(const variable& );
    variable(const char* );
    variable(double );
    variable(int);

    variable(variable&& other);
    ~variable();

    var_type    get_type() const;
    std::string to_string() const ;
    int         to_int()    const ;
    double      to_double() const ; 

    void set_type(var_type vtype) ;

    void clear();
    
    bool operator < (const variable& other)const ;
    variable& operator[] (size_t );
    variable& operator[] (const variable& );
    variable& operator[] (const char* str );

    variable& operator = (int );
    variable& operator = (const double );
    variable& operator = (const char* str);
    variable& operator = (const variable& other);
    variable& operator = (variable&& other);
    
};


#endif