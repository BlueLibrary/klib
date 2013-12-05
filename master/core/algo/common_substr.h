#include <iostream>
using namespace std;

/* ������Ӵ� Longest Common Substring */

int maxlen;    /* ��¼��󹫹��Ӵ����� */
int maxindex;  /* ��¼��󹫹��Ӵ��ڴ�1����ʼλ�� */
void outputLCS(char * X);  /* ���LCS */

/* ������Ӵ� �����㷨 */
int comlen(char * p, char * q)
{
	int len = 0;
	while(*p && *q && *p++ == *q++)
	{
		++len;
	}
	return len;
}

void LCS_base(char * X, int xlen, char * Y, int ylen)
{
	for(int i = 0; i < xlen; ++i)
	{
		for(int j = 0; j < ylen; ++j)
		{
			int len = comlen(&X[i],&Y[j]);
			if(len > maxlen)
			{
				maxlen = len;
				maxindex = i;
			}
		}
	}
	outputLCS(X);
}

/* ������Ӵ� DP */
int dp[30][30];

void LCS_dp(char * X, int xlen, char * Y, int ylen)
{
	maxlen = maxindex = 0;
	for(int i = 0; i < xlen; ++i)
	{
		for(int j = 0; j < ylen; ++j)
		{
			if(X[i] == Y[j])
			{
				if(i && j)
				{
					dp[i][j] = dp[i-1][j-1] + 1;
				}
				if(i == 0 || j == 0)
				{
					dp[i][j] = 1;
				}
				if(dp[i][j] > maxlen)
				{
					maxlen = dp[i][j];
					maxindex = i + 1 - maxlen;
				}
			}
		}
	}
	outputLCS(X);
}

/* ������Ӵ� ��׺���� */
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
			return len;
		}
	}
	return 0;
}

void LCS_suffix(char * X, int xlen, char * Y, int ylen)
{
	int suf_index = maxlen = maxindex = 0;

	int len_suff = xlen + ylen + 1;
	char * arr = new char [len_suff + 1];  /* ��X��Y���ӵ�һ�� */
	strcpy(arr,X);
	arr[xlen] = '#';
	strcpy(arr + xlen + 1, Y);

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
			suf_index = i;
		}
	}
	outputLCS(suff[suf_index]);
}

/* ���LCS 
 * �ں�׺���鷽���У�maxindex=0
 * ��Ϊ�������ľ��Ǻ�׺����suff[]����0��ӡmaxlen���ַ�
 */
void outputLCS(char * X)
{
	if(maxlen == 0)
	{
		printf("NULL LCS\n");
		return;
	}
	printf("The len of LCS is %d\n",maxlen);

	int i = maxindex;
	while(maxlen--)
	{
		printf("%c",X[i++]);
	}
	printf("\n");
}

void main()
{
	char X[] = "aaaba";
	char Y[] = "abaa";

	/* �����㷨 */
	LCS_base(X,strlen(X),Y,strlen(Y));

	/* DP�㷨 */
	LCS_dp(X,strlen(X),Y,strlen(Y));

	/* ��׺���鷽�� */
	LCS_suffix(X,strlen(X),Y,strlen(Y));
}