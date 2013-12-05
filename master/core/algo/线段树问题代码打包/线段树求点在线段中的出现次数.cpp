#include<iostream>
using namespace std;

/* �߶����������֪�߶��еĳ��ִ��� */
#define BORDER 100  // ���߶ζ˵����겻����100

struct Node         // �߶���
{
	int left;
	int right;
	int counter;
}segTree[4*BORDER];  

/* �����߶��� ���ڵ㿪ʼ��������[lef,rig]���߶���*/
void construct(int index, int lef, int rig)
{
	segTree[index].left = lef;
	segTree[index].right = rig;
	if(lef == rig)   // Ҷ�ڵ�
	{
		segTree[index].counter = 0;
		return;
	}
	int mid = (lef+rig) >> 1;
	construct((index<<1)+1, lef, mid);
	construct((index<<1)+2, mid+1, rig);
	segTree[index].counter = 0;
}

/* �����߶�[start,end]���߶���, ͬʱ����������� */
void insert(int index, int start, int end)
{
	if(segTree[index].left == start && segTree[index].right == end)
	{
		++segTree[index].counter;
		return;
	}
	int mid = (segTree[index].left + segTree[index].right) >> 1;
	if(end <= mid)
	{
		insert((index<<1)+1, start, end);
	}else if(start > mid)
	{
		insert((index<<1)+2, start, end);
	}else
	{
		insert((index<<1)+1, start, mid);
		insert((index<<1)+2, mid+1, end);
	}
}

/* ��ѯ��x�ĳ��ִ��� 
 * �Ӹ��ڵ㿪ʼ��[x,x]Ҷ�ӵ�����·�������е������ӷ�Ϊx���ִ���
 */
int query(int index, int x)
{
	if(segTree[index].left == segTree[index].right) // �ߵ�Ҷ�ӣ�����
	{
		return segTree[index].counter;
	}
	int mid = (segTree[index].left+segTree[index].right) >> 1;
	if(x <= mid)
	{
		return segTree[index].counter + query((index<<1)+1,x);
	}
	return segTree[index].counter + query((index<<1)+2,x);
}

/* �����߶� */
int segment[10][2] = {
	5, 8,	10, 45,	   0, 7,
	2, 3,	 3, 9,	  13, 26,
   15, 38,  50, 67,	   39, 42, 
   70, 80 
};
/* ���Ե� ��answer: 1,2,2,3,1,3,2,1,0,0 */
int testPoint[10] = {
	1, 2, 4, 6, 9,
  15, 13, 44, 48, 90
};

void main()
{
	construct(0,0,100);           // ����[0,100]�߶���
	for(int i = 0; i < 10; ++i)   // ��������߶�
	{
		insert(0,segment[i][0],segment[i][1]);
	}
	for(int i = 0; i < 10; ++i)   // ��ѯ����ִ���
	{
		printf("frequent of point %d is: %d\n", 
			   testPoint[i], query(0,testPoint[i]));
	}
}