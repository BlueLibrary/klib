#ifndef _klib_must_have_base_h
#define _klib_must_have_base_h

/*
ǰ��������ָ������������Լ���������������������ǰ�������ǵ����ߵ����Ρ�
����������û������ǰ���������򱻵����������κ����鶼����ȫ����ġ�

���������ں���ִ�����ʱ����Ϊ�档ȷ�����������������Ǳ������ߵ����Ρ�
ǰ�������ͺ����������Ա�Ӧ�õ���ĳ�Ա������Ҳ���Ա��õ����ɺ������ϡ�

��̬���Լ���̬���Ե�ʹ��
*/
template<typename D, typename B>
struct must_have_base
{
    ~must_have_base(void)
    {
        void (*p)(D *, B *) = constraints;
    }
private:
    static void constraints(IN D *pd, IN B *pb)
    {
        pb = pd;
    }
};




#endif