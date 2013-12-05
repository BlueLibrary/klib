#include<iostream>
using namespace std;

/* �߶������߶θ��ǳ��� */
#define BORDER 100  // ���߶ζ˵����겻����100

struct Node         // �߶���
{
	int left;
	int right;
	int isCover;    // ����Ƿ񱻸���
}segTree[4*BORDER];  

/* �����߶��� ���ڵ㿪ʼ��������[lef,rig]���߶���*/
void construct(int index, int lef, int rig)
{
	segTree[index].left = lef;
	segTree[index].right = rig;
	if(rig - 1 == lef)                 // ����λ1�߶�
	{
		segTree[index].isCover = 0;
		return;
	}
	int mid = (lef+rig) >> 1;
	construct((index<<1)+1, lef, mid);
	construct((index<<1)+2, mid, rig); // ��mid+1���߶θ���[mid,mid+1]
	segTree[index].isCover = 0;
}

/* �����߶�[start,end]���߶���, ͬʱ��Ǹ��� */
void insert(int index, int start, int end)
{
	if(segTree[index].isCover == 1)	return; // ���Ѹ��ǣ�û��Ҫ�������²�

	if(segTree[index].left == start && segTree[index].right == end)
	{
		segTree[index].isCover = 1;
		return;
	}
	int mid = (segTree[index].left + segTree[index].right) >> 1;
	if(end <= mid)
	{
		insert((index<<1)+1, start, end);
	}else if(start >= mid)             // ��©=
	{
		insert((index<<1)+2, start, end);
	}else
	{
		insert((index<<1)+1, start, mid);
		insert((index<<1)+2, mid, end);  
		// ע������mid+1���߶θ��ǣ�����©[mid,mid+1]
	}
}

/* �����߶θ��ǳ��� */
int Count(int index)
{
	if(segTree[index].isCover == 1)
	{
		return segTree[index].right - segTree[index].left;
	}else if(segTree[index].right - segTree[index].left == 1)
	{
		return 0;
	}
	return Count((index<<1)+1) + Count((index<<1)+2);
}

/* �����߶� answer: 71 */
int segment[10][2] = {
	5, 8,	10, 45,	   0, 7,
	2, 3,	 3, 9,	  13, 26,
   15, 38,  50, 67,	   39, 42, 
   70, 80 
};

void main()
{
	construct(0,0,100);           // ����[0,100]�߶���
	for(int i = 0; i < 10; ++i)   // ��������߶�
	{
		insert(0,segment[i][0],segment[i][1]);
	}
	printf("the cover length is %d\n", Count(0));
}