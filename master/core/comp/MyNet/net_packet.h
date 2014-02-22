#pragma once

class net_conn;
class tcp_net_facade_imp;
class inetpacket_mgr_imp;

// �����������ݰ���
class net_packet
{
    friend tcp_net_facade_imp;
    friend inetpacket_mgr_imp;
public:
    net_packet(void);
    ~net_packet(void);

    bool init_buff(size_t bf_size);
    char* get_buff() ;

protected:
    net_conn*   pConn;	                        // �����ĸ�����

    size_t      bf_size_;                       // �������б������ݵĴ�С
    char*       bf_ptr_;                        // ������
};