#pragma once

/*  Э��ְ��ϲ���(����tcp��)  */

class ICombiner
{
public:
  /*
	buff: �����Ļ��������ж��Ƿ��ܹ���һ��������
    bufflen: ������buff����Ч���ݳ���
    packetlen: ��ɷ����ĳ���,�������С��bufflen, Ҳ���ܴ���bufflen
  */
  virtual bool IsIntactPacket(const char* buff, int bufflen, int& packetlen) = 0;
};