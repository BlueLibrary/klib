// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� OXNET_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// OXNET_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef OXNET_EXPORTS
#define OXNET_API __declspec(dllexport)
#else
#define OXNET_API __declspec(dllimport)
#endif

//  //�����Ǵ� OXNet.dll ������
// class OXNET_API COXNet {
// public:
// 	COXNet(void);
// 	// TODO: �ڴ�������ķ�����
// };
// 
// extern OXNET_API int nOXNet;
// 
// OXNET_API int fnOXNet(void);
