#include "StdAfx.h"
#include "inetpacket_mgr_imp.h"
#include "net_packet.h"
#include "net_conn.h"

inetpacket_mgr_imp::inetpacket_mgr_imp(void)
{
    read_packet_pos_ = 0;

    InitPacketMgr();
}

inetpacket_mgr_imp::~inetpacket_mgr_imp(void)
{
}

bool inetpacket_mgr_imp::add_packet(net_packet* pPacket) 
{
    int iPos =  HashFun(pPacket->pConn);
    auto_lock helper(combin_list_mutex_[iPos]);

    NetConnMapType::const_iterator itrMap;
    stConnPacketHash& stPair = combin_list_[iPos];
    itrMap = stPair.connMapExists.find(pPacket->pConn);
    if (itrMap == stPair.connMapExists.end()) 
    {
        //û�д��������
        stPacketStore* pStore = new stPacketStore;
        if (NULL == pStore) 
        {
            return false;
        }

        pStore->pConn = pPacket->pConn;
        pStore->net_packet_list_.push_back(pPacket);
        stPair.connQueue.push_back(pStore);
        stPair.connMapExists[pPacket->pConn] = pStore;
    }
    else 
    {
        //��ӷ��
        itrMap->second->net_packet_list_.push_back(pPacket);
    }

    return true;
}

net_packet* inetpacket_mgr_imp::get_packet(bool bRemoveFlag/* = true*/) 
{
    net_packet* pPacket = NULL;
    int ipos = 0;

    stPacketStore* stStore = NULL;
    int  iRepeatCount = 0;

    do
    {
        ++ iRepeatCount;
        read_packet_pos_ %= NET_MAX_NETPACKET_LIST_LENGTH;
        ipos = read_packet_pos_;
        ++ read_packet_pos_;
        if (combin_list_[ipos].connMapExists.empty()) {
            continue;
        }

        auto_lock helper(combin_list_mutex_[ipos]);
        stConnPacketHash& stpair = combin_list_[ipos];
        BOOL bRepeatFlag = FALSE;

        do 
        {
            bRepeatFlag = FALSE;
            if (!stpair.connQueue.empty()) 
            {
                //�����л���û�з��ʹ���
                stStore = stpair.connQueue.front();     //ȡ����ǰһ��δ��������
                stpair.connQueue.pop_front();     //�������ӵ�������
                stpair.connMapVisited[stStore->pConn] = stStore;    //���浽�Ѿ������˵��б�
                if (!stStore->net_packet_list_.empty()) 
                {
                    //���������з��δ����
                    pPacket = stStore->net_packet_list_.front();
                    stStore->net_packet_list_.pop_front();
                }
                else 
                {
                    //��������û�з����������Ҫ�ٴ�ȡ�ö��е�����
                    bRepeatFlag = TRUE;
                }
            }
            else {
                //ȫ���ʹ��ˣ������ʹ���ȫ����ӵ�������ȥ
                NetConnMapType::const_iterator itrRebuild;
                itrRebuild = stpair.connMapVisited.begin();
                for (; itrRebuild != stpair.connMapVisited.end(); ++itrRebuild)
                {
                    stpair.connQueue.push_back(itrRebuild->second);
                    if (!itrRebuild->second->net_packet_list_.empty()) 
                    {
                        bRepeatFlag = TRUE;
                    }
                }      
                stpair.connMapVisited.clear();
            }
        } while (bRepeatFlag);

    } while (NULL == pPacket && iRepeatCount < NET_MAX_NETPACKET_LIST_LENGTH + 1);

    return pPacket;
}

