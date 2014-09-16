//#include "StdAfx.h"
#include "wm_wrapper.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

using namespace klib::algo;


wm_wrapper::wm_wrapper(void)
    : pattern_num_(0)
{
    wm_handle_ = wmNew();
}

wm_wrapper::~wm_wrapper(void)
{
    if (wm_handle_) 
    {
        wmFree(wm_handle_);
        wm_handle_ = nullptr;
    }
}

bool wm_wrapper::add_pattern(const char* pszPatten, int iLen, void* user_data)
{
    _ASSERT(pszPatten);
    int ret  = wmAddPattern(wm_handle_, (unsigned char *) pszPatten, iLen, user_data);
    if (0 == ret)
    {
        ++ pattern_num_;
    }
    return 0 == ret;
}

bool wm_wrapper::add_pattern(const char* pszPatten, void* user_data)
{
    _ASSERT(pszPatten);
    int ret  = wmAddPattern(wm_handle_, (unsigned char *) pszPatten, (int) strlen(pszPatten), user_data);
    if (0 == ret)
    {
        ++ pattern_num_;
    }
    return 0 == ret;
}

bool wm_wrapper::compile()
{
    return 0 == wmPrepPatterns(wm_handle_);
}

size_t wm_wrapper::size()
{
    return pattern_num_;
}

void wm_wrapper::set_callback(search_match_callback call, void* pthis)
{
    callback_func_ = call;
    callback_this_ = pthis;
}

bool wm_wrapper::search(const char* pszSearch, int nLen)
{
    _ASSERT(pszSearch);
    return wmSearch(wm_handle_, this, (unsigned char *) pszSearch, (int) nLen);
}

wm_wrapper::WM_STRUCT * wm_wrapper::wmNew()
{
    WM_STRUCT *p = (WM_STRUCT *) malloc(sizeof(WM_STRUCT));
    if (!p) {
        return 0;
    }
    memset(p, 0, sizeof(*p));
    p->msNumPatterns = 0;
    p->msSmallest = 1000;
    return p;
}
void wm_wrapper::wmFree(WM_STRUCT *ps)
{
    if (ps->plist) 
    {
        auto pTmp = ps->plist;
        int i = ps->msNumPatterns;
        while ( i > 0) 
        {
            pTmp = ps->plist;
            ps->plist = ps->plist->next;
            if (pTmp) 
            {
                if (pTmp->psPat) 
                {
                    free(pTmp->psPat);
                    pTmp->psPat = nullptr;
                }

                free(pTmp);
            }

            -- i;
        }
    }

    if (ps->msPatArray)
    {
        //if (ps->msPatArray->psPat)
            //free(ps->msPatArray->psPat);
        free(ps->msPatArray);
    }
    if (ps->msNumArray)
        free(ps->msNumArray);
    if (ps->msHash)
        free(ps->msHash);
    if (ps->msPrefix)
        free(ps->msPrefix);
    if (ps->msShift)
        free(ps->msShift);
    free(ps);
}

int wm_wrapper::wmAddPattern(WM_STRUCT *ps, unsigned char *q, int m, void* user_data)//m�ַ�������
{
    WM_PATTERN_STRUCT *p;
    p = (WM_PATTERN_STRUCT *) malloc(sizeof(WM_PATTERN_STRUCT));
    if (!p) {
        return -1;
    }

    memset(p, 0, sizeof(*p));
    p->psPat = (unsigned char*) malloc(m + 1);
    memset(p->psPat + m, 0, 1);
    memcpy(p->psPat, q, m);

    p->psLen = m;
    p->user_data = user_data;

    ps->msNumPatterns++;
    if (p->psLen < (unsigned) ps->msSmallest)
        ps->msSmallest = p->psLen;
    p->next = ps->plist;
    ps->plist = p;
    return 0;
}

unsigned wm_wrapper::HASH16(unsigned char *T)
{
    return (unsigned short) (((*T) << 8) | *(T + 1));
}

void wm_wrapper::sort(wm_wrapper::WM_STRUCT *ps)//�ַ�����ϣֵ��С��������
{
    int m = ps->msSmallest;
    int i, j;

    WM_PATTERN_STRUCT tmp_struct;  // ���ڽ�������

    int flag;
    for (i = ps->msNumPatterns - 1, flag = 1; i > 0 && flag; i--)
    {
        flag = 0;
        for (j = 0; j < i; j++)
        {
            if (HASH16(&(ps->msPatArray[j + 1].psPat[m - 2])) < HASH16(
                &(ps->msPatArray[j].psPat[m - 2])))
            {
                flag = 1;

                tmp_struct = ps->msPatArray[j + 1];
                ps->msPatArray[j + 1] = ps->msPatArray[j];
                ps->msPatArray[j]     = tmp_struct;
            }
        }
    }
}

void wm_wrapper::wmPrepHashedPatternGroups(WM_STRUCT *ps)//�����ж��ٸ���ͬ��ϣֵ���Ҵ�С����
{
    unsigned sindex, hindex, ningroup;
    int i;
    int m = ps->msSmallest;
    ps->msNumHashEntries = HASHTABLESIZE;
    ps->msHash = (HASH_TYPE*) malloc(sizeof(HASH_TYPE) * ps->msNumHashEntries);
    if (!ps->msHash)
    {
        printf("No memory in wmPrepHashedPatternGroups()\n");
        return;
    }

    for (i = 0; i < (int) ps->msNumHashEntries; i++)
    {
        ps->msHash[i] = (HASH_TYPE) -1;
    }

    for (i = 0; i < ps->msNumPatterns; i++)
    {
        hindex = HASH16(&ps->msPatArray[i].psPat[m - 2]);
        sindex = ps->msHash[hindex] = i;
        ningroup = 1;
        while ((++i < ps->msNumPatterns) && (hindex == HASH16(
            &ps->msPatArray[i].psPat[m - 2])))
            ningroup++;
        ps->msNumArray[sindex] = ningroup;
        i--;
    }
}

