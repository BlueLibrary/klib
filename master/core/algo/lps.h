#include <iostream>
using namespace std;

/* ������Ӵ� LPS */

int maxlen;  // ��¼������Ӵ��ĳ���

/* ��׺���� */
char * suff[100];

int pstrcmp(const void *p, const void *q)
{
	return strcmp(*(char**)p,*(char**)q);
}

int comlen_suff(char * p, char * q)
{
	int len = 0;
	while(*p && *q && *p++ == *q++)
	{
		++len;
		if(*p == '#' || *q == '#')
		{
			break;
		}
	}
	int count = 0;
	while(*p)
	{
		if(*p++ == '#')
		{
			++count;
			break;
		}
	}
	while(*q)
	{
		if(*q++ == '#')
		{
			++count;
			break;
		}
	}
	if(count == 1)
		return len;
	return 0;
}

void LPS_suffix(char * X, int xlen)
{
	maxlen = 0;

	int len_suff = 2 * xlen + 1;
	char * arr = new char [len_suff + 1];  /* ��X������X���ӵ�һ�� */
	strcpy(arr,X);
	arr[xlen] = '#';

	char *p = X, *q = arr + len_suff;  /* ������ */
	*q = '\0';
	while(*p && (*--q = *p++));

	for(int i = 0; i < len_suff; ++i)  /* ��ʼ����׺���� */
	{
		suff[i] = & arr[i];
	}

	qsort(suff, len_suff, sizeof(char *), pstrcmp);

	for(int i = 0; i < len_suff-1; ++i)
	{
		int len = comlen_suff(suff[i],suff[i+1]);
		if(len > maxlen)
		{
			maxlen = len;
		}
	}
}

/* O(n)�ⷨ */
#define MIN(a,b) ((a) < (b) ? (a) : (b))

int maxid;        // ������Ӵ��±�
int LPS_rb[100];  // iΪ���ĵĻ����Ӵ��ұ߽��±�right border
char str[100];    // ԭ�ַ��������ĸ���

void LPS_linear(char * X, int xlen)
{
	maxlen = maxid = 0;

	str[0] = '$';  // ��ԭ��������������ʽ
	char *p = str;
	*(++p)++ = '#';
	while((*p++ = *X++) != '\0')
	{
		*p++ = '#';
	}
	
	for(int i = 1; str[i]; ++i)  // ����LPS_rb��ֵ
	{
		if(maxlen > i)          // ��ʼ��LPS[i]
		{
			LPS_rb[i] = MIN(LPS_rb[2*maxid-i],(maxlen-i));
		}else
		{
			LPS_rb[i] = 1;
		}
		while(str[i-LPS_rb[i]] == str[i+LPS_rb[i]]) // ��չ
		{
			++LPS_rb[i];
		}
		if(LPS_rb[i]-1 > maxlen)
		{
			maxlen = LPS_rb[i]-1;
			maxid = i;
		}
	}
}

void main()
{
	char X[30];  // �贮������30
	/* test case
	 * aaaa
	 * abab
	 */
	while(cin.getline(X,30))
	{
		/* ��׺���鷽�� */
		LPS_suffix(X,strlen(X));
		printf("%d\n", maxlen);

		/* O(n)���� */
		LPS_linear(X,strlen(X));
		printf("%d\n", maxlen);
	}	
}