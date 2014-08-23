
#if !defined(_Klib_TcpSoket_H)
#define _Klib_TcpSoket_H


#include <WinSock2.h>
#include <windows.h>


namespace klib {
namespace net {



//TCP�׽�����
//ʹ��:
//	tcp_socket  socket;
//  socket.init();
//  socket.connect();
class tcp_socket  
{
public:
    tcp_socket();
    tcp_socket(SOCKET sock, BOOL bConnected);
    virtual ~tcp_socket();

public:
    operator SOCKET& () {  return sock_; }

public:
    BOOL init();              ///< ��ʼ���׽���,���uPort��Ϊ0���
    SOCKET& get_socket();

    BOOL bind_port(USHORT uPort);                    ///< 
    USHORT get_bind_port();                           ///< ���صĶ˿�Ϊ������

    BOOL listen(int ibacklog = 5);                  ///< ��ʾ����
    tcp_socket accept();                            ///< ��������

    BOOL connect(const char* ip, USHORT uPort);		///< ������
    BOOL connect(UINT32  uIP, USHORT uPort);		///< �����ֽ���

    BOOL close();                                   ///< �ر�����
    BOOL is_connected();                              ///< �ж��Ƿ�����
    BOOL enable_noblock(BOOL bEnable = TRUE);        ///< ���÷�����
    BOOL send(const char* buff, int iLen);          ///< ��������
    int  recv(char* buff, int iLenOfBuff, int seconds, int useconds);  ///< seconds = 1 �ȴ�1��

protected:
    SOCKET sock_;                                  ///< �׽���
    BOOL   connected_;                            ///< ��ʾ�Ƿ���������

};


}}


#endif // !defined(_Klib_TcpSoket_H)
