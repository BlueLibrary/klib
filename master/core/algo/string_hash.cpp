//#include "StdAfx.h"
#include "string_hash.h"

namespace klib{
namespace algo{


string_hash::string_hash(const long ntable_length /*= MAXTABLELEN*/)
{
	init_crypt_table();

	m_tablelength = ntable_length;  
	//��ʼ��hash��
	m_HashIndexTable = new HASHTABLE[ntable_length];  
	for ( int i = 0; i < ntable_length; i++ )  
	{  
		m_HashIndexTable[i].nHashA = -1;  
		m_HashIndexTable[i].nHashB = -1;  
		m_HashIndexTable[i].bExists = false;  
	}          
}

string_hash::~string_hash(void)
{
	//�����ڴ�
	if ( NULL != m_HashIndexTable )  
	{  
		delete []m_HashIndexTable;  
		m_HashIndexTable = NULL;  
		m_tablelength = 0;  
	}  
}

/************************************************************************/
/*��������InitCryptTable
/*��  �ܣ��Թ�ϣ������Ԥ����  
/*����ֵ����
/************************************************************************/
void string_hash::init_crypt_table()  
{   
	unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;  

	for( index1 = 0; index1 < 0x100; index1++ )  
	{   
		for( index2 = index1, i = 0; i < 5; i++, index2 += 0x100 )  
		{   
			unsigned long temp1, temp2;  
			seed = (seed * 125 + 3) % 0x2AAAAB;  
			temp1 = (seed & 0xFFFF) << 0x10;  
			seed = (seed * 125 + 3) % 0x2AAAAB;  
			temp2 = (seed & 0xFFFF);  
			cryptTable[index2] = ( temp1 | temp2 );   
		}   
	}   
}  

/************************************************************************/
/*��������HashString
/*��  �ܣ���ȡ��ϣֵ   
/*����ֵ������hashֵ
/************************************************************************/
unsigned long string_hash::caculate_hash(const string& lpszString, unsigned long dwHashType)  
{   
	unsigned char *key = (unsigned char *)(const_cast<char*>(lpszString.c_str()));  
	unsigned long seed1 = 0x7FED7FED, seed2 = 0xEEEEEEEE;  
	int ch;  

	while(*key != 0)  
	{   
		ch = toupper(*key++);  

		seed1 = cryptTable[(dwHashType << 8) + ch] ^ (seed1 + seed2);  
		seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;   
	}  
	return seed1;   
}  

/************************************************************************/
/*��������Hashed
/*��  �ܣ����һ���ַ����Ƿ�hash��
/*����ֵ��������ڣ�����λ�ã����򣬷���-1
/************************************************************************/
unsigned long string_hash::is_hashed(const string& lpszString)  

{   
	const unsigned long HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2;  
	//��ͬ���ַ�������hash������ײ�ļ������޽ӽ��ڲ�����
	unsigned long nHash = caculate_hash(lpszString, HASH_OFFSET);  
	unsigned long nHashA = caculate_hash(lpszString, HASH_A);  
	unsigned long nHashB = caculate_hash(lpszString, HASH_B);  
	unsigned long nHashStart = nHash % m_tablelength,  
	nHashPos = nHashStart;  

	while ( m_HashIndexTable[nHashPos].bExists)  
	{   
		if (m_HashIndexTable[nHashPos].nHashA == nHashA && m_HashIndexTable[nHashPos].nHashB == nHashB)   
			return nHashPos;   
		else   
			nHashPos = (nHashPos + 1) % m_tablelength;  

		if (nHashPos == nHashStart)   
			break;   
	}  

	return -1; //û���ҵ�  
}  

/************************************************************************/
/*��������Hash
/*��  �ܣ�hashһ���ַ��� 
/*����ֵ���ɹ�������true��ʧ�ܣ�����false
/************************************************************************/
bool string_hash::hash(const string& lpszString)
{  
	const unsigned long HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2;  
	unsigned long nHash = caculate_hash(lpszString, HASH_OFFSET);  
	unsigned long nHashA = caculate_hash(lpszString, HASH_A);  
	unsigned long nHashB = caculate_hash(lpszString, HASH_B);  
	unsigned long nHashStart = nHash % m_tablelength, 
		nHashPos = nHashStart;  

	while ( m_HashIndexTable[nHashPos].bExists)  
	{   
		nHashPos = (nHashPos + 1) % m_tablelength;  
		if (nHashPos == nHashStart) //һ���ֻ�  
		{  
			//hash����û�п����λ����,�޷����hash
			return false;   
		}  
	}  
	m_HashIndexTable[nHashPos].bExists = true;  
	m_HashIndexTable[nHashPos].nHashA = nHashA;  
	m_HashIndexTable[nHashPos].nHashB = nHashB;  

	return true;  
}  


}}
