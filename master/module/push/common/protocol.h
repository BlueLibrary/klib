#pragma once


/*
 Э�鶨���ļ���
    Э�����������������ͻ��ˣ����������������ͨ��˵��

    ����ʹ���Զ�����Э��ṹ����Ū�����
*/


// ����������ҳϵͳ�İ汾��
#define MINISYS_VERSION  (0x01)

// �����㷨���Ͷ��� 
enum ENCRYPTION {
	E_None,
	E_MD5 = 1,
	E_CRC,
	E_RC4,
	E_RC6,
	E_SHA1,
	E_SELF //�Զ�������㷨
};

// ���������ö�ٶ���
enum OPERATION_RESULT {
	OP_SUCCESS,
	OP_FAILED,
};

// ���ƶ˵��������Ŀ�������
typedef enum C2S {

    // ��ѯ�߼��������ĵ�ַ
    CMD_QUERY_LOGIC_SERVER = 10,
    /*
    UINT       :     uuid;
    */

    // ���صĿ��е��߼���������IP+PORT
    CMD_QUERY_LOGIC_SERVER_ACK,
    /*
    UINT       :     uLogicIP;
    USHORT     :     uLogicUdpPort;
    USHORT     :     uLogicTcpPort;
    */

    // ��ѯ��ǰ���°汾
    CMD_QUERY_CURRENT_VERSION,
    /*
    UINT             :     uCurVerValue;    // ��ǰ�汾ֵ
    std::string      :     strVersion;       // �汾���ַ���
    */

    // ���ص�ǰ�İ汾��
    CMD_QUERY_CURRENT_VERSION_ACK,
    /*
    std::string      :    strVersion;        // �汾���ַ���ֵ
    UINT             :    uVersionValue;     // �汾������ֵ
    std::string      :    strUpdateUrl;      // ���°��ĵ�ַ
    std::string      :    strUpdateSign;     // ����ǩ��
    BOOL             :    bForceUpdate;      // ǿ�Ƹ���
    BOOL             :    bClientRequest;    // �Ƿ��ǿͻ������������
    */
    
    // ���¶�֪ͨ������������Ϣ
    CMD_ONLINE,
    /*
    UINT64         :   uid;
    std::string    :   account;          // �û����ʺ�(ע����û�)
    std::string    :   mac;              // mac��ַ
    std::string    :   channel;          // ����
    UINT64         :   last_msg_id;          // ����յ�����ϢID
    std::string    :   login_name;        // ���Եĵ�½��
    UINT           :   version;            // �汾ֵ������ֵ�Ƚϸ���Щ
    */

    // ������Ϣ�Ļظ���Ϣ
    CMD_ONLINE_ACK,
    /*
    BOOL          :    bStartMini;   // ��������ҳ
    UINT64        :    uuid;         // ���ɵ�Ψһ��ʶ��ID��
    UINT64        :    msgId;        // ��Ҫչʾ����ϢID
    */

    // ������Ϣ���ͻ����˳���ʱ����
    CMD_OFFLINE,
    /*
    UINT64        :   uuid;         // ���ɵ�Ψһ��ʶ��ID��
    */

    // ������Ϣ�Ļظ���Ϣ������������ѡ�񲻻ظ���Ϣ
    CMD_OFFLINE_ACK,
    /*
    */

    // ��Ϣ֪ͨЭ��
    CMD_MESSAGE_NOTIFY,             
    /*
    UINT64        :   msgID;        // ��ϢID
    */
	
    // ��Ϣ����Э��
    CMD_MESSAGE_CONTENT,
    /*
    UINT64         :   uMsgID;              // ��ϢID
    UINT32         :   uMsgType;            // ��Ϣ����
    UINT32         :   uDelayFetch;         // �ӳٶ೤ʱ���ȡ��Ϣ
    UINT32         :   uDelayShow;          // ��ʾ�ӳ�ʱ�䣨��ָ��Ҫ�ӳٶ���ʱ����ʾ��
    UINT32         :   uShowTime;           // ��ʾ��ʱ�������Ϊ0��һֱ��ʾ��
    std::string    :   strSign;             // ��Ϣ���ݵ�ǩ������������ݵ�md5ֵ,ʹ��ǩ���㷨ǩ����
    std::string    :   strMsgContent;       // ��Ϣ����
    */

    // ��Ϣȷ��
    CMD_MESSAGE_CONTENT_ACK,
    /*
    UINT64         :       uMsgID;          // Ҫȷ�ϵ���ϢID
    UINT           :       emOpResult;      // ��ʾ���
    */


} COMMAND_CLIENT_TO_SERVER;
