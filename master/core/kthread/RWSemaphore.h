#pragma once
#include "Semaphore.h"
#include "../istddef.h"

namespace klib {
namespace kthread {


class CWRSemaphore
{
public:
	CWRSemaphore(tstring strSeamporeName);
	~CWRSemaphore(void);

	//�ȴ���д�ź�
	void write_wait();

	//�ȴ��ɶ��ź�
	void read_wait();

	//����д�ź�
	void write_set();

	//���ö��ź�
	void read_set();

	void close_semaphore();

private:
	CSemaphore      m_write;
	CSemaphore      m_read;
};


}}
