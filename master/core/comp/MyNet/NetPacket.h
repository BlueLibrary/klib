#pragma once

// �������������
#define  MAX_NETPACKET_LENTTH  4*1024

// �����������ݰ���
class net_conn;
class NetPacket
{
public:
    NetPacket(void);
    ~NetPacket(void);

public:
    BOOL    m_bFixed;                   // ��ʾ�Ƿ��ǹ̶��ģ�������Ҫ��delete����
    net_conn*  pConn;	            // �����ĸ�����
    size_t datalen;                     // �������б������ݵĴ�С
    char buff[MAX_NETPACKET_LENTTH];    // ������
};