#pragma once

class net_packet;

//��������
class dispatcher_handler
{
public:
  virtual bool DispatchPacket(net_packet* pPacket) = 0;
  //virtual 
};