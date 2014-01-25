
#ifndef _XLIB_SORT_H_
#define _XLIB_SORT_H_

namespace klib{
namespace algo{


template<class T>
void Swap(T list[], int l, int r)
{
    T *temp = list + l;
    list[l] = list[r];
    list[r] = *temp;
}

//----------------------------------------------------------------------
// Summary:
//      ð������,ʱ�临�Ӷ� 0(N) - 0(N^2)
//----------------------------------------------------------------------
template<class T>
void BubbleSort(T list[], int l, int r)
{
    if(l<r)
    {
        for(i=l;i<r;j++)
        {
            if(list[i]>list[i+1])
            {
                Swap(list,i,i+1);
            }
        }
        BubbleSort(list,l,r-1);
    }
}


//******************************************************�������� O(lgN) - 0(N^2) ����������
/*
��Ҫ�����������A[0]����A[N-1]����������ѡȡһ�����ݣ�ͨ��ѡ�õ�һ�����ݣ���Ϊ�ؼ����ݣ�Ȼ�����б���С�������ŵ���ǰ�棬
���б�����������ŵ������棬������̳�Ϊһ�˿�������
ֵ��ע����ǣ�����������һ���ȶ��������㷨��Ҳ����˵�������ͬ��ֵ�����λ��Ҳ������㷨����ʱ�����䶯��
һ�˿���������㷨�ǣ�
1��������������I��J������ʼ��ʱ��I=0��J=N-1��
2���Ե�һ������Ԫ����Ϊ�ؼ����ݣ���ֵ��key���� key=A[0]��
3����J��ʼ��ǰ���������ɺ�ʼ��ǰ������J=J-1�����ҵ���һ��С��key��ֵA[J]������key������
4����I��ʼ�������������ǰ��ʼ���������I=I+1�����ҵ���һ������key��A[I]����key������
5���ظ���3��4��5����ֱ�� I=J�� (3,4�����ڳ�����û�ҵ�ʱ��j=j-1��i=i+1��ֱ���ҵ�Ϊֹ���ҵ���������ʱ��i�� jָ��λ�ò��䡣
���⵱i=j�����һ��������i+��j-��ɵ������ѭ����������
*/


//һ�λ����и����� ����������λ��
template<class T>
int Division(T list[],int l,int r)
{
    T temp;
    while(l<r)
    {
        while(l<r && list[r]>list[l])
            r--;
        Swap(list,l,r);
        while(l<r && list[l]<list[r])
            l++;
        Swap(list,l,r);
    }
    return l;
}

//��������
template<class T>
void QuickSort(T list[],int l,int r)
{
    if(l<r)
    {
        int i = Division(list,l,r);
        QuickSort(list,l,i-1);
        QuickSort(list,i+1,r);
    }
}


//***************************************************************ֱ��ѡ������ O(N^2) ����������
template<class T>
void SelSort(T list[],int l,int r)
{
    if(l<r)
    {
        int i,min = l;
        for(i=l+1;i<r;i++)
            if(list[i]<list[min])
                min = i;
        Swap(list,l,min);
        SelSort(list,l+1,r);
    }
}

//****************************************************************������ O(NlogN) ����������
template<class T>
void HeapAdjust(T list[],int i,int size) //������ 
{
    int l=2*i; //i�����ӽڵ���� 
    int r=2*i+1; //i���Һ��ӽڵ���� 
    int max=i; //��ʱ���� 
    if(i<=size/2) //���i����Ҷ�ڵ�ͽ��е��� 
    {
        if(l<=size&&list[l]>list[max])
        {
            max=l;
        } 
        if(r<=size&&list[r]>list[max])
        {
            max=r;
        }
        if(max!=i)
        {
            Swap(list,i,max);
            HeapAdjust(list,max,size); //�������֮����maxΪ���ڵ���������Ƕ� 
        }
    } 
}

template<class T>
void BuildHeap(T list[],int size) //������ 
{
    int i;
    for(i=size/2;i>=1;i--) //��Ҷ�ڵ�������ֵΪsize/2 
    {
        HeapAdjust(list,i,size);
    }
}

//������ �ҳ�ǰtop�����ֵ
template<class T>
void HeapSort(T list[],int size,int top)
{
    int i;
    BuildHeap(list,size);//������ 
    for(i=size;i>=size-top;i--)
    {
        Swap(list,1,i);//�����Ѷ������һ��Ԫ�أ���ÿ�ν�ʣ��Ԫ���е�����߷ŵ������ 
        HeapAdjust(a,1,i-1);//���µ����Ѷ��ڵ��Ϊ�󶥶�
    }
}

//*************************************************ֱ�Ӳ������� ����ϴ�� O(N^2) 
template<class T>
void InsertSort(T list[],int size)
{
    int i,j;
    for(i=1;i<size;i++)//��һ����
    {
        for(j = i-1;j>=0 && list[i]<list[j];j--)//�Ƚ����ϵ��Ѿ��źõ��� 
            list[j+1] = list[j]; 
        list[j+1] = list[i]; //�ҵ�����λ�ò���
    }
}


//*************************************************ϣ������ O(N^3/2) - O(N^2) ���ȶ�����
/*
��ȡһ��С��n������d1��Ϊ��һ�����������ļ���ȫ����¼�ֳ�d1���顣���о���Ϊd1�ı����ļ�¼����ͬһ�����С�
���ڸ����ڽ���ֱ�Ӳ�������Ȼ��ȡ�ڶ�������d2<d1�ظ������ķ��������ֱ����ȡ������dt=1(dt<dt-l<��<d2<d1)��
�����м�¼����ͬһ���н���ֱ�Ӳ�������Ϊֹ��
*/
template<class T>
void ShellSort(T list[],int size)
{
    int i,j,step = size/2;
    while(step>=1)
    {
        for(i=0;i<size;i++)
        {
            for(j=i-step;j>= && list[i]<list[j];j-=step)
                list[j+step] = list[j];
            list[j+step] = list[i];
        }
        step=step/2;
    }
}

//**************************************************�鲢����

// ����������ϲ�����
template<class T>
void Merger(T list[], T temp[], int l, int mid, int r)
{
    int lEnd = mid-1;//��ָ��β
    int rStart = mid+1;//��ָ��ͷ
    int tempIdx = l;//��ʱ������±�

    //��ѭ�����������ѡ����Сֵ������ʱ����
    while((l<lEnd)&&(rStart<r))
    {
        if(list[l]<list[rStart])
            temp[tempIdx++] = list[l++];
        else
            temp[tempIdx++] = list[rStart++];
    }
    //�ж��������Ƿ����
    while(l<=lEnd)
        temp[tempIdx++] = list[l++];
    //�ж��������Ƿ����
    while(rStart<=right)
        temp[tempIdx++] = list[rStart++];
    int i;
    //�������źõ�����
    for(i=l;i<r;i++)
        list[i] = temp[i];
}

//�鲢����
template<class T>
void MergerSort(T list[], T temp[], int l, int r)
{
    if(l<r)
    {
        int mid = (l+r)/2;
        //�ݹ黮������������
        MergerSort(list,temp,l,mid);
        //�ݹ黮������������
        MergerSort(list,temp,mid+1,r);
        //����ϲ�����
        Merger(list,l,mid+1,r);
    }
}

/**
* �㷨: ����������
**/
template<typename T>
void t_swap( T& v1, T& v2 )
{
    T t = v1; v1 = v2; v2 = t;
}

/**
* �㷨: ��ת����
**/
template<typename T>
void t_reverse( T* v, size_t size )
{
    size_t s = 0, e = size-1;
    while( s < e && s < size && e > 0 )
        t_swap( v[s++], v[e--] );
}

/**
* �㷨: ��ҡ�㷨,��ָ��λ����ת����(���������ڶ���)
**/
template<typename T>
void t_exchange( T* v, size_t size, size_t n )
{
    t_reverse( v, n );
    t_reverse( v + n, size - n );
    t_reverse( v, size );
}

/**
* �㷨: �ϲ������������������
**/
template<typename T>
void t_merge( T& v, size_t size, size_t pos )
{
    size_t fir = 0, sec = pos;
    while ( fir < sec && sec < size )
    {
        while ( fir < sec && v[fir] <= v[sec] ) fir++;
        size_t maxMove = 0;
        while ( sec < size && v[fir] > v[sec] ) maxMove++, sec++;
        t_exchange( &v[fir], sec - fir, sec - fir - maxMove );
        fir += maxMove;

        display(array, sizeof(array)/sizeof(int));
    }
}

/**
* �㷨: �鲢����
**/
template<typename T>
void t_merge_sort( T* v, size_t size )
{
    if ( size <= 1 ) return;
    t_merge_sort( v, size/2 );
    t_merge_sort( v + size/2, size - size/2 );
    t_merge( v, size, size/2 );
}



}}


#endif //_ADVERTISEMENT_SORT_H_