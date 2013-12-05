#include "StdAfx.h"
#include "INetPacketMgrImp.h"
#include "NetPacket.h"
#include "INetConnection.h"

INetPacketMgrImp::INetPacketMgrImp(void)
{
    m_iReadPacketPos = 0;

    InitPacketMgr();
}

INetPacketMgrImp::~INetPacketMgrImp(void)
{
}

bool INetPacketMgrImp::AddPacket(NetPacket* pPacket) 
{
    int iPos =  HashFun(pPacket->pConn);
    auto_lock helper(m_CsCombinedList[iPos]);

    NetConnMapType::const_iterator itrMap;
    stConnPacketHash& stPair = m_CombinedConnList[iPos];
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
        pStore->m_packetList.push_back(pPacket);
        stPair.connQueue.push_back(pStore);
        stPair.connMapExists[pPacket->pConn] = pStore;
    }
    else 
    {
        //��ӷ��
        itrMap->second->m_packetList.push_back(pPacket);
    }

    return true;
}

NetPacket* INetPacketMgrImp::GetPacket(bool bRemoveFlag/* = true*/) 
{
    NetPacket* pPacket = NULL;
    int ipos = 0;

    stPacketStore* stStore = NULL;
    int  iRepeatCount = 0;

    do
    {
        ++ iRepeatCount;
        m_iReadPacketPos %= NET_MAX_NETPACKET_LIST_LENGTH;
        ipos = m_iReadPacketPos;
        ++ m_iReadPacketPos;
        if (m_CombinedConnList[ipos].connMapExists.empty()) {
            continue;
        }

        auto_lock helper(m_CsCombinedList[ipos]);
        stConnPacketHash& stpair = m_CombinedConnList[ipos];
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
                if (!stStore->m_packetList.empty()) 
                {
                    //���������з��δ����
                    pPacket = stStore->m_packetList.front();
                    stStore->m_packetList.pop_front();
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
                    if (!itrRebuild->second->m_packetList.empty()) 
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

bool INetPacketMgrImp::FreeConnPacket(INetConnection* pConn)
{
    _ASSERT(pConn);
    int ipos =  HashFun(pConn);
    auto_lock helper(m_CsCombinedList[ipos]);

    NetConnMapType::const_iterator itrMap;
    stConnPacketHash& stpair = m_CombinedConnList[ipos];
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
            itrPackList = stStore->m_packetList.begin();
            for (; itrPackList != stStore->m_packetList.end(); ++itrPackList) 
            {
                FreeNetPacket(*itrPackList);
            }
            stStore->m_packetList.clear();
            delete stStore;
        }
    }

    return false;
}

NetPacket* INetPacketMgrImp::AllocNetPacket() 
{
    NetPacket* pPacket = NULL;

    auto_lock helper(m_CsAllocedList);

    // ��������б�������ӿ����б��л�ȡ������ʼ��
    // ����ʹ��ڴ�������
    if (!m_FreePacketList.empty())
    {
        pPacket = m_FreePacketList.front();

        BOOL bFixed = pPacket->m_bFixed;
        new(pPacket) NetPacket;
        pPacket->m_bFixed = bFixed;

        m_FreePacketList.pop_front();
    }
    else 
    {
        pPacket = new NetPacket;
    }

    return pPacket;
}

bool INetPacketMgrImp::FreeNetPacket(NetPacket* pPacket) 
{
    _ASSERT(pPacket);
    auto_lock helper(m_CsAllocedList);

    // �����б��еĶ���Ƚ϶��ʱ��Ҫ���ǹ̶�������ڴ���ͷŵ�
    if (m_FreePacketList.size() > NET_MAX_PACKET_COUNT)
    {
        if (!pPacket->m_bFixed)
        {
            delete pPacket;
            return true;
        }
        else 
        {
            m_FreePacketList.push_back(pPacket);
        }
    }
    else 
    {
        m_FreePacketList.push_back(pPacket);
    }

    return true;
}

bool INetPacketMgrImp::InitPacketMgr(UINT uInitPacketNum/* = 300*/) 
{
    auto_lock helper(m_CsAllocedList);

    NetPacket* buff = new NetPacket[uInitPacketNum];
    if (NULL == buff) 
    {
        _ASSERT(FALSE && "out of memory!!!");
        return false;
    }

    // ��������ڴ汣�浽�����У��Ȳ���ʼ������ʹ�õ�ʱ���ʼ��(placement new)
    NetPacket* pPacket = NULL;
    for (UINT i=0; i<uInitPacketNum; ++ i) 
    {
        pPacket = (NetPacket*) & buff[i];
        pPacket->m_bFixed = TRUE;
        m_FreePacketList.push_back(pPacket);
    }

    return true;
}

int INetPacketMgrImp::HashFun(void* param)
{
    return ((int)param % NET_MAX_NETPACKET_LIST_LENGTH);
}