void wm_wrapper::wmPrepShiftTable(WM_STRUCT *ps)//����shift��
{
    int i;
    unsigned short m, k, cindex;
    unsigned shift;
    m = (unsigned short) ps->msSmallest;
    ps->msShift = (unsigned char*) malloc(SHIFTTABLESIZE * sizeof(char));
    if (!ps->msShift)
        return;

    for (i = 0; i < SHIFTTABLESIZE; i++)
    {
        ps->msShift[i] = (unsigned) (m - 2 + 1);
    }

    for (i = 0; i < ps->msNumPatterns; i++)
    {
        for (k = 0; k < m - 1; k++)
        {
            shift = (unsigned short) (m - 2 - k);
            cindex = ((ps->msPatArray[i].psPat[k] << 8)
                | (ps->msPatArray[i].psPat[k + 1]));//BΪ2
            if (shift < ps->msShift[cindex])
                ps->msShift[cindex] = shift;//k=m-2ʱ��shift=0��
        }
    }
}

void wm_wrapper::wmPrepPrefixTable(WM_STRUCT *ps)//����Prefix��
{
    int i;
    ps->msPrefix = (HASH_TYPE*) malloc(sizeof(HASH_TYPE) * ps->msNumPatterns);
    if (!ps->msPrefix)
    {
        printf("No memory in wmPrepPrefixTable()\n");
        return;
    }
    for (i = 0; i < ps->msNumPatterns; i++)
    {
        ps->msPrefix[i] = HASH16(ps->msPatArray[i].psPat);
    }
}

void wm_wrapper::wmGroupMatch(void* search_data,
                              WM_STRUCT *ps,//��׺��ϣֵ��ͬ���Ƚ�ǰ׺�Լ������ַ�ƥ��
                              int lindex, 
                              unsigned char *Tx, 
                              unsigned char *T)
{
    WM_PATTERN_STRUCT *patrn;
    WM_PATTERN_STRUCT *patrnEnd;
    int text_prefix;
    unsigned char *px, *qx;

    patrn = &ps->msPatArray[lindex];
    patrnEnd = patrn + ps->msNumArray[lindex];
    text_prefix = HASH16(T);

    for (; patrn < patrnEnd; patrn++)
    {
        if (ps->msPrefix[lindex++] != text_prefix)
        {
            continue;
        }
        else
        {
            px = patrn->psPat;
            qx = T;
            while (*(px++) == *(qx++) && *(qx - 1) != '\0')
                ;

            if (*(px - 1) == '\0')
            {
                //printf("Match pattern \"%s\" at line %d column %d\n",
                //    patrn->psPat, m_nline, T - Tx + 1);
                
                size_t index = T - Tx;
                
                matched_patten match_patn;
                match_patn.pstr  = (char*)patrn->psPat;
                match_patn.len   = patrn->psLen;

                int ret = Match (search_data, patrn->user_data, &match_patn, index);
                if (ret > 0)
                {
                    return;
                }
            }
        }
    }
}

int wm_wrapper::wmPrepPatterns(WM_STRUCT *ps)//��plist�õ�msPatArray
{
    int kk;
    WM_PATTERN_STRUCT *plist;

    ps->msPatArray = (WM_PATTERN_STRUCT*) malloc(sizeof(WM_PATTERN_STRUCT)
        * ps->msNumPatterns);
    if (!ps->msPatArray)
        return -1;
    ps->msNumArray
        = (unsigned short*) malloc(sizeof(short) * ps->msNumPatterns);
    if (!ps->msNumArray)
        return -1;

    for (kk = 0, plist = ps->plist; plist != NULL && kk < ps->msNumPatterns; plist
        = plist->next)
    {
        memcpy(&ps->msPatArray[kk++], plist, sizeof(WM_PATTERN_STRUCT));
    }
    sort(ps);
    wmPrepHashedPatternGroups(ps);
    wmPrepShiftTable(ps);
    wmPrepPrefixTable(ps);
    return 0;
}

bool wm_wrapper::wmSearch(WM_STRUCT *ps, void* search_data, unsigned char *Tx, int n)//�ַ�������
{
    int Tleft, lindex, tshift;
    unsigned char *T, *Tend, *window;
    Tleft = n;
    Tend = Tx + n;
    if (n < ps->msSmallest)
        return false;

    for (T = Tx, window = Tx + ps->msSmallest - 1; window < Tend; T++, window++, Tleft--)
    {
        tshift = ps->msShift[(*(window - 1) << 8) | *window];
        while (tshift)
        {
            window += tshift;
            T += tshift;
            Tleft -= tshift;
            if (window > Tend)
                return false;
            tshift = ps->msShift[(*(window - 1) << 8) | *window];
        }
        if ((lindex = ps->msHash[(*(window - 1) << 8) | *window])
            == (HASH_TYPE) -1)
            continue;
        lindex = ps->msHash[(*(window - 1) << 8) | *window];
        wmGroupMatch(search_data, ps, lindex, Tx, T);
    }

    return true;
}

int wm_wrapper::Match(
    void* p,
    void * user_data, 
    matched_patten* pattern,
    int  index
    )
{
    wm_wrapper* pThis = (wm_wrapper*) p;
    if (pThis->callback_func_)
    {
        return pThis->callback_func_(pThis->callback_this_,
            user_data,
            pattern,
            index);
    }

    return 0;
}

