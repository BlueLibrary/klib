#pragma once

/*

   �������� 

*/

class net_packet;
class net_conn;

class inetpacket_mgr
{
public:
  inetpacket_mgr(void){}
  virtual ~inetpacket_mgr(void) {}

public:
  virtual bool add_packet(net_packet* pPacket)  = 0;					//��ӵ������������ȥ��pPacket���Ѿ�����õķ����
  virtual net_packet* get_packet(bool bRemoveFlag = true) = 0;		//�ӷ���������л�ȡһ�����������ݰ�
  virtual bool free_conn_packets(net_conn* pConn) = 0;			//�ͷ�һ�����ӵ����з��

  virtual net_packet* alloc_net_packet() = 0;							//����һ������ṹ
  virtual bool free_net_packet(net_packet* pPacket) = 0;				//�ͷ�һ������ṹ

protected:

};
