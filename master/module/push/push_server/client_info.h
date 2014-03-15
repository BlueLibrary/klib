#pragma once

using namespace klib::mem;

#include <net/ip_v4.h>
#include <pattern/fix_circle_buff.h>


//----------------------------------------------------------------------
// �ͻ���ȷ���˵���ϢID
struct confirm_info
{
    confirm_info() : uMsgID(0), uConfirmTime(0) {}
    confirm_info(UINT64 uID, UINT64 uTime) : uMsgID(uID), uConfirmTime(uTime) {}

    UINT64  uMsgID;
    UINT64  uConfirmTime;
};


//----------------------------------------------------------------------
// �ͻ�����Ϣ
class client_info
{
public:
    client_info() : 
        client_port_(0),

        uLoginTime(0), 
        uLastActiveTime(0), 
        uHeartCount(0),
        uVersion(0),

        uLastMsgID(0)
     {}
    ~client_info(void) {}


    ip_v4   client_addr_;              ///< �ͻ��˵�IP��ַ
    USHORT  client_port_;             ///< �ͻ��˵Ķ˿ڣ������ֽ���

    UINT64  uLoginTime;              ///< ��½��ʱ��
    UINT64  uLastActiveTime;         ///< �ϴλ�Ծ��ʱ��
    UINT64  uHeartCount;             ///< ��������
    UINT    uVersion;                ///< �汾ֵ

    UINT64  uLastMsgID;              ///< ��Ϣ�ı��
    small_string<13>  strMac;        ///< mac��ַ��δʹ��
    small_string<21>  strChannel;    ///< �ͻ��˵�����
    small_string<31>  strLoginName;  ///< ���Ե�½��
    small_string<31>  strAccount;    ///< ���µ��ʺ�

    UINT64    uid;                   ///< �û���ID

    fix_circle_buff<confirm_info, UINT8, 10> objConfirmList;
};

