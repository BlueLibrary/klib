#ifndef _klib_rw_semaphore_h
#define _klib_rw_semaphore_h
#include "semaphore.h"
#include "../istddef.h"

namespace klib {
namespace kthread {


class wr_semaphore
{
public:
	wr_semaphore(tstring strSeamporeName);
	~wr_semaphore(void);

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
	semaphore      m_write;
	semaphore      m_read;
};


}}


#endif
