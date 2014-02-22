#pragma once

#include "inetpacket_mgr.h"
#include <list>
#include <map>
#include <hash_map>
#include <queue>

#include <pattern/object_pool.h>
using namespace klib::pattern;

#define  NET_MAX_NETPACKET_LIST_LENGTH 9                        // ����hash����ĳ���
#define  NET_MAX_PACKET_COUNT	5000                            // ��������������������

//----------------------------------------------------------------------
// Summary:
//		�����߼��������ʵ��
//----------------------------------------------------------------------
class inetpacket_mgr_imp : public inetpacket_mgr
{
public:
    inetpacket_mgr_imp(void);
    ~inetpacket_mgr_imp(void);

public:
    virtual bool add_packet(net_packet* pPacket) ;                   ///< �������ӵ������������
    virtual net_packet* get_packet(bool bRemoveFlag = true) ;        ///< ���������������л�ȡһ�����ɺ��˵ķ��

    virtual bool free_conn_packets(net_conn* pConn);                ///< �ͷ�һ�����Ӷ����µ����з��

    virtual net_packet* alloc_net_packet() ;                         ///< ����һ���������
    virtual bool free_net_packet(net_packet* pPacket) ;              ///< �ͷ�������

protected:
    int  _hash_func(void* param);

protected:
    typedef std::list<net_packet*> NetPacketListType;

    //----------------------------------------------------------------------
    // Summary:
    //       ��ʾһ�������±���Ķ���������,���һ�����ӵķ������������
    //----------------------------------------------------------------------
    struct stPacketStore{
        net_conn* pConn;
        NetPacketListType net_packet_list_;
    };

    typedef std::list<stPacketStore*> NetConnListType;
    typedef stdext::hash_map<net_conn*, stPacketStore*> NetConnMapType;
    struct stConnPacketHash {
        NetConnListType connQueue;                  // ���������ʽ�������е�����ָ��
        NetConnMapType  connMapVisited;             // ��map��ʽ�����Ѿ������˵�����
        NetConnMapType  connMapExists;              // ��map��ʽ�������е�����
    };

    stConnPacketHash     combin_list_[NET_MAX_NETPACKET_LIST_LENGTH];               ///< ��hash������ʽ�������ӣ��Ա���ٲ���
    mutex                combin_list_mutex_[NET_MAX_NETPACKET_LIST_LENGTH];         ///< �������Ӧ��ͬ������
    
    int                  read_packet_pos_;               ///< ��ʾ�ϴζ�ȡ�������е��±�
        
    CObjectPool<net_packet, 10000, 10000> packet_pool_;

};