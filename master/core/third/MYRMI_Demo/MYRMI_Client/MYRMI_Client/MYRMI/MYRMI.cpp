
#include "MYRMI.h"


//�����ڽӵ�stop����󣬼򵥵ĵ���TerminateThread���������пͻ��˵������̹߳رգ���Ƿ���ǣ�
// Ӧѡ��һ����ʵ��Ч�Ļ��Ʊ����ڿͻ��˵��ú���ʱ��ȷ������������������������û�����һ����Ϣ
RMIServerBase::RMIServerBase()
{
	mConnected=false;
	mServerPort=SERVERPORT;
	mClientCount=0;
	mSemaphoreThreadList=CreateSemaphore(NULL,1,10,NULL);
	mSemaphoreConnect=CreateSemaphore(NULL,1,10,NULL);
	mListenThreadHandle = NULL ;
	for( int i = 0 ; i< MAX_CLIENT_COUNT ; i++)
	{
		hThreadRequest[i] = NULL;
	}
}
RMIServerBase::~RMIServerBase()
{
	
}
bool RMIServerBase::SendResponse(void* pV,int L,SOCKET tSocket){
	if(send(tSocket,(char*)pV,L,0)==L)
		return true;
	else
		return false;
	
}
bool RMIServerBase::Listen(int tPort)
{
	mServerPort=tPort;
	if(mConnected)
	{//�жϵ�ǰ�Ƿ���������״̬
		return false;
	}
	wVersionRequested= MAKEWORD(1,1);
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet)
	{//winsocket��ʼ������
		WSACleanup();
		return false;
	}
	
	//
	// Check WinSock version
	//
	if (wsaData.wVersion != wVersionRequested)
	{//winsock�汾����
		WSACleanup();
		return false;
	}
	
	mLocalSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mLocalSocket == INVALID_SOCKET)
	{//����soket�ṹ�����
		return false;
	}
	
	//���洴�����ؼ���
	mLocalAddr.sin_family=AF_INET;
	mLocalAddr.sin_port=htons(mServerPort);
	mLocalAddr.sin_addr.S_un.S_addr=INADDR_ANY;
	if(bind(mLocalSocket,(LPSOCKADDR)&mLocalAddr,sizeof(mLocalAddr))==SOCKET_ERROR)
	{//�����󶨴���
		nError=WSAGetLastError();
		return false;
	}
	if(listen(mLocalSocket,SOMAXCONN)!=0)
	{
		nError=WSAGetLastError();
		return false;
	}
	
	
	//���ȹ�������̵߳Ĳ����ṹ
	//THREADPARA tThreadPara ;
	//tThreadPara.mSocket = mLocalSocket;
	//tThreadPara.pRMIServerBase = this;
	ThreadParaListenThread.mSocket=mLocalSocket;
	ThreadParaListenThread.pRMIServerBase=this;
	
	
	mListenThreadHandle=CreateThread(NULL,0,ListenThread,(LPVOID )&ThreadParaListenThread,0,NULL);
	
	if(mListenThreadHandle==NULL)
	{
		nError=GetLastError();
		return false;
	}
	
	//�����������״̬ 
	mConnected=true;
	return true;
}

bool RMIServerBase::Stop()
{//�п��ܽ������ɹ�
	if(WaitForSingleObject(mSemaphoreConnect,5000)==WAIT_TIMEOUT)
		return false;
	if(mConnected)
	{//�������У����߳��б��е������߳�ȫ��kill����

		mConnected =  false ;

		ReleaseSemaphore(mSemaphoreConnect,1,NULL);
	
		//�رռ����߳���������socket
		//�رմ�socket�󣬼����߳��Զ��˳�
		closesocket(mLocalSocket);
		
		mListenThreadHandle=NULL;


		//����ر������Ĵ����߳�

		if(WaitForSingleObject(mSemaphoreThreadList,5000)==WAIT_TIMEOUT)
		{
			return false;
		}
		
		std::map<SOCKET,HANDLE>::iterator mapIterator;
		
		//�ر����д򿪵�socket
		for(mapIterator=mClientThreadMap.begin();mapIterator!=mClientThreadMap.end();mapIterator++)
		{
			closesocket(mapIterator->first);//���ȹرտͻ����߳������е�socket
// 			if(WaitForSingleObject(mapIterator->second,3000)==WAIT_TIMEOUT)//�ȴ��߳̽���
// 			{
// 				
// 				TerminateThread(mapIterator->second,0);//��ʱ��ǿ�н����߳�
// 				printf("����TerminateThread�ر��̣߳�\r\n");
// 			}
			
		}
		ReleaseSemaphore(mSemaphoreThreadList,1,NULL);//�ͷ��ź���
		
		WSACleanup();
		return true;
		
	}
	else{
		//û�д�������״̬
		ReleaseSemaphore(mSemaphoreConnect,1,NULL);
		WSACleanup();
		return true;
	}
}

