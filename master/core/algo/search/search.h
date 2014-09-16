
#ifndef _Klib_SEARCH_H_
#define _Klib_SEARCH_H_

namespace klib {
namespace algo {



//***********************************˳������㷨 ���д�low-high������ ƽ�����ҳ��ȣ�ASL = (n+1)/2
template<class T>
int SeqSearch(T list[],int count,T key)
{
    int i;
    for(i=count ; list[i] == key ; i--);
    return i;
}

//***********************************�۰����
template<class T>
int BinSearch(T list[],int count,T key)
{
    int mid, low = 0;
    int high = count - 1;//��������������½�
    while (low <= high)
    {
        mid = (low+high)/2;
        if(e == list[mid]) {
            return mid;
        }
        else if(e < list[mid]) {
            high = mid-1;
        }
        else {
            low = mid+1;
        }
    }

    return -1;
}



//***********************************�ֿ����

//������
template<class T>
class TIdx
{
    T key;//�������ֵ����Сֵ��
    int index;//��Ӧ�����Ա�������
    int size;//���С
};

template<class T>
int IdxSearch(T list[], int count,T key, TIdx<T> idx[],int idxcount)
{
    //���ַ����ҿ�
    int mid, low = 0;
    int high = idxcount - 1;
    while(low<=high)
    {
        mid = (low+high)/2;
        if(e == list[mid]) {
            return mid;
        }
        else if(e<list[mid]) {
            high = mid-1;
        }
        else {
            low = mid+1;
        }
    }
    //�ڿ���˳�����
    if(low<idxcount)
    {
        for(i=idx[low].index;i<idx[low].index+idx[low].size-1 && i<count;i++)
        {
            if(list[i] == key) return i;
        }
    }
    return -1;
}


//***********************************���������� ʱ�临�Ӷ�O��lgN�� ���ΪO(N)
//���ڵ�
template<class T>
class TNode
{
    T key;//�ڵ�ؼ���
    TNode<T> *lc,*rc;//���ҽڵ�
};

//head Ϊ���� pΪ��ǰ���ڵ� qΪ��ǰ�ڵ� key Ϊ��Ҫ����
template<class T>
bool TreeSearch(TNode<T> *head,TNode<T> **p,TNode<T> **q,T key)
{
    *q=head;
    while(*q) //�Ӹ��ڵ���
    {
        if(key>(*q)->key) //key����������
        {
            *p = *q;
            *q = (*q)->rc;
        }
        else if(key<(*q)->key)//keyС��������
        {
            *p = *q;
            *q = (*q)->lc;
        }
        else
            return true;
    }
    return false;
}

//����ڵ�
template<class T>
bool InsertNode(TNode<T> *head,T key)
{
    TNode<T> *p = *head,*q,*s;
    if(!TreeSearch(*head,&p,&q,key))
    {
        s = new TNode<T>;
        s->key = key;
        s->lc = NULL;
        s->rc = NULL;
        if(!p) //����ǿ���
            head = s;
        else
        {
            if(key>p->key) //
                p->rc = s;
            else
                p->lc = s;
        }
        return true;
    }
    return false;
}



//*********************************************************************ɢ��ֵHash����
/*
��ʵ���õ�����ϣ���ַ��С����֡���
��һ�֣���ֱ�Ӷ�ַ������
��������⣬key=Value+C�� �����C"�ǳ�����Value+C��ʵ����һ���򵥵Ĺ�ϣ������
�ڶ��֣�������ȡ�෨����
��������⣬ key=value%C;����ͬ�ϡ�
�����֣������ַ���������
����������˼��������һ��value1=112233��value2=112633��value3=119033��
��������������Ƿ������м��������Ƚϲ��������������䡣��ô����ȡkey��ֵ�Ϳ�����
key1=22,key2=26,key3=90�� 
�����֣���ƽ��ȡ�з������˴����ԣ�����ʶ�⡣
�����֣����۵�������
����������˼,����value=135790��Ҫ��key��2λ����ɢ��ֵ����ô���ǽ�value��Ϊ13+57+90=160��
Ȼ��ȥ����λ��1��,��ʱkey=60����������������ǵĹ�ϣ��ϵ����������Ŀ�ľ���key��ÿһλvalue����
�أ���������ɢ�е�ַ�������ܷ�ɢ��Ŀ�ء�
*/
template<class T>
void InsertHash(T list[],int size,T key)
{
    int hashAddr = key%13;
    while(list[hashAddr]!=0)
    {
        hashAddr = (++hashAddr)%size;
    }
    list[hashAddr]=key;
}

template<class T>
int HashSearch(T list[], int size,int key)
{
    int hashAddr = key % 13;
    while(list[hashAddr] !=0 && hash[Addr] != key)
    {
        hashAddr = (++hashAddr) % size;
    }

    if(list[hashAddr]==0) return -1;
    return hashAddr;
}


}}


#endif 

