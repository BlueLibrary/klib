#pragma once

class NetPacket;

//��������
class IDispatchHandler
{
public:
  virtual bool DispatchPacket(NetPacket* pPacket) = 0;
  //virtual 
};