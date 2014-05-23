#pragma once
#include <Windows.h>
#include <string>
using namespace std;


//�����װ��Win32�ź���ͬ�����ƣ�֧���̡߳����̼�ͬ��
//�ź���ͬ�����ƣ�֧��һ���̷߳����źţ�һ���̵߳ȴ��ź�,ÿ����һ���ź�
//�ں˶�����źż�����1��ÿ�ȴ���һ���ź��ں˶����źż�����1�����ں˶���
//�źż���Ϊ0ʱ���ȴ���������������ֱ���źż�������0��ȴ���ʱ

#include "../istddef.h"

namespace klib {
namespace kthread{


/**
 * @brief �ź���ʵ����
 */
class semaphore
{
public:
	semaphore(int usercount, int maxcount, tstring name);
	~semaphore(void);

    /**
     * @brief �źż�����1,�ͷ��ź���
     */
	void release(int n = 1);

    /**
     * @brief �ȴ��źţ��źż�����1
     */
	void wait(DWORD dwMilliseconds = INFINITE);

    /**
     * @brief 
     */
	void close();

private:
	HANDLE  m_hSemaphore;               ///< �ź������
};


}}
