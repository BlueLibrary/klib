#include <iostream>
using namespace std;

/* �߶������������ֵ */
#define MAXN 100   /* ���Ԫ�ظ��� */
#define MAX(a,b) ((a) > (b) ? (a) : (b))

struct Node
{
	int left;
	int right;
	int maxV;
}segTree[4*MAXN];  /* �߶��� */

int arr[] = {56,23,10,78,9,
			100,26,52,30,18};   /* �������� */ 

/* ���ݲ������齨���߶��� ���ڵ㿪ʼ����[left,right]�������ֵ*/
void construct(int index, int left, int right)
{
	segTree[index].left = left;
	segTree[index].right = right;
	if(left == right)  // Ҷ�ӣ���Ϊ���������ֵ���������
	{
		segTree[index].maxV = arr[left];
		return;
	}
	int mid = (left + right) >> 1;
	construct((index<<1)+1, left, mid);
	construct((index<<1)+2, mid+1, right);
	segTree[index].maxV = MAX(segTree[(index<<1)+1].maxV,segTree[(index<<1)+2].maxV);
}

/* �߶����Ӹ��ڵ��ѯĳ����������ֵ */
int query(int index, int left, int right)
{
	if(segTree[index].left == left && segTree[index].right == right)
	{
		return segTree[index].maxV;
	}
	int mid = (segTree[index].left+segTree[index].right) >> 1;
	if(right <= mid)     // ע��������Ϊ[0,mid] �� <=
	{
		return query((index<<1)+1,left,right);
	}else if(left > mid) // ע��������Ϊ[mid+1,right] �� >
	{
		return query((index<<1)+2,left,right);
	}
	return MAX(query((index<<1)+1,left,mid), query((index<<1)+2,mid+1,right));
}

void main()
{
	construct(0,0,sizeof arr / sizeof(int) - 1);

	// ע��construct��query����ʡ���˱߽���

	printf("max of [0,9] is: %d\n",query(0,0,9));
	printf("max of [1,4] is: %d\n",query(0,1,4));
	printf("max of [3,7] is: %d\n",query(0,3,7));
	printf("max of [6,9] is: %d\n",query(0,6,9));
}