DWORD WINAPI RMIServerBase::ProcessRequest(LPVOID pPara)
{
	
	char pFunID[MAX_FUN_LENGHT];//Զ�̷���ID
	int mParaCount;//�����õķ���������Ŀ
	int mUnionLength;//ÿ����������Ԫ���ĳ���
	int nRes;//socket���ܺ����ķ����ֽ�����
	int addrL=sizeof(sockaddr);//socket����ʱ��Ҫ�ĵ�ַ���ͳ���
	THREADPARA mThreadPara=*((THREADPARA*)pPara);//��ȡ�߳�����Ҫ�ĸ��ֲ���
	char *pParaList;
	int mFunIDLength;
	
	//������Ҫ���õĺ���ID
	while(true)//ѭ������-��������
	{
		///���Ƚ��ܺ���ID�ĳ���
		nRes=recv(mThreadPara.mSocket,(char*)&mFunIDLength,sizeof(int),0);
		TEST_RETURE_VALUE()
					
		memset(pFunID,0,mFunIDLength+1);
		//���ܺ���ID
		if(mFunIDLength>MAX_FUN_LENGHT)
		{//����ID�����涨����
			
			WaitForSingleObject(mThreadPara.pRMIServerBase->mSemaphoreThreadList,5000);
			printf("%d�߳��˳�\r\n",mThreadPara.pRMIServerBase->mClientThreadMap[mThreadPara.mSocket]);
			mThreadPara.pRMIServerBase->mClientThreadMap.erase(mThreadPara.mSocket);
			mThreadPara.pRMIServerBase->mClientCount--;
			ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);
			closesocket(mThreadPara.mSocket);
			ExitThread(1);
			return 0;
		}
		nRes=recv(mThreadPara.mSocket,pFunID,mFunIDLength,0);
		TEST_RETURE_VALUE()
			pFunID[nRes]='\0';
		
		
		
		//������ܲ�����Ŀ
		nRes=::recv(mThreadPara.mSocket,(char*)&mParaCount,sizeof(mParaCount),0);
		TEST_RETURE_VALUE()
			
			
			
		//������ܡ����ϡ����ͳ���
		nRes=::recv(mThreadPara.mSocket,(char*)&mUnionLength,sizeof(mUnionLength),0);
		TEST_RETURE_VALUE()
			
			
		//������ܲ�����
		pParaList=new char[mParaCount*mUnionLength];
		memset(pParaList,0,mParaCount*mUnionLength);
		if(mParaCount*mUnionLength!=0)
		{//ֻ���в�����ʱ��Ž���
			nRes=::recv(mThreadPara.mSocket,pParaList,mParaCount*mUnionLength,0);
			if(nRes<0)
			{
				WaitForSingleObject(mThreadPara.pRMIServerBase->mSemaphoreThreadList,5000);
				printf("%d�߳��˳�\r\n",mThreadPara.pRMIServerBase->mClientThreadMap[mThreadPara.mSocket]);
				mThreadPara.pRMIServerBase->mClientThreadMap.erase(mThreadPara.mSocket);
				mThreadPara.pRMIServerBase->mClientCount--;
				ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);
				closesocket(mThreadPara.mSocket);

				//�˴���ǰ��TEST_RETURE_VALUE����ͬ������һ��ͷŴ洢�ռ䡱
				delete pParaList;

				ExitThread(1);
				return 0;
			}
		}
		
		//���ñ��ط���
		mThreadPara.pRMIServerBase->CallLocalFunction(pFunID,(void*)pParaList,mUnionLength,mThreadPara.mSocket);
		delete pParaList;
	}

	WaitForSingleObject(mThreadPara.pRMIServerBase->mSemaphoreThreadList,5000);

	mThreadPara.pRMIServerBase->mClientThreadMap.erase(mThreadPara.mSocket);
	mThreadPara.pRMIServerBase->mClientCount--;

	ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);

	closesocket(mThreadPara.mSocket);

	ExitThread(1);

	return 0;
}

