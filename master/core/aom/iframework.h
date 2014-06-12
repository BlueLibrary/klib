#ifndef _klib_iframework_h
#define _klib_iframework_h


namespace klib {
namespace aom {



/* AOM��ܵİ汾�� */
#define   AOM_LIB_VERSION  (1<<4)

#define   KLIB_ASSERT(expression)                             \
        _ASSERT(expression)

/* ע��ģ��궨�� */
#define  BEGIN_REGISTER_MODULES     \
    EXTERN_C                                                                    \
    {                                                                           \
        _declspec(dllexport)                                                    \
        HRESULT RegisterObjects(UINT uLibVersion, klib::aom::iframework* pIServer)      \
        {                                                                       \
            if (uLibVersion != AOM_LIB_VERSION)                                 \
            {                                                                   \
                return S_FALSE;                                                 \
            }

#define REGISTER_MODULE(ModuleName)   \
            pIServer->register_module(TEXT(#ModuleName), new ModuleName);

#define END_REGISTER_MODULES                                                    \
            return S_OK;                                                        \
        }                                                                       \
    };


/* ģ���ڲ��Ľӿ����� */
#define  BEGIN_INTERFACE_ENTRY                                          \
    HRESULT find_interface(GUID guid, void** pPointer)                  \
    {

#define INTERFACE_ENTRY(_theGUID, _theINTERFACE)                        \
    if (guid == _theGUID)                                               \
    {                                                                   \
        if (pPointer)                                                   \
            *pPointer = (_theINTERFACE*) this;                          \
        return S_OK;                                                    \
    }

#define END_INTERFACE_ENTRY                                             \
        return S_FALSE;                                                 \
    }

/* ģ��ӿڶ��� */
struct iframework;
struct imodule
{
    virtual iframework* frame() = 0;

    /**
    * @brief ���������¼�֮׼������
    * 
    *     ����ģ�����������¼��в�ѯ������ģ��ͽӿڣ���ʼ��ϵͳ����
    */
    virtual void on_pre_run() = 0;

    /**
    * @brief   ���������¼�֮����
    *
    *   	���������еı�����Դ���ɹ����򷵻�FALSE�����򷵻�TRUE
    *
    *		�������ʧ�ܣ�һ��Ҫ����GetEngineServer()->SetLastErrorText()�������ô���ԭ��
    *       ����ģ�����������¼�������ϵͳ��Դ�������̣߳���������Ľӿڵȡ�
    */
    virtual BOOL on_run() = 0;

    /**
    * @brief  ���������¼�֮����֮��
    *
    *     ����ģ�����������¼��н������ݵ�������紴����ʱ��...
    */
    virtual void on_post_run() = 0;

    /**
    * @brief   ���������¼�֮׼��ֹͣ
    *
    *     ����ģ�����������¼�����׼��ֹͣ�Ĳ�������������߳��¼��źţ�ֹͣ��ʱ��
    */
    virtual void on_pre_stop() = 0;

    /**
    * @brief  ���������¼�ֹ֮ͣ
    *
    *     ����ģ�����������¼��н����߳�
    */
    virtual void on_stop() = 0;

    /**
    * @brief    ���������¼�ֹ֮֮ͣ��
    *
    *     ����ģ�����������¼����ͷ�ϵͳ��Դ
    */
    virtual void on_post_stop() = 0;

    /**
    * @brief   ���ݽӿ�GUID������Ӧ�Ľӿ�
    *
    *		guid		-	
    *		pInterface		-	
    * Returns:
    *		
    */
    virtual HRESULT find_interface(GUID guid, void** pInterface) = 0;
};

class module_imp : public imodule
{
public:
    module_imp() : framwork_(NULL) {}
    iframework* frame()
    {
        return framwork_;
    }

    virtual void on_pre_run()
    {
    }
    virtual BOOL on_run()
    {
        return TRUE;
    }
    virtual void on_post_run()
    {
    }
    virtual void on_pre_stop()
    {
    }
    virtual void on_stop()
    {
    }
    virtual void on_post_stop()
    {
    }

public:
    iframework* framwork_;
};

typedef HRESULT (*RegisterObjects_Fn)(UINT uLibVersion, iframework* pIAomServer) ;

/**  AOM��ܷ���ӿ�  */
struct iframework
{
public:
    // ����ģ��
    virtual HRESULT load_module(LPCTSTR pszModuleFile) = 0;

    // ж��ģ��
    virtual HRESULT unload_module(LPCTSTR pszModuleName) = 0;

    // ע��ģ��
    virtual HRESULT register_module(LPCTSTR pszModuleName, imodule* pModule) = 0;

    // ���ҽӿڹ���
    virtual imodule* find_next_interface(GUID guid, void** pInteface, imodule* pModule = NULL) = 0;

    // ��������
    virtual HRESULT start() = 0;

    // ֹͣ����
    virtual HRESULT stop() = 0;
};



}}


#endif
