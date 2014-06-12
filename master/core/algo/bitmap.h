#ifndef _klib_bitmap_h
#define _klib_bitmap_h



namespace klib {
namespace algo{


class bitmap
{
public:
    bitmap();
    ~bitmap();

    //���ܣ���ʼ��bitmap 
    //������ size��bitmap�Ĵ�С����bitλ�ĸ��� 
    //      start����ʼֵ 
    //����ֵ��0��ʾʧ�ܣ�1��ʾ�ɹ� 
    bool init(int size, int start);

    //���ܣ���ֵindex�Ķ�Ӧλ��Ϊ1 
    //index:Ҫ���ֵ 
    //����ֵ��0��ʾʧ�ܣ�1��ʾ�ɹ� 
    bool set(int index);

    //���ܣ�ȡbitmap��iλ��ֵ 
    //i����ȡλ 
    //����ֵ��-1��ʾʧ�ܣ����򷵻ض�Ӧλ��ֵ 
    int get(int i) ;

    //���ܣ�����indexλ��Ӧ��ֵ   
    int data(int index);

    //�ͷ��ڴ� 
    void clear();


protected:
    char* bitmap_ ;  
    int size_ ;  
    int start_; 

};









}}



#endif
