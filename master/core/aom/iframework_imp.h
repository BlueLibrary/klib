#ifndef _klib_iframework_imp_h
#define _klib_iframework_imp_h


#pragma warning(disable: 4996)

#include "iframework.h"
#include "../core/singleton.h"

#include <vector>
#include <string.h>
#include <tchar.h>
#include <windows.h>

namespace klib{
namespace aom{



// ���ģ�����Ƶĳ���
#define  MAX_MODULE_NAME_LENGTH 200


// ģ����Ϣ������
struct module_info
{
    imodule*    module_;
    TCHAR       name_[MAX_MODULE_NAME_LENGTH];
    module_info()  {  memset(this, 0, sizeof(*this));  }
};


// AOM���ʵ��
class framework : public iframework
{
protected:
    framework(framework&);
    framework& operator = (const framework&);
    framework()
    {
        m_vecInterfaceInfo.reserve(20);
    }


public:
    ~framework(void) {}

    static iframework* instance()
    {
        static framework _instance;
        return & _instance;
    }

    //----------------------------------------------------------------------
    // Summary:
    //		����ģ��
    // Parameters:
    //		pszModuleFile		-	ģ���·��(���������·��)
    // Returns:
    //		�ɹ�����S_OK��ʧ�ܷ���S_FALSE��
    //----------------------------------------------------------------------
    HRESULT load_module(LPCTSTR pszModuleFile) 
    {
        RegisterObjects_Fn RegisterObjects;
        HMODULE hModule = LoadLibrary(pszModuleFile);
        HRESULT hr = S_FALSE;

        if (hModule)
        {
            RegisterObjects = (RegisterObjects_Fn) GetProcAddress(hModule, "RegisterObjects");
            KLIB_ASSERT(RegisterObjects);
            if (nullptr == RegisterObjects)
            {
                return S_FALSE;
            }
            hr = RegisterObjects(AOM_LIB_VERSION, this);
        }

        return hr;
    }

    // ͨ������������ģ��
    HRESULT load_module(RegisterObjects_Fn RegisterObjects)
    {
        HRESULT hr = S_FALSE;

        if (nullptr == RegisterObjects)
        {
            return hr;
        }

        hr = RegisterObjects(AOM_LIB_VERSION, this);
        return hr;
    }

    //----------------------------------------------------------------------
    // Summary:
    //		ж��ģ�飨�ӿ�����У�
    // Parameters:
    //		pszModuleName		-	
    // Returns:
    //		
    //----------------------------------------------------------------------
    HRESULT unload_module(LPCTSTR pszModuleName)
    {
        KLIB_ASSERT(FALSE);

        return S_OK;
    }

    //----------------------------------------------------------------------
    // Summary:
    //		ע��ģ��(��ģ��ע��)
    // Parameters:
    //		pszModuleName		-	
    //		pModule		-	
    // Returns:
    //		
    //----------------------------------------------------------------------
    HRESULT register_module(LPCTSTR pszModuleName, imodule* pModule)
    {
        //�ӿڿ����ظ����
        KLIB_ASSERT(pModule);
        if (NULL == pModule) {
            return S_FALSE;
        }

        module_info info;
        _tcsncpy(info.name_, pszModuleName, _countof(info.name_));
        info.module_ = (imodule*) pModule;
        m_vecInterfaceInfo.push_back(info);

        ((module_imp*)pModule)->framwork_ = this;
        
        return S_OK;
    }

    //----------------------------------------------------------------------
    // Summary:
    //		��ģ���в��ҽӿ�
    // Parameters:
    //		guid		    -	
    //		pInteface		-	
    //      pModule         -   ָʾ��pModuleģ�鿪ʼ������
    // Returns:
    //		
    //----------------------------------------------------------------------
    imodule* find_next_interface(GUID guid, void** pInteface, imodule* pModule/* = NULL*/) 
    {
        InterfaceInfoVector::const_iterator itr;
        itr = m_vecInterfaceInfo.begin();
        HRESULT hr = S_FALSE;
        imodule* pFindedImodule = NULL;
        BOOL   bHasCheckedModule = (NULL == pModule ? TRUE : FALSE);

        for (; itr != m_vecInterfaceInfo.end(); ++ itr)
        {
            if (!bHasCheckedModule) {
                if (itr->module_ != pModule) {
                    continue;
                }
                bHasCheckedModule = TRUE;
                continue;
            }

            pFindedImodule = itr->module_;
            hr = pFindedImodule->find_interface(guid, (void**) pInteface);
            if (hr == S_OK)
            {
                return pFindedImodule;
            }
        }

        return NULL;
    }

    virtual HRESULT start() 
    {
        InterfaceInfoVector::const_iterator itr;
        itr = m_vecInterfaceInfo.begin();
        for (; itr != m_vecInterfaceInfo.end(); ++ itr)
        {
            itr->module_->on_pre_run();
        }

        itr = m_vecInterfaceInfo.begin();
        for (; itr != m_vecInterfaceInfo.end(); ++ itr)
        {
            if (!itr->module_->on_run())
            {
                this->stop();
                return S_FALSE;
            }
        }

        itr = m_vecInterfaceInfo.begin();
        for (; itr != m_vecInterfaceInfo.end(); ++ itr)
        {
            itr->module_->on_post_run();
        }

        return S_OK;
    }

    virtual HRESULT stop() 
    {
        InterfaceInfoVector::const_iterator itr;
        itr = m_vecInterfaceInfo.begin();
        for (; itr != m_vecInterfaceInfo.end(); ++ itr)
        {
            itr->module_->on_pre_stop();
        }

        itr = m_vecInterfaceInfo.begin();
        for (; itr != m_vecInterfaceInfo.end(); ++ itr)
        {
            itr->module_->on_stop();
        }

        itr = m_vecInterfaceInfo.begin();
        for (; itr != m_vecInterfaceInfo.end(); ++ itr)
        {
            itr->module_->on_post_stop();
        }

        return S_OK;
    }

protected:
    typedef std::vector<module_info>  InterfaceInfoVector;
    InterfaceInfoVector             m_vecInterfaceInfo;
};




}}


#endif
