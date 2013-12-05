#pragma once

/*

   �������� 

*/

class NetPacket;
class INetConnection;

class INetPacketMgr
{
public:
  INetPacketMgr(void);
  ~INetPacketMgr(void);

public:
  virtual bool AddPacket(NetPacket* pPacket)  = 0;					//��ӵ������������ȥ��pPacket���Ѿ�����õķ����
  virtual NetPacket* GetPacket(bool bRemoveFlag = true) = 0;		//�ӷ���������л�ȡһ�����������ݰ�
  virtual bool FreeConnPacket(INetConnection* pConn) = 0;			//�ͷ�һ�����ӵ����з��

  virtual NetPacket* AllocNetPacket() = 0;							//����һ������ṹ
  virtual bool FreeNetPacket(NetPacket* pPacket) = 0;				//�ͷ�һ������ṹ

protected:

};
