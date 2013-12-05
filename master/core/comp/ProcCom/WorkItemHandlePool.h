#pragma once
#include<Windows.h>
#include <string>
#include <functional>
using namespace std;

class CWorkItemHandlePool
{
public:
	CWorkItemHandlePool(void);
    ~CWorkItemHandlePool(void);

    /**
     * @brief �ص���������
     */
    typedef tr1::function<void(string)> WorkerType;


    /**
     * @brief  �ύ��������
     */
	void SubmitWork(WorkerType fun, string str);

private:
    /**
     * @brief �ص�����
     */
	static DWORD WINAPI ThreadPoolCallback(LPVOID pvContext);

private:
	struct WorkItem
	{
	public:
		WorkItem(WorkerType Cmd,string str)
		{
			cmd = Cmd;
			inparam = str;
		}
		~WorkItem()
		{}

		WorkerType cmd;
		string inparam;
	};


};
