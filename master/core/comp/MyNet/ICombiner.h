#pragma once

/*  Э��ְ��ϲ���(����tcp��)  */

#include "net_conn.h"

class ICombiner
{
public:
  /*
	buff: �����Ļ��������ж��Ƿ��ܹ���һ��������
    bufflen: ������buff����Ч���ݳ���
    packetlen: ��ɷ����ĳ���,�������С��bufflen, Ҳ���ܴ���bufflen
  */
  virtual bool IsIntactPacket(const net_stream_type& stream, int& packetlen) = 0;
};