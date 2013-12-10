
#include <string>
#include <map>
#include <list>



/* ����������� */
enum var_type
{
    type_int,
    type_double,
    type_string,
    type_array,
    type_list,
};

/* �ַ������� */
struct str_type
{
    char*   buff_;
    size_t  len_;
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
        str_type    str_val_;                   // �ַ���
    };
    std::map<variable, variable> array_;        // ��������
    std::list<variable>          list_;         // ��������

    variable() : 
    type_(type_int), 
        int_val_(0) 
    {}

    

    std::string to_string() const ;
    int         to_int()    const ;
    double      to_double() const ; 

    void clear();
    
    bool operator < (const variable& other)const ;
    variable& operator[] (size_t );
    variable& operator[] (const variable& );

    variable& operator = (int );

protected:


};

void variable::clear()
{
    if (type_ == type_string) 
    {
        delete str_val_.buff_;
        str_val_.buff_ = 0;
    }
}

bool variable::operator < (const variable& other)const 
{
    return true;
}

variable& variable::operator[] (size_t )
{

    return *this;
}

variable& variable::operator[] (const variable& var)
{
    clear();

    return array_[var];
}

variable& variable::operator = (int var)
{
    this->type_ = type_int;
    int_val_ = var;
    return *this;
}