bool inetpacket_mgr_imp::free_conn_packets(net_conn* pConn)
{
    _ASSERT(pConn);
    int ipos =  HashFun(pConn);
    auto_lock helper(combin_list_mutex_[ipos]);

    NetConnMapType::const_iterator itrMap;
    stConnPacketHash& stpair = combin_list_[ipos];
    itrMap = stpair.connMapExists.find(pConn);
    if (itrMap == stpair.connMapExists.end()) 
    {
        //�����ڵľͲ���Ҫ�ͷ�
        return false;
    }
    else 
    {
        //���ڣ���Ҫ�Ӷ��У�map��ɾ��������exist��ɾ��
        NetConnMapType::const_iterator itrVistMap;
        NetConnListType::const_iterator itrConnList;
        NetPacketListType::const_iterator itrPackList;
        stPacketStore* stStore = NULL;

        stpair.connMapExists.erase(itrMap);
        itrVistMap = stpair.connMapVisited.find(pConn);
        if (itrVistMap != stpair.connMapVisited.end()) 
        {
            //ɾ���ѷ����б��е�����
            stStore = itrVistMap->second;
            stpair.connMapVisited.erase(itrVistMap);
        }
        else 
        {
            //ɾ���ڶ����е�����(����ٶ�����Щ��������θĽ�)
            itrConnList = stpair.connQueue.begin();
            for (; itrConnList != stpair.connQueue.end(); ++itrConnList)
            {
                if ((*itrConnList)->pConn == pConn) 
                {
                    stStore = *itrConnList;
                    stpair.connQueue.erase(itrConnList);
                    break;
                }
            }
        }

        if (stStore) 
        {
            //�ͷ������µķ��
            itrPackList = stStore->net_packet_list_.begin();
            for (; itrPackList != stStore->net_packet_list_.end(); ++itrPackList) 
            {
                free_net_packet(*itrPackList);
            }
            stStore->net_packet_list_.clear();
            delete stStore;
        }
    }

    return false;
}

net_packet* inetpacket_mgr_imp::alloc_net_packet() 
{
    net_packet* pPacket = NULL;

    auto_lock helper(free_packet_list_mutex_);

    // ��������б�������ӿ����б��л�ȡ������ʼ��
    // ����ʹ��ڴ�������
    if (!free_packet_list_.empty())
    {
        pPacket = free_packet_list_.front();

        BOOL bFixed = pPacket->m_bFixed;
        new(pPacket) net_packet;
        pPacket->m_bFixed = bFixed;

        free_packet_list_.pop_front();
    }
    else 
    {
        pPacket = new net_packet;
    }

    return pPacket;
}

bool inetpacket_mgr_imp::free_net_packet(net_packet* pPacket) 
{
    _ASSERT(pPacket);
    auto_lock helper(free_packet_list_mutex_);

    // �����б��еĶ���Ƚ϶��ʱ��Ҫ���ǹ̶�������ڴ���ͷŵ�
    if (free_packet_list_.size() > NET_MAX_PACKET_COUNT)
    {
        if (!pPacket->m_bFixed)
        {
            delete pPacket;
            return true;
        }
        else 
        {
            free_packet_list_.push_back(pPacket);
        }
    }
    else 
    {
        free_packet_list_.push_back(pPacket);
    }

    return true;
}

bool inetpacket_mgr_imp::InitPacketMgr(UINT uInitPacketNum/* = 300*/) 
{
    auto_lock helper(free_packet_list_mutex_);

    net_packet* buff = new net_packet[uInitPacketNum];
    if (NULL == buff) 
    {
        _ASSERT(FALSE && "out of memory!!!");
        return false;
    }

    // ��������ڴ汣�浽�����У��Ȳ���ʼ������ʹ�õ�ʱ���ʼ��(placement new)
    net_packet* pPacket = NULL;
    for (UINT i=0; i<uInitPacketNum; ++ i) 
    {
        pPacket = (net_packet*) & buff[i];
        pPacket->m_bFixed = TRUE;
        free_packet_list_.push_back(pPacket);
    }

    return true;
}

int inetpacket_mgr_imp::HashFun(void* param)
{
    return ((int)param % NET_MAX_NETPACKET_LIST_LENGTH);
}