#pragma once

#include "net_packet.h"

//��������
class dispatcher_handler
{
public:
  virtual bool dispatch_packet(net_packet* pPacket) = 0;
  //virtual 
};