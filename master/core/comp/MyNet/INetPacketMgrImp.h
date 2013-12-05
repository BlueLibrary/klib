#pragma once

#include "INetPacketMgr.h"
#include <list>
#include <map>
#include <hash_map>
#include <queue>

#define  NET_MAX_NETPACKET_LIST_LENGTH 9                        // ����hash����ĳ���
#define  NET_MAX_PACKET_COUNT	5000                            // ��������������������

//----------------------------------------------------------------------
// Summary:
//		�����߼��������ʵ��
//----------------------------------------------------------------------
class INetPacketMgrImp : public INetPacketMgr
{
public:
    INetPacketMgrImp(void);
    ~INetPacketMgrImp(void);

public:
    //----------------------------------------------------------------------
    // Summary:
    //		�������ӵ������������
    // Parameters:
    //		pPacket		-	���ɵ�������
    // Returns:
    //		��ӳɹ�����true,ʧ�ܷ���false
    //----------------------------------------------------------------------
    virtual bool AddPacket(NetPacket* pPacket) ;


    //----------------------------------------------------------------------
    // Summary:
    //		���������������л�ȡһ�����ɺ��˵ķ��
    // Parameters:
    //		bRemoveFlag		-	�Ƿ�ɾ���ı��
    // Returns:
    //		��ȡ�ɹ����ط����ָ�룬ʧ�ܷ���NULL
    //----------------------------------------------------------------------
    virtual NetPacket* GetPacket(bool bRemoveFlag = true) ;


    //----------------------------------------------------------------------
    // Summary:
    //		�ͷ�һ�����Ӷ����µ����з��
    // Parameters:
    //		pConn		-	����ָ�����
    // Returns:
    //		�ͷ���ɷ���true, ʧ�ܷ���false
    //----------------------------------------------------------------------
    virtual bool FreeConnPacket(INetConnection* pConn);  

    //----------------------------------------------------------------------
    // Summary:
    //		����һ���������
    // Returns:
    //		�ɹ�����ָ�룬ʧ�ܷ���NULL
    //----------------------------------------------------------------------
    virtual NetPacket* AllocNetPacket() ;


    //----------------------------------------------------------------------
    // Summary:
    //		�ͷ�������
    // Parameters:
    //		pPacket		-	������ָ��
    // Returns:
    //		�ͷųɹ�����true,ʧ�ܷ���false
    // Remark:
    //        �ͷŵ�һ�������л����ͷ��ڴ�
    //----------------------------------------------------------------------
    virtual bool FreeNetPacket(NetPacket* pPacket) ;

protected:
    bool InitPacketMgr(UINT uInitPacketNum = 900) ; //nCount��ʾ��ʼ�������
    int  HashFun(void* param);

protected:
    typedef std::list<NetPacket*> NetPacketListType;

    //----------------------------------------------------------------------
    // Summary:
    //       ��ʾһ�������±���Ķ���������,���һ�����ӵķ������������
    //----------------------------------------------------------------------
    struct stPacketStore{
        INetConnection* pConn;
        NetPacketListType m_packetList;
    };

    typedef std::list<stPacketStore*> NetConnListType;
    typedef stdext::hash_map<INetConnection*, stPacketStore*> NetConnMapType;
    struct stConnPacketHash {
        NetConnListType connQueue;              // ���������ʽ�������е�����ָ��
        NetConnMapType  connMapVisited;         // ��map��ʽ�����Ѿ������˵�����
        NetConnMapType  connMapExists;           // ��map��ʽ�������е�����
    };

    //----------------------------------------------------------------------
    // Summary:
    //      ��hash������ʽ�������ӣ��Ա���ٲ���
    //----------------------------------------------------------------------
    stConnPacketHash          m_CombinedConnList[NET_MAX_NETPACKET_LIST_LENGTH];

    //----------------------------------------------------------------------
    // Summary:
    //      �������Ӧ��ͬ������
    //----------------------------------------------------------------------
    auto_cs                   m_CsCombinedList[NET_MAX_NETPACKET_LIST_LENGTH];

    //----------------------------------------------------------------------
    // Summary:
    //      ��ʾ�ϴζ�ȡ�������е��±�
    //----------------------------------------------------------------------
    int m_iReadPacketPos;

    //----------------------------------------------------------------------
    // Summary:
    //      �������������ͷ����
    //----------------------------------------------------------------------
    NetPacketListType         m_FreePacketList;
    auto_cs                   m_CsAllocedList;

};