#pragma once


#include <windows.h>
#include <set>
#include <map>
#include <stdio.h>
#include <iostream>

namespace klib
{
namespace pattern
{
namespace fsm
{

/*
״̬����ʵ��

*/


///< �¼�
class FsmEvent
{
public:
    FsmEvent() {}
    FsmEvent(UINT uEventType) { m_uEventType = uEventType; }

    UINT    m_uEventType;         ///< ��ʾ�¼�����
    UINT64  m_uEventData;         ///< Я���¼�����
};

///< ״̬�ӿ�
class IState
{
public:
    virtual UINT GetStateID() = 0;                      ///< ����״̬��ID���
    virtual void SetStateID(UINT uStateID) = 0;         ///< ����״̬��ID���

    virtual char*  GetStateName() = 0;                  ///< ��ȡ״̬����
    virtual void   SetStateName(char* pszStateName) = 0;   ///< ����״̬����

    virtual void SetParentID(UINT uParentID) = 0;       ///< ���ø�״̬
    virtual UINT GetParentID() = 0;                     ///< ���ø�״̬

    virtual void Enter(IState* s) = 0;                  ///< ����״̬
    virtual void Leave() = 0;                           ///< �뿪״̬
    virtual void OnEvent(FsmEvent* e, UINT& uNewStateID) = 0;                 ///< �����¼�
};

///< ״̬������
class CState : public IState
{
public:
    CState() : m_uParentID(0), m_uStateID(0) { m_szStateName[0] = 0; }
    CState(UINT uStateID) : m_uParentID(0), m_uStateID(uStateID) { m_szStateName[0] = 0; }
    virtual ~CState() {}

    virtual UINT GetStateID() {return m_uStateID;}
    virtual void SetStateID(UINT uStateID) { m_uStateID = uStateID; }

    virtual char*  GetStateName() { return m_szStateName; }
    virtual void   SetStateName(char* pszStateName) 
    { 
        strncpy_s(m_szStateName, _countof(m_szStateName) -1, pszStateName, strlen(pszStateName)); 
    }

    virtual void SetParentID(UINT uParentID) { m_uParentID = uParentID; }
    virtual UINT GetParentID() { return m_uStateID; }

    virtual void Enter(IState* s) {}
    virtual void Leave() {}
    virtual void OnEvent(FsmEvent* e, UINT& uNewStateID) {}

private:
    UINT        m_uParentID;                ///< ��״̬ID
    UINT        m_uStateID;                 ///< ��ǰ״̬ID
    char        m_szStateName[20];          ///< ��ǰ״̬����
};

///< ״̬��
class CFsm
{
public:
    CFsm() : m_pCurState(NULL), m_pExitState(NULL)  {}
    ~CFsm() 
    {
        auto itr = m_StateMap.begin();
        for (; itr  != m_StateMap.end(); ++ itr) 
        {
            delete itr->second;
        }
        m_StateMap.clear();
    }

public:
    ///< ��õ�ǰ��״̬ID
    UINT GetCurStateID() 
    {
        if (m_pCurState) 
        {
            return m_pCurState->GetStateID();
        }

        return ~0;
    }

    ///< ���úͻ�ȡ��ʼ״̬
    void SetInitState(IState* s)  { m_pCurState = s; }
    IState* GetInitState() { return m_pCurState; }

    ///< ���úͻ�ȡ�˳�״̬
    void SetExitState(IState* s) { m_pExitState = s; }
    IState* GetExitState(IState* s) { return m_pExitState; }

    ///< ���״̬
    void AddState(IState* s) { m_StateMap.insert(std::make_pair(s->GetStateID(), s)); }

    ///< ɾ����Ӧ��״̬
    void DelState(UINT uStateID) 
    {
        IState* s = GetState(uStateID);
        if (s) 
        {
            m_StateMap.erase(uStateID);
            delete s;
        }
    }

    ///< ����״̬ID��ȡ״̬
    IState* GetState(UINT uStateID) 
    {
        auto itr = m_StateMap.find(uStateID);
        if (itr != m_StateMap.end()) 
        {
            return itr->second;
        }
        _ASSERT(FALSE);
        return NULL;
    }

