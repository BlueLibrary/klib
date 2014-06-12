#ifndef _klib_ip_seg_table_h
#define _klib_ip_seg_table_h

#include "../Net/ip_v4.h"
#include <set>
using std::set;

namespace klib {
namespace algo {
    
using namespace klib::net;


// Ip�νṹ�嶨��
struct ip_seg
{
    // ע:�����������Ip, ������Ĵ�С�Ƚϻ������    
    ip_v4    ulStartIp;         
    ip_v4    ulEndIp;

    //      ����Set��С�ں�����    
    bool operator < (const ip_seg &stCmpElem) const
    {
        return ulEndIp < stCmpElem.ulStartIp;
    }
};


class ip_seg_table
{
public:
    ip_seg_table(void) { clear(); }
    ~ip_seg_table(void) { clear(); }

public:
    // ���Ip��
    void clear() { m_IpTable.clear(); }

    // ����һ��Ip��, �����Ƿ�ɹ�
    bool insert(const ip_seg &stElem)
    {
        // ���Ϸ���Ip��
        if (stElem.ulStartIp > stElem.ulEndIp)
        {
            return false;
        }
        return m_IpTable.insert(stElem).second;           // �����в���һ��Ip�Σ�����Ip���ص�ʱ��ʧ��
    }
        
    // ͬ��
    bool insert(ip_v4 ulStartIp, ip_v4 ulEndIp)
    {
        ip_seg   stTempElem;
        stTempElem.ulStartIp = ulStartIp;
        stTempElem.ulEndIp   = ulEndIp;
        return insert(stTempElem);
    }
        
    // ����һ��ip��ַ��
    bool insert(const char* pszStartIp, const char* pszEndIp) 
    {
        ip_seg  stTempElem;
        stTempElem.ulStartIp = ntohl(inet_addr(pszStartIp));
        stTempElem.ulEndIp   = ntohl(inet_addr(pszEndIp));
        return insert(stTempElem);
    }

    
    //    
    bool insert(const char* pszStartIp, UINT uIpCount)
    {
        ip_seg  stTempElem;
        stTempElem.ulStartIp  = ntohl(inet_addr(pszStartIp));
        stTempElem.ulEndIp    = stTempElem.ulStartIp + uIpCount - 1;
        return insert(stTempElem);
    }
        
    // ɾ��������Ip��Ip��, �����Ƿ�ɹ�
    bool remove(const ip_v4 ulIp)
    {
        ip_seg   stTempElem;
        stTempElem.ulStartIp = stTempElem.ulEndIp = ulIp;

        return 1 == m_IpTable.erase(stTempElem);
    }

    //  ���Ұ�����Ip��Ip��
    //  �ҵ����ظ�Ip�ε�ָ�� (����ֱ���޸�ָ��ָ�������)
    //  ���򷵻�NULL
    const ip_seg * find(const ip_v4 ulIp)
    {
        ip_seg   stTempElem;
        stTempElem.ulStartIp = stTempElem.ulEndIp = ulIp;
        set <ip_seg>::iterator itr = m_IpTable.find(stTempElem);

        return itr == m_IpTable.end() ? NULL : &*itr;
    }

protected:
    // �洢Ip��Set��
    std::set <ip_seg>         m_IpTable;            
};


}}

#endif
