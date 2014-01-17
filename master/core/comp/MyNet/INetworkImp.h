#pragma once

#include "INetwork.h"
#include "net_socket.h"

#include <list>
#include <Mswsock.h>

//----------------------------------------------------------------------
// ���������
//  ���������������
class net_overLapped: public OVERLAPPED
{
public:
    ///<  �������û���Ҫ����Ϣ
    bool bFixed;                        ///< ��ʾ�Ƿ��ǹ̶����ڴ棬�������ͷ�

    //�ⲿʹ��
    int       operate_type_;            ///< �ύ�Ĳ�������
    void*     pend_data_;               ///< �����������ݵ�
    DWORD     transfer_bytes_;          ///< �����˵�����
    WSABUF    wsaBuf_;                  ///< ����������
    char      buff_[2 * 1024];			///< �������ݵ�buf
    
public:
    net_overLapped() : bFixed(false), pend_data_(0), transfer_bytes_(0)
    {
    }
};


/// �����ʵ��
class inetwork_imp : public inetwork
{
public:
    inetwork_imp(void);
    ~inetwork_imp(void);

public:
    virtual bool init_network() ;   
    virtual bool set_net_event_handler(inet_event_handler* handler) ;          ///< �����¼�����ӿ�
    virtual bool run_network() ;                                             ///< ���������-�������߳�

    virtual net_conn* post_accept(net_conn* pListenConn) ;	                ///< Ͷ�ݽ�������,�ɶ��Ͷ�ݣ������׽��ֱ����� create_listen_conn ����
    virtual bool post_connection(net_conn* pConn) ;		                    ///< Ͷ����������
    virtual bool post_read(net_conn* pConn) ;	                            ///< Ͷ�ݶ������ڽ������Ӻ�ײ��Ĭ��Ͷ��һ������
    virtual bool post_write(net_conn* pConn, const char* buff, size_t len) ;	///< Ͷ��д����

    virtual net_conn* create_listen_conn(USHORT uLocalPort) ;	            ///< �������ؼ����׽��֣�����һ��net_conn�ṹ�����ڽ�������
    virtual net_conn* create_conn() ;			                            ///< ����һ�������׽���
    virtual bool release_conn(net_conn* pConn);		                        ///< �ͷ�����

protected:
    static unsigned int WINAPI work_thread_(void* param);

    void init_fixed_overlapped_list(size_t nCount);                         ///< ��ʼ�̶���Overlapped�ĸ������ⲿ���ڴ治�ܱ��ͷ�
    net_overLapped* get_net_overlapped();                                   ///< �����ص��ṹ
    bool release_net_overlapped(net_overLapped* pMyoverlapped);	            ///< �ͷ��ص��ṹ

    void check_and_disconnect(net_conn* pConn);                             ///< �ж����׽����ϻ���û��δ�����Ͷ���������û������Ͽ�����

private:
    HANDLE                  hiocp_;                                         ///< ��ɶ˿ھ��
    inet_event_handler*     net_event_handler_;                             ///< �ƽ��ϲ㴦��Ľӿ�
    LPFN_ACCEPTEX           m_lpfnAcceptEx;                                 ///< AcceptEx����ָ��

    typedef std::list<net_overLapped*> OverLappedListType;                  ///< ����net_overLapped����������
    OverLappedListType      overlapped_list_;                               ///< ����������������net_overLapped
    auto_cs                 overlapped_list_mutex_;                         ///< ͬ������overlapped_list_

    typedef std::list<net_conn*> INetConnListType;                          ///< �ӿ��б����Ͷ���
    INetConnListType        free_net_conn_list_;                            ///< �����������ӽӿ�����
    auto_cs                 free_net_conn_mutex_;                           ///< ͬ������free_net_conn_list_
};