DWORD WINAPI RMIServerBase::ListenThread(LPVOID pPara)//�����߳�
{ 
	
	
	THREADPARA mThreadPara=*((THREADPARA*)pPara);//��ȡ�߳�����Ҫ�ĸ��ֲ���
	sockaddr_in tRemoteAddr;
	SOCKET tNewSocket;
	int nAddrLength=sizeof(sockaddr_in);

	while(true)
	{
		
		tNewSocket=accept(mThreadPara.mSocket,(sockaddr*)&tRemoteAddr,&nAddrLength);

		WaitForSingleObject(mThreadPara.pRMIServerBase->mSemaphoreThreadList,5000);	
		
		if(tNewSocket==INVALID_SOCKET)
		{
	
			if ( ! mThreadPara.pRMIServerBase->mConnected )
			{//���������ӡ���־�����ã�������Ϊ�˳�
				mThreadPara.pRMIServerBase->mListenThreadHandle  = NULL;
				ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);
				return 0 ;
			}
			else
			{
				ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);
				continue;
			}
		}
		
		//�ж�������Ŀ�Ƿ񳬹����������Ŀ
		if(mThreadPara.pRMIServerBase->mClientCount>=MAX_CLIENT_COUNT)
		{
			closesocket(tNewSocket);
			ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);
			continue;
		}
	
		
		//û�г��������Ŀ���ơ�����һ�������̴߳�����ܵ�������
		//�Ȳ���һ�����õ��߳̾�������ڱ��漴�����ɵ��������߳�
		
// 		for ( int i = 0 ; i < MAX_CLIENT_COUNT ; i ++ )
// 		{
// 			if ( mThreadPara.pRMIServerBase->hThreadRequest[i] != NULL )
// 			{
// 				break;
// 			}
// 		}
// 
// 		if ( i >= MAX_CLIENT_COUNT )
// 		{//���ҿ��õľ��ʧ��
// 			closesocket(tNewSocket);
// 			ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);
// 			continue;
// 
// 		}

		mThreadPara.pRMIServerBase->ThreadParaProcessRequest.mSocket=tNewSocket;

		mThreadPara.pRMIServerBase->ThreadParaProcessRequest.pRMIServerBase = mThreadPara.pRMIServerBase;
		

		HANDLE tHandle=CreateThread(NULL,0,ProcessRequest,(LPVOID )&mThreadPara.pRMIServerBase->ThreadParaProcessRequest,0,NULL);

		if(tHandle==NULL)
		{
			mThreadPara.pRMIServerBase->nError = GetLastError();
			continue;
		}
		
		
		//�����߳���Ŀ 

		mThreadPara.pRMIServerBase->mClientThreadMap.insert(std::pair<SOCKET,HANDLE>(tNewSocket,tHandle));
		mThreadPara.pRMIServerBase->mClientCount++;
		ReleaseSemaphore(mThreadPara.pRMIServerBase->mSemaphoreThreadList,1,NULL);
		
	}
	
	return 0;
}





//////////////////////////////////////////////////////////////////////////
//������RMIClientBase�ľ���ʵ��

