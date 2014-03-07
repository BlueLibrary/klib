
#if !defined(_Klib_HttpDown_H)
#define _Klib_HttpDown_H


#include <cstdio>
#include <string>
#include "tcp_socket.h"



namespace klib {
namespace net {


///< �������ؽ��ջ�������С
#define  HTTPDOWN_RECV_BUFF			(2*1024)


//----------------------------------------------------------------------
// 
// User-Agent:Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.17 (KHTML, like Gecko) Chrome/24.0.1312.52 Safari/537.17
//----------------------------------------------------------------------

class http_down  
{
public:
	http_down() {}
	virtual ~http_down() {}

public:
    ///< ����URL��ַ
	BOOL parse_url(const char* url, char* hostAddr, int& port, char* getPath) ;

    ///< ����url�ļ���ָ����·��
	BOOL download(const char* pszHttpUrl, const char* pszSaveFile);

    ///< ֱ�ӻ�ȡurl·��������
	BOOL get_url_content(const char* pszHttpUrl, std::string& str_content);
    	
protected:
	tcp_socket m_socket;                        ///< TCP�׽��ֶ���
	char m_szRecvBuff[HTTPDOWN_RECV_BUFF];      ///< ���ջ�����  
};


}}



#endif // !defined(_Klib_HttpDown_H)