    ///< ��ʼ״̬��
    void Start()
    {
        if (m_pCurState) 
        {
            m_pCurState->Enter(m_pCurState);
        }
    }

    ///< ֹͣ״̬��
    void Stop()
    {
        if (m_pCurState && m_pCurState == m_pExitState) 
        {
            m_pCurState->Leave();
        }
    }

    ///< �ı�״̬
    void ChangeState(UINT uStateID)
    {
        if (m_pCurState) 
        {
            if (uStateID != m_pCurState->GetStateID()) 
            {
                IState* pOldState = m_pCurState;

                m_pCurState->Leave();
                m_pCurState = GetState(uStateID);
                m_pCurState->Enter(pOldState);
            }
        }
    }

public:
    void OnEvent(FsmEvent* e)
    {
        if (m_pCurState) 
        {
            UINT uNewStateID = m_pCurState->GetStateID();
            m_pCurState->OnEvent(e, uNewStateID);

            if (uNewStateID != m_pCurState->GetStateID()) 
            {
                IState* pOldState = m_pCurState;

                m_pCurState->Leave();
                m_pCurState = GetState(uNewStateID);
                m_pCurState->Enter(pOldState);
            }
        }
    }

protected:
    std::map<UINT, IState*> m_StateMap;   ///< ״̬��

    IState* m_pCurState;                  ///< ��ǰ״ָ̬��
    IState* m_pExitState;
};


//------------------------------------------------------------------------
// ʹ�ú궨�壬����״̬��
#define  BEGIN                                                  \
{

#define   END           };

#define  BEGIN_STATE_DECLARE(State_Name, State_ID)              \
class State_Name : public CState                                \
{                                                               \
    enum {STATE_ID = State_ID};                                 \
    public: State_Name() : CState(State_Name::STATE_ID)         \
    {                                                           \
        this->SetStateName(#State_Name);                        \
    }

///< ��ʼ����OnEvent
#define  BEGIN_ON_EVENT()                                       \
public: void OnEvent(Event* e, UINT& uNewStateID)               \
{

///< ��������OnEvent
#define  END_ON_EVENT                                           \
}

///< ��ʼ����Leave
#define  BEGIN_LEAVE()                                          \
public: void Leave()                                            \
{

///< ��������Leave
#define  END_LEAVE                                              \
}

///< ��ʼ����Enter
#define  BEGIN_ENTER()                                          \
public: void Enter(IState* s)                                   \
{

///< ��������Enter
#define  END_ENTER                                              \
}

///< ��ʼ����fsm
#define  BEGIN_FSM(FSM_Name)                                    \
class FSM_Name : public CFsm                                    \
{                                                               \
public:                                                         \
    FSM_Name()                                                  \
    {                                                           \
        RegisterAllState();                                     \
    }

///< ��ʼע��״̬
#define  BEGIN_REGISTER_STATE()                                 \
void RegisterAllState()                                         \
{                                                               \
    IState* s = NULL;

///< ע��״̬
#define  REGISTER_STATE(State_Name)                             \
    this->AddState(new State_Name);                             \

///< ע���ʼ״̬
#define REGISTER_INIT_STATE(State_Name)                         \
    s = new State_Name;                                         \
    if (s) {                                                    \
        this->AddState(s);                                      \
        this->SetInitState(s);                                  \
    }

///< ע�����״̬
#define REGISTER_EXIT_STATE(State_Name)                         \
    s = new State_Name;                                         \
    this->AddState(s);                                          \
    this->SetExitState(s);

///< ����ע��״̬
#define  END_REGISTER_STATE                                     \
}

///< ����fsm����
#define  END_FSM                                                \
};


/*
///< ״̬���Ķ���
BEGIN_FSM(ClientAppFsm)
    BEGIN_REGISTER_STATE()
        REGISTER_INIT_STATE(CQueryLogicState)
        REGISTER_STATE(CQueryNewVerState)
        REGISTER_STATE(COnlineState)
    END_REGISTER_STATE
END

ClientAppFsm    m_fsm;
*/


}

}}