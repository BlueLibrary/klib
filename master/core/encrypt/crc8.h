

//CRC8.h      CRC8У���ʵ��
static unsigned char checkCRC(unsigned char& crc, unsigned char data)  // crc������Ϊ���õ���
{
	for (unsigned char i = 0x80; i > 0; i >>= 1) // ����ѭ����ÿ�ֽ���8��ѭ��������һ���ֽ�
	{
		if ((crc & 0x80) != 0) // ��λΪ1����λ�����������  
		{ 
			crc <<= 1; // ����һλ
			if ( (data & i) != 0) // ����¶��������Ϊ1�������λ��1
			{
				crc |= 1;
			}
			crc ^= 0x07;// ��λ�Ƴ����Ե�8λ����������㣬7�Ķ�����Ϊ0000,0111
		} 
		else // ��λΪ0��ֻ��λ�������������
		{  
			crc <<= 1;// ����һλ
			if ( (data & i) != 0)// ����¶��������Ϊ1�������λ��1
			{
				crc |= 1;
			}
		}
	}
	return crc;
}