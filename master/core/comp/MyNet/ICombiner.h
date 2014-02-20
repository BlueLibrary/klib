#pragma once

/*  Э��ְ��ϲ���(����tcp��)  */

#include "net_conn.h"

class icombiner
{
public:
  /*
	buff: �����Ļ��������ж��Ƿ��ܹ���һ��������
    bufflen: ������buff����Ч���ݳ���
    packetlen: ��ɷ����ĳ���,�������С��bufflen, Ҳ���ܴ���bufflen
  */
  virtual bool is_intact_packet(const net_stream_type& stream, int& packetlen) = 0;
};