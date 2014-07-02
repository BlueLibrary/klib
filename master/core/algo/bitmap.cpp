
#include "bitmap.h"
#include <string>

#ifndef NULL
#define  NULL 0
#endif

namespace klib{
namespace algo{


bitmap::bitmap()
{
    bitmap_ = NULL;

    contain_size_ = 0;
    bucket_size_  = 0;
    start_ = 0;
}

bitmap::~bitmap()
{
    clear();
}

bool bitmap::init(int size, int start)
{
    contain_size_ = size;
    bucket_size_ = size / 8 + 1;
    start_ = start;
    bitmap_ = new char[bucket_size_];
    if(bitmap_ == NULL)
    {
        return false;
    }
    memset(bitmap_, 0x0, bucket_size_);
    return true;
}

bool bitmap::set(int index)
{
    if (index < 0 || index >= contain_size_)
        return false;
    
    int quo = (index - start_) / 8 ;  //ȷ�����ڵ��ֽ�
    int remainder = (index - start_) % 8;  //�ֽ��ڵ�ƫ��  
    unsigned char x = (0x1 << remainder);    
    if( quo > bucket_size_)  
        return false;

    bitmap_[quo] |= x;   //�����ֽ��ڵ��ض�λ��Ϊ1  
    return true;   
}

int bitmap::get(int i)
{
    if (i<start_ || i>contain_size_) 
        return -1;
    
    int quo = (i) / 8 ;
    int remainder = (i) % 8;
    unsigned char x = (0x1 << remainder);
    unsigned char res;
    if( quo > bucket_size_)
        return -1;
    res = bitmap_[quo] & x;
    return res > 0 ? 1 : 0;
}

int bitmap::data(int index)
{
    return (index + start_);
}

void bitmap::clear()
{
    bucket_size_ = 0;
    contain_size_ = 0;
    start_ = 0;
    delete[] bitmap_;
    bitmap_ = nullptr;
}


}}
