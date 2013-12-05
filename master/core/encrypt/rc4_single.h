
#if !defined(_Klib_rc4_H_)
#define _Klib_rc4_H_


namespace klib{
namespace encode{

class rc4_encrypt
{
public:
    void set_key(const char* pszKey, int iLen)
    {
        rc4_init(m_sBox,(unsigned char *)pszKey, iLen);
    }

    void encrypt(unsigned char* pszBuff, int iLen)
    {
        rc4_crypt(m_sBox, pszBuff, iLen);
    }

    void decrypt(unsigned char* pszBuff, int iLen)
    {
        rc4_crypt(m_sBox, pszBuff, iLen);
    }

protected:
    
/*
����RC4�����㷨�Ǵ���������RSA�������е�ͷ������Ron Rivest��1987����Ƶ���Կ���ȿɱ���������㷨�ء�
      ֮���Գ���Ϊ�أ�����������Ĳ��ֵ�S-box���ȿ�Ϊ���⣬��һ��Ϊ256�ֽڡ�
      ���㷨���ٶȿ��ԴﵽDES���ܵ�10�����ң��Ҿ��кܸ߼���ķ����ԡ�RC4��������ڱ�����ҵ���ܵġ�
      ������1994��9�£������㷨�������ڻ������ϣ�Ҳ�Ͳ�����ʲô��ҵ�����ˡ�
      RC4Ҳ������ARC4��Alleged RC4������ν��RC4������ΪRSA������û����ʽ����������㷨��
*/

    //@todo ��װ��һ���࣬����ʹ��
    void rc4_init(unsigned char *s, unsigned char *key, unsigned long Len) //��ʼ������
    {
	    int i =0, j = 0;
	    char k[256] = {0};
	    unsigned char tmp = 0;
	    for(i=0;i<256;i++)
	    {
		    s[i]=i;
		    k[i]=key[i%Len];
	    }
	    for (i=0; i<256; i++)
	    {
		    j=(j+s[i]+k[i])%256;
		    tmp = s[i];
		    s[i] = s[j]; //����s[i]��s[j]
		    s[j] = tmp;
	    }
    }
    void rc4_crypt(unsigned char *s, unsigned char *Data, unsigned long Len) //�ӽ���
    {
	    int i = 0, j = 0, t = 0;
	    unsigned long k = 0;
	    unsigned char tmp;
	    for(k=0;k<Len;k++)
	    {
		    i=(i+1)%256;
		    j=(j+s[i])%256;
		    tmp = s[i];
		    s[i] = s[j]; //����s[x]��s[y]
		    s[j] = tmp;
		    t=(s[i]+s[j])%256;
		    Data[k] ^= s[t];
	    }
    }

protected:
    unsigned char m_sBox[256];
};

/*
void main()
{
	unsigned char s[256] = {0},s2[256] = {0}; //S-box
	char key[256] = {"just for test"};
	char pData[512] = "����һ���������ܵ�����Data";
	unsigned long len = strlen(pData);
	printf("pData = %s\n",pData);
	printf("key = %s, length = %d\n\n",key,strlen(key));
	rc4_init(s,(unsigned char *)key, strlen(key)); //�Ѿ�����˳�ʼ��
	printf("��ɶ�S[i]�ĳ�ʼ�������£�\n\n");
	for (int i=0; i<256; i++)
	{
		printf("%-3d ",s[i]);
	}
	printf("\n\n");
	for(i=0;i<256;i++)//��s2[i]��ʱ����������ʼ����s[i]������Ҫ�ģ�����
	{
		s2[i]=s[i];
	}
	printf("�Ѿ���ʼ�������ڼ���:\n\n");
	rc4_crypt(s,(unsigned char *)pData,len);//����
	printf("pData = %s\n\n",pData);
	printf("�Ѿ����ܣ����ڽ���:\n\n");
	rc4_init(s,(unsigned char *)key, strlen(key)); //��ʼ����Կ
	rc4_crypt(s2,(unsigned char *)pData,len);//����
	printf("pData = %s\n\n",pData);
}
*/


/***

<?php

//$pwd ��Կ
//$data Ҫ���ܵ�����
function rc4 ($pwd, $data) {//$pwd��Կ $data������ַ���
    $key[] ="";
    $box[] ="";
    $pwd_length = strlen($pwd);
    $data_length = strlen($data);
	$cipher = '';
    for ($i = 0; $i < 256; $i++){
        $key[$i] = ord($pwd[$i % $pwd_length]);
        $box[$i] = $i;
    }
    for ($j = $i = 0; $i < 256; $i++){
        $j = ($j + $box[$i] + $key[$i]) % 256;
        $tmp = $box[$i];
        $box[$i] = $box[$j];
        $box[$j] = $tmp;
    }
    for ($a = $j = $i = 0; $i < $data_length; $i++){
        $a = ($a + 1) % 256;
        $j = ($j + $box[$a]) % 256;
        $tmp = $box[$a];
        $box[$a] = $box[$j];
        $box[$j] = $tmp;
        $k = $box[(($box[$a] + $box[$j]) % 256)];
        $cipher .= chr(ord($data[$i]) ^ $k);
    }
    return $cipher;
}
$key = '5201314';                   //ԭʼKEY
$pwd = md5(md5($key).'���ǳ���');   //md5+����
$pwd = "just for test";
$data = '����һ���������ܵ�����Data';          //Ҫ���ܵĔ���
$cipher = rc4($pwd, $data);         //AC4 �����㷨
$c = rc4($pwd, $cipher);            //AC4 �����㷨߀ԭ (߀ԭֻ��Ҫ���¼���һ��)
echo '<pre>';
var_dump($key);
var_dump($pwd);
var_dump($data);
var_dump($cipher);
var_dump($c);
?>

*/

}} // namespace 

#endif