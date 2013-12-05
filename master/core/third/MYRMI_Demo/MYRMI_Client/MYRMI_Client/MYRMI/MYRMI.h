
#ifndef MYRMI_WANG_SHU_DONG_2008_01_08
#define MYRMI_WANG_SHU_DONG_2008_01_08


#pragma comment(lib,"wsock32.lib ")

#include "stdio.h"
#include <string>
#include "winsock2.h"
#include <windows.h>//�ź������Լ��߳̿��ƺ���
#include <list>
#include <map>

//�����Ǹ��ֳ�������
#define MAXBUFFERLENGTH 2048//��������󳤶ȣ���RemoteFunctionStu(Զ�̷����ĺ������)����󳤶�
#define SERVERPORT 1110     //������Ĭ�϶˿ں�
#define MAXPARAMETERCOUNT 10     //������������
#define MAX_CLIENT_COUNT 10		//һ����������ͬʱ��������ͻ�����Ŀ
#define MAX_FUN_LENGHT 1024		//����id����󳤶�


//�����Ǹ����ڲ����ݵĶ���
class MYIDENTIFYCLASS{};   //ģ����Ĭ�ϵĲ������ͣ�����ݴ��ж�������Ĳ��������Լ���Ŀ
class MYVOIDCLASS{};		//�����޷���ֵʱ��Ĭ�����͡����ģ���еĲ����Ǵ����ͣ���˵�������޷���ֵ��һ����������Ӧ����

static MYIDENTIFYCLASS mDefaultPara; //ȫ�ֱ������ڳ�ʼ��Ĭ�ϵ�ģ�����---FunctionObjectģ������Ҫ


//�������   
struct RemoteFunctionStub{
	std::string mFunctionID;		//����ID��ÿ����������һ��Ψһ��ID��ʶ�˺�����ϵͳ�ݴ�ID�жϵ����ĸ�����
	int mParaCount;					//�����Ĳ�����Ŀ
	void* mReturnValue;				//����ֵ��ŵ�λ��
	int mReturnValueLength;			//����ֵ���͵ĳ���
	int mStructLength;				//�ڲ��������͵ĳ��ȣ���ʾ������������󳤶�
	void* pPara;					//���������в������������ʽ�����˴�
} ;


//�����̣߳�ListenThread�����Լ��������̣߳�ProcessRequest������Ҫ�Ĳ����ṹ 
class RMIClientBase;		//�ͻ��˶˻��ࡣ�����ж���
class RMIServerBase;		//�������˻��ࡣ�����ж���
struct THREADPARA{
	SOCKET mSocket;							//socket����
	RMIServerBase* pRMIServerBase;			//�������˻���ָ��
};


//���֡��������á���

//FunctionObjectģ������Ҫ�ĺ�
#define CHECKPARA(Para) \
	if(typeid(Para).name()[strlen(typeid(Para).name())]=='*'||              \
		typeid(Para).name()[strlen(typeid(Para).name())]==')')				\
	{																		\
		strcpy(mParaException,"���󣺲����а���ָ���������\r\n");			\
		throw mParaException;												\
		return tError;														\
	}																	

//RMIServerBase��Ҫ�ĺ�
#define TEST_RETURE_VALUE()\
			if(nRes<0)\
			{\
				WaitForSingleObject(mThreadPara.pRMIServerBase->mSemaphoreThreadList,5000);\
				printf("%d�߳��˳�\r\n",mThreadPara.pRMIServerBase->mClientThreadMap[mThreadPara.mSocket]);\
				mThreadPara.pRMIServerBase->mClientThreadMap.erase(mThreadPara.mSocket);\
				mThreadPara.pRMIServerBase->mClientCount--;\
				ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);\
				closesocket(mThreadPara.mSocket);\
				ExitThread(1);\
				return 0;\
			}




//����������Ҫ��ģ����
//��������ģ��
template<
typename ReturnValue, 
typename P1 =MYIDENTIFYCLASS, 
typename P2 =MYIDENTIFYCLASS, 
typename P3 =MYIDENTIFYCLASS, 
typename P4 =MYIDENTIFYCLASS, 
typename P5 =MYIDENTIFYCLASS, 
typename P6 =MYIDENTIFYCLASS, 
typename P7 =MYIDENTIFYCLASS, 
typename P8 =MYIDENTIFYCLASS, 
typename P9 =MYIDENTIFYCLASS>