RMIClientBase::RMIClientBase(){
	mConnected=false;
}
RMIClientBase::~RMIClientBase()
{
	StopConnect();
}
bool RMIClientBase::Connect(char* tRomateIP,int tRomatePort)
{
	mRemoteIP=tRomateIP;
	mPort=tRomatePort;
	if(mConnected)
	{//�жϵ�ǰ�Ƿ���������״̬
		return false;
	}
	wVersionRequested= MAKEWORD(1,1);
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet)
	{//winsocket��ʼ������
		WSACleanup();
		return false;
	}
	
	//
	// Check WinSock version
	//
	if (wsaData.wVersion != wVersionRequested)
	{//winsock�汾����
		WSACleanup();
		return false;
	}
	
	mRemoteSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mRemoteSocket == INVALID_SOCKET)
	{//����soket�ṹ�����
		WSACleanup();
		return false;
	}
	
	//����Socket��Ϣ
	BOOL sockval;
	sockval = TRUE;

	setsockopt(mRemoteSocket, SOL_SOCKET, SO_KEEPALIVE,(char*) &sockval, sizeof(sockval));

	mRemoteAddr.sin_port = htons(tRomatePort);
	mRemoteAddr.sin_family = AF_INET;
	mRemoteAddr.sin_addr.s_addr=inet_addr(tRomateIP);
	//�����Ǵ����������˵�����;
	if(connect(mRemoteSocket,(LPSOCKADDR)&mRemoteAddr,sizeof(mRemoteAddr))==SOCKET_ERROR)
	{//�����󶨴���
		WSACleanup();
		return false;
	}
	
	mConnected=true;
	return true;
};
//	static DWORD WINAPI ConnectHolder
void RMIClientBase::StopConnect()
{
	if(mConnected)
	{
		closesocket(mRemoteSocket);
	}
	
	WSACleanup();

	mConnected=false;
}
bool RMIClientBase::IsAvailable()
{
	int optValue;
	int optLength = sizeof(optValue);

	getsockopt(mRemoteSocket,SOL_SOCKET,SO_ACCEPTCONN,(char*)&optValue,&optLength) ;
	if(optValue != 1)
	{//û�н�������

	}
	if(WSAGetLastError()==WSAECONNRESET) 
	{//�����ѹر�
	
		StopConnect();
	}
	
	return mConnected;
};
bool RMIClientBase::CallRemoteFunction(RemoteFunctionStub* tRFStub)
{///�˷���Ӧ���ڿ�ͷ�ж����������tcp������Ȼ��Ч 
	
	
	///�ȷ���RemoteFunctionStub��mFunctionID
	
	int nReCount=0;//�����ֽ���
	char* pReBuffer=NULL;//�����ؽ��������˴�
	//���ͺ���ID�ĳ���
	int tStrLength=tRFStub->mFunctionID.length();
	if(!SendMessage((void*)&tStrLength,sizeof(tStrLength)))
		return false;
	//���ͺ���ID
	if(!SendMessage((void*)tRFStub->mFunctionID.c_str(),tRFStub->mFunctionID.length()))
		return false;
	//���Ͳ�����Ŀ
	if(!SendMessage((void*)&(tRFStub->mParaCount),sizeof(tRFStub->mParaCount)))
		return false;
	//���͡����ϡ����ͳ���
	if(!SendMessage((void*)&(tRFStub->mStructLength),sizeof(tRFStub->mStructLength)))
		return false;
	//���Ͳ�����
	if(!SendMessage((void*)(tRFStub->pPara),(tRFStub->mStructLength)*(tRFStub->mParaCount)))
		return false;
	if(tRFStub->mReturnValue==NULL)
	{//�޷���ֵ
		return true;
	}
	else
	{//�з���ֵ
		pReBuffer=new char[tRFStub->mReturnValueLength];
		memset((void*)pReBuffer,0,tRFStub->mReturnValueLength);
		int sockaddrlengh=sizeof(sockaddr);
		nReCount=recvfrom(mRemoteSocket,pReBuffer,tRFStub->mReturnValueLength,0,(sockaddr*)&mRemoteAddr,&sockaddrlengh);
		if(nReCount==tRFStub->mReturnValueLength)
		{//�ɹ����ܵ�
			memcpy(tRFStub->mReturnValue,pReBuffer,nReCount);
			delete pReBuffer;
			return true;
		}
		else
		{
			delete pReBuffer;
			return false;
		}
	}
};
bool RMIClientBase::SendMessage(void* pValue,int tSendLength)
{
	if(!mConnected)
		return false;
	if(send(mRemoteSocket,(char*)pValue,tSendLength,0)<tSendLength)
		return false;
	else
		return true;
};