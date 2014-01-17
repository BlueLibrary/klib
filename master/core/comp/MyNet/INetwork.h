#pragma once

#include "MySocket.h"

class INetConnection;
class INetEventHandler;
class INetNetwork
{
public:
  INetNetwork(void);
  ~INetNetwork(void);

public:
  /// 
  /// @brief ��ʼ�������ӿڣ���INetTcpClient����
  virtual bool InitNetwork() = 0; 

  /// @brief �����¼�����������Ҫ���������¼�
  virtual bool SetNetEventHandler(INetEventHandler* handler) = 0;

  /// @brief ��������㣬�����߳���Щ
  virtual bool RunNetwork() = 0;
  
  /// @brief Ͷ�ݽ�������
  ///
  /// @param[in] plistenConn �����׽��ֽӿڣ���Ҫ��ʼ�� ���ö����@ref InitListenConnection����
  /// @return �´��������ڽ��ܿͻ������ӵ��׽��ֽӿ�
  /// @retval NULL Ͷ��ʧ��
  /// @retval !=NULL  �´��������ڽ������ӵ��׽��ֽӿ�,������ INetConnection*
  /// @post  �����׽��ֽӿڣ���Ͷ��һ��AcceptEx���󣬵�ϵͳ�������ʱ���յ�һ���������
  ///
  /// @usage   INetConnection* pListenConn = pClient->GetNetwork()->CreateNewConnection();
  /// pListenConn->set_local_port(7000);
  /// pClient->GetNetwork()->InitListenConnection(pListenConn);
  /// pClient->GetNetwork()->PostAccept(pListenConn);
  ///
  virtual INetConnection* PostAccept(INetConnection* plistenConn) = 0;

  /// @brief Ͷ������
  /// 
  /// ���ӶԷ�������ip+�˿�
  ///
  /// @param[in] pConn  ���������ӽӿ�
  /// @return ��ʾͶ���Ƿ�ɹ���������ʾ�Ƿ�������
  /// @retval false Ͷ��ʧ��
  /// @retval true  Ͷ�ݳɹ�
  /// @post  Ͷ������������ɶ˿���
  ///
  /// @usage   INetConnection* pMyConn = pClient->GetNetwork()->CreateNewConnection();
  /// pMyConn->set_peer_addr("127.0.0.1");
  /// pMyConn->set_peer_port(7000);
  /// pClient->GetNetwork()->PostConnection(pMyConn);
  /// 
  virtual bool PostConnection(INetConnection* pConn) = 0;

  /// @brief �ڽ�����������Ͷ��һ��������
  ///
  /// @param[in] pConn  �Ѿ����ӵ����ӽӿ�
  /// @return ��ʾͶ���Ƿ�ɹ���������ʾ�Ƿ��Ѷ�ȡ������
  /// @retval false Ͷ��ʧ��
  /// @retval true  Ͷ�ݳɹ�
  /// @post  
  ///
  /// @usage   INetConnection* pMyConn = pClient->GetNetwork()->CreateNewConnection();
  /// pClient->GetNetwork()->PostRead(pMyConn);
  /// 
  virtual bool PostRead(INetConnection* pConn) = 0;

  /// @brief �ڽ�����������Ͷ��һ��д��������
  ///
  /// @param[in] pConn  �Ѿ����ӵ����ӽӿ�
  /// @return ��ʾͶ���Ƿ�ɹ���������ʾ�Ƿ��ѷ���������
  /// @retval false Ͷ��ʧ��
  /// @retval true  Ͷ�ݳɹ�
  /// @post  
  ///
  /// @usage   INetConnection* pMyConn = pClient->GetNetwork()->CreateNewConnection();
  /// pClient->GetNetwork()->PostWrite(pMyConn, buff, 1024);
  /// 
  virtual bool PostWrite(INetConnection* pConn, const char* buff, size_t len) = 0;

  /// @brief ������ȡ�����׽ӿ�
  ///
  /// @param[in] plistenConn  ʹ��@ref CreateNewConnection���������Ľӿ�
  /// @return INetConnection �������׽��ֽӿ�
  /// @retval NULL   ����ʧ��
  /// @retval != NULL  �����ɹ�
  /// @post  
  ///
  /// @usage   INetConnection* pListenConn = CreateListenConn(9000);
  ///          ʹ��PostAccept(pListenConn); Ͷ�ݽ�������
  /// 
  virtual INetConnection* CreateListenConn(USHORT uLocalPort) = 0;

  /// @brief ��ȡһ���׽��ֽӿ�
  ///
  /// �������ǻ�ȡ���ڵ��׽��ֽӿڣ����û�еĻ��ͻ�new����һ��������
  ///
  /// @return �׽��ֽӿ�
  /// @retval NULL   ����ʧ��
  /// @retval !=NULL  �����ɹ�
  /// @post  
  ///
  /// @usage   INetConnection* pListenConn = pClient->GetNetwork()->CreateNewConnection();
  /// 
  virtual INetConnection* CreateNewConn() = 0;

  /// @brief ����һ���׽��ֽӿ�
  ///
  /// ��������е������Ƚϴ�Ļ�����ռ���ڴ�ͻᱻ�ͷţ���������бȽ�С�򲻻��ͷ�
  ///
  /// @param[in] pConn   �����ɹ��˵��׽��ֽӿ�
  /// @return ��ʾ�Ƿ��ͷųɹ�
  /// @retval false   �ͷ�ʧ��
  /// @retval true    �ͷųɹ�
  /// @post  
  ///
  /// @usage   INetConnection* pListenConn = pClient->GetNetwork()->FreeConnection(pConn);
  /// 
  virtual bool ReleaseConnection(INetConnection* pConn) = 0;
};