class ParaListAnalyser{
public:
	ParaListAnalyser(void* tp)
	{///���캯����Ӧ���ж�������Ĳ����Ƿ����ָ���������
		pValue=tp;
	};
	std::string GetReturnValueType(){
		std::string tstr;
		tstr=typeid(ReturnValue).name();

	}
	P1 GetP1()
	{
		return ((UNIONTYPE*)pValue)[1].a1;
	}
	P2 GetP2()
	{
		return ((UNIONTYPE*)pValue)[2].a2;
	}
	P3 GetP3()
	{
		return ((UNIONTYPE*)pValue)[3].a3;
	}
	P4 GetP4()
	{
		return ((UNIONTYPE*)pValue)[4].a4;;
	}
	P5 GetP5()
	{
		return ((UNIONTYPE*)pValue)[5].a5;
	}
	P6 GetP6()
	{
		return ((UNIONTYPE*)pValue)[6].a6;
	}
	P7 GetP7()
	{
		return ((UNIONTYPE*)pValue)[7].a7;
	}
	P8 GetP8()
	{
		return ((UNIONTYPE*)pValue)[8].a8;
	}
	P9 GetP9()
	{
		return ((UNIONTYPE*)pValue)[9].a9;
	}
private:

	union UNIONTYPE{
		ReturnValue mReturnValue;
		P1 a1;
		P2 a2;
		P3 a3;
		P4 a4;
		P5 a5;
		P6 a6;
		P7 a7;
		P8 a8;
		P9 a9;
	};
//	UNIONTYPE mTypeList[MAXPARAMETERCOUNT];
	////����Ϊ�����ж����õ��ı���
	void* pValue;
};



//������clientBase����Ҫ��ģ��֧��

template<
typename ReturnValue, 
typename P1 =MYIDENTIFYCLASS, 
typename P2 =MYIDENTIFYCLASS, 
typename P3 =MYIDENTIFYCLASS, 
typename P4 =MYIDENTIFYCLASS, 
typename P5 =MYIDENTIFYCLASS, 
typename P6 =MYIDENTIFYCLASS, 
typename P7 =MYIDENTIFYCLASS, 
typename P8 =MYIDENTIFYCLASS, 
typename P9 =MYIDENTIFYCLASS>

class FunctionObject{
public:
	FunctionObject()
	{///���캯����Ӧ���ж�������Ĳ����Ƿ����ָ���������

		char targetstr[]="class MYIDENTIFYCLASS";

		mTypeList[0].mType=typeid(ReturnValue).name();
		mTypeList[1].mType=typeid(P1).name();
		mTypeList[2].mType=typeid(P2).name();
		mTypeList[3].mType=typeid(P3).name();
		mTypeList[4].mType=typeid(P4).name();
		mTypeList[5].mType=typeid(P5).name();
		mTypeList[6].mType=typeid(P6).name();
		mTypeList[7].mType=typeid(P7).name();
		mTypeList[8].mType=typeid(P8).name();
		mTypeList[9].mType=typeid(P9).name();

///�������ʵ������Ŀ
		for(mParaCount=0;mParaCount<MAXPARAMETERCOUNT&&
			!(strcmp(mTypeList[mParaCount].mType.c_str(),targetstr)==0);mParaCount++){}
		

	};
	ReturnValue operator() (const char* tRomateFunction,RMIClientBase* pClient ,P1 p1=mDefaultPara,P2 p2=mDefaultPara,P3 p3=mDefaultPara
		,P4 p4=mDefaultPara,P5 p5=mDefaultPara,P6 p6=mDefaultPara
		,P7 p7=mDefaultPara,P8 p8=mDefaultPara,P9 p9=mDefaultPara)
	{//�ݲ�����ָ�룡
		
		ReturnValue r;//��ʱ������������ֵ��
		ReturnValue tError;//�����ش����Ƿ��ش�ֵ
		char mParaException[200];
		memset(mParaException,0,200);
	//�����ж������������Ƿ���ָ�룬��������׳��쳣
		CHECKPARA(p1);
		CHECKPARA(p2);
		CHECKPARA(p3);
		CHECKPARA(p4);
		CHECKPARA(p5);
		CHECKPARA(p6);
		CHECKPARA(p7);
		CHECKPARA(p8);
		CHECKPARA(p9);

		
		mUnionList[1].a1=p1;
		mUnionList[2].a2=p2;
		mUnionList[3].a3=p3;
		mUnionList[4].a4=p4;
		mUnionList[5].a5=p5;
		mUnionList[6].a6=p6;
		mUnionList[7].a7=p7;
		mUnionList[8].a8=p8;
		mUnionList[9].a9=p9;
		//�����װ����stub��Ȼ����÷��ͺ�������stub���ͳ�ȥ
		mRFStub.mFunctionID=tRomateFunction;
		mRFStub.mReturnValueLength=sizeof(ReturnValue);
		if(strcmp(typeid(ReturnValue).name(),typeid(MYVOIDCLASS).name())==0)
		{//�޷���ֵ
			mRFStub.mReturnValue=NULL;
		}
		else
		{//�з���ֵ
			mRFStub.mReturnValue=(void*)&r;
		}
		mRFStub.mStructLength=sizeof(UNIONTYPE)*(mParaCount-1);
		mRFStub.pPara=(void*)mUnionList;
		mRFStub.mParaCount=mParaCount;
		//
		if(pClient->CallRemoteFunction(&mRFStub))
			return r;
		else
			return tError;
	}
private:
	int mParaCount;
//�ڲ��ṹ������ͨ�ò�������
	union UNIONTYPE{
		ReturnValue mReturnValue;
		P1 a1;
		P2 a2;
		P3 a3;
		P4 a4;
		P5 a5;
		P6 a6;
		P7 a7;
		P8 a8;
		P9 a9;
	};
	struct TYPEINFOR{
		UNIONTYPE mUnionType;
		std::string mType;
	};
	TYPEINFOR mTypeList[MAXPARAMETERCOUNT];
	UNIONTYPE mUnionList[MAXPARAMETERCOUNT];
	////����Ϊ�����ж����õ��ı���
	RemoteFunctionStub mRFStub;

};


