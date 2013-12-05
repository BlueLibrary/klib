#pragma once
#include "../istddef.h"
#include <string>

namespace klib {
namespace algo {

using namespace std;


#define MAXTABLELEN 1024    // Ĭ�Ϲ�ϣ�������С 
//////////////////////////////////////////////////////////////////////////  
// ��ϣ��������  
typedef struct  _HASHTABLE
{  
	long nHashA;
	long nHashB;
	bool bExists;
} HASHTABLE, *PHASHTABLE ;

class string_hash
{
public:
	string_hash(const long ntable_length = MAXTABLELEN);
	~string_hash(void);

private:  
	unsigned long   cryptTable[0x500];      // 
	unsigned long   m_tablelength;          // ��ϣ��������  
	HASHTABLE *     m_HashIndexTable; 

private:
	void init_crypt_table();                                               // �Թ�ϣ������Ԥ���� 
	unsigned long caculate_hash(const string& lpszString, unsigned long dwHashType); // ��ȡ��ϣֵ   

public:
	bool hash(const string& url);
	unsigned long is_hashed(const string&  url);    // ���url�Ƿ�hash��
};


}}