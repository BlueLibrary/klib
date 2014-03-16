/***

Copyright 2006 bsmith@qq.com

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#include <stdio.h>

#include "./lance/ldebug.h"
#include "./lance/tcpsrv.hpp"
#include "./lance/systm.h"

// Client�����࣬�����ӽ���ʱ�Զ������������ӶϿ�ʱ�Զ�����
class MyClient : public lance::net::Client
{
		// ���ӽ���ʱ������
public: void OnConnect()
		{
			printf("OnConnect: fd=%08x, ip=%d, port=%d\n", fd, ip, port);
			// ֪ͨ����ϵͳ��������
			// ������ʱ��û���������գ����ͻ��������ݷ�����ʱ
			// �������Զ��������ݣ�Ȼ��ͨ��OnRecv֪ͨ���ݽ������
			recv(data, 255);
		}
		// ���ӶϿ�ʱ������
public: void OnDisconnect()
		{
			printf("OnDisconnect: fd=%08x, ip=%d, port=%d\n", fd, ip, port);
		}
		// �������ݱ�����ʱ���ã����յ�ʵ�����ݳ���Ϊlen
public: void OnRecv(int len)
		{
			data[len] = 0x00;
			printf("OnRecv: fd=%08x, data=%s\n", fd, data);
			// �Ͽ���������
			if (data[0] == 'a')
			{
				printf("user exit command\n");
				// ֪ͨ����ϵͳ�Ͽ����ӣ�������ϵͳ������ɺ�������Ͽ�����
				close();
			}
			// ֪ͨ����ϵͳ��������
			// ������ʱ��û���������գ����ͻ��������ݷ�����ʱ
			// �������Զ��������ݣ�Ȼ��ͨ��OnRecv֪ͨ���ݽ������
			recv(data, 255);
		}
// ���ݻ�����
public: char data[256];
};


int main(char * args[])
{
	lance::net::TCPSrv<MyClient> srv;
	// ���ü����׽��ְ󶨵�IP
	// 0Ϊ�����б�������IP��ַ
	srv.ip = 0;
	// �����˿�
	srv.port = 1234;
	// �󶨵Ķ������Դָ��
	// MyClient�������ͨ��srv->ptr��ȡ���ָ��
	srv.ptr = NULL;
	// �����׽������Ӷ��г���
	srv.backlogs = 10;
	// �����̳߳��߳���
	srv.threads = 1;
	// �������߳�����ͨ���Ǳ���CPU����2��
	// 0��ʾ�Զ�ѡ��
	srv.scheds = 0;
	// �����������
	srv.start();

	// ѭ������֤���̲��˳�
	while(true)
	{
		lance::systm::sleep(2000);
	}

	return 0;
}