//������ͨѶ����������Ҫ�Ļ������͵Ķ���

//�������˻���    ���������socketͨѶ����
//�����ڽӵ�stop����󣬼򵥵ĵ���TerminateThread���������пͻ��˵������̹߳رգ���Ƿ���ǣ�
// Ӧѡ��һ����ʵ��Ч�Ļ��Ʊ����ڿͻ��˵��ú���ʱ��ȷ������������������������û�����һ����Ϣ
class RMIServerBase
{
public:
	RMIServerBase();			//
	~RMIServerBase();			//

	bool Listen(int tPort);//��ʼ�������˿���tPortָ��
		
	bool SendResponse(void* pV,int L,SOCKET tSocket);//��tSocket������pVָ���ĳ�����L������	

	//�麯�� ���Ա�����ʵ�֡����ñ��ط�����pFunID������ID��pParaList��������tParaListLengt�������б�ĳ���
	virtual	void CallLocalFunction(const char* pFuncID, void* pParaList,int tParaListLenght,SOCKET tSocket)=0;
	bool Stop();//ֹͣ����
private:
	
	static DWORD WINAPI ProcessRequest(LPVOID pPara);//���յ�һ�����������������߳�
	
	static DWORD WINAPI ListenThread(LPVOID pPara);//�����߳�;


private:
	bool mConnected;//��־��ǰ�Ƿ��ڼ���״̬
	int mServerPort;//����˿�
	
	WORD wVersionRequested;//winsocket�汾 = MAKEWORD(1,1);
	WSADATA wsaData;//winsocket������Ϣ
	int nRet;//
	SOCKET	mLocalSocket;	//���ؼ���socket
	sockaddr_in mLocalAddr;//���ص�ַ
	
	int mClientCount;//�ͻ���������Ŀ
	HANDLE mSemaphoreThreadList;//�ź�����������ʡ��������߳��б�mClientThreadMap��
	HANDLE mSemaphoreConnect;//�ź���:������ʣ�mClientCount;
	HANDLE mListenThreadHandle;//�����̵߳��߳̾��
	std::map<SOCKET,HANDLE> mClientThreadMap; //�������߳��б������������ڴ���������߳�handle
	THREADPARA ThreadParaListenThread;//
	THREADPARA ThreadParaProcessRequest;//
	HANDLE hThreadRequest[MAX_CLIENT_COUNT];    //�������洦��ÿ���ͻ���������߳̾��
	///int 
public:
	int nError;//�������
	
};	


//�ͻ��˻���
class RMIClientBase
{
public:
	RMIClientBase();
	~RMIClientBase();
	bool Connect(char* tRomateIP,int tRomatePort);
	void StopConnect();
	bool IsAvailable();
	bool CallRemoteFunction(RemoteFunctionStub* tRFStub);
private:
	bool SendMessage(void* pValue,int tSendLength);
	bool mConnected;
	int mPort;//�������˿�
	std::string mRemoteIP;
	
	WORD wVersionRequested;
	WSADATA wsaData;
	int nRet,nError;
	sockaddr_in mRemoteAddr;
	
	SOCKET mRemoteSocket;
	HANDLE mHandleStopEvent;
	HANDLE mHandleSendMessageEvent;
	HANDLE mHandleListenThread;
	
};


#endif