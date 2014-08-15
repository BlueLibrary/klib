
#if !defined(_Klib_HttpDown_H)
#define _Klib_HttpDown_H


#include <functional>
#include <cstdio>
#include <string>
#include "tcp_socket.h"



namespace klib {
namespace net {


///< �������ؽ��ջ�������С
#define  HTTPDOWN_RECV_BUFF			(2*1024)

// ����ʱ��ͷ����һЩ��Ϣ
struct request_header_info
{
    std::string cookie_;
    std::string refer_;
    std::string user_agent_;
};


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
	bool parse_url(const char* url, char* hostAddr, int& port, char* getPath) ;

    ///< ����url�ļ���ָ����·��
	bool download(const char* http_url, 
        const char* pszSaveFile, 
        const request_header_info& info = request_header_info());

    ///< ֱ�ӻ�ȡurl·��������
	bool get_url_content(const char* http_url, 
        std::string& str_content,
        const request_header_info& info = request_header_info());

protected:
    typedef std::function<void(const char* data, size_t len)> data_callback;
    bool handle_url_content(const char* http_url,
        const request_header_info& info,
        const data_callback& handler);

protected:
	tcp_socket m_socket;                      // TCP�׽��ֶ���

};


}}



#endif // !defined(_Klib_HttpDown_H)
