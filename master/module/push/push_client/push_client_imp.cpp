#include "stdafx.h"
#include "push_client_imp.h"

#include "../common/protocol.h"
#include "../common/protocol_st.h"

#include "app_data.h"
#include <net/proto/local_archive.h>


#pragma comment(lib, "sign_verify")

using namespace std::placeholders;

//----------------------------------------------------------------------
//

//----------------------------------------------------------------------
// ״̬���
void query_logic_state::enter(state_i* s)
{
    query_addr_watch_.reset();
}

void query_logic_state::on_event(FsmEvent* e, UINT& uNewStateID)
{
    push_client_imp* pclient = push_client_imp::instance();

    if (e->get_evt_type() == event_timer) 
    {
        pclient->send_query_logic_addr();

        if (query_addr_watch_.is_triggerd()) 
        {
            query_addr_watch_.reset();

            // �ж��ǿ�����Ϊ�˿ڱ����ƣ�������Ҫ���°󶨶˿�
            pclient->reinit();
        }
    }
    else if (e->get_evt_type() == event_query_logic_ack_msg)
    {
        // ת��״̬
        uNewStateID = status_query_newver;
    }
    else
    {
        _ASSERT(FALSE);
    }
}

void query_newver_state::enter(state_i* s)
{
    // ��Ҫ���ö�ʱ������
    newver_watch_.reset();
}

void query_newver_state::on_event(FsmEvent* e, UINT& uNewStateID)
{
    push_client_imp* pclient = push_client_imp::instance();

    if (e->get_evt_type() == event_timer) 
    {
        // ���Ͳ�ѯ���°汾��Ϣ
        pclient->send_query_curver();

        // �ж��Ƿ�ʱ������
        if (newver_watch_.is_triggerd()) 
        {
            newver_watch_.reset();

            uNewStateID = status_query_logic_addr;

            // �ж��ǿ�����Ϊ�˿ڱ����ƣ�������Ҫ���°󶨶˿�
            pclient->reinit();
        }
    }
    else if (e->get_evt_type() == event_query_ver_ack_msg) 
    {
        // �л�������״̬
        uNewStateID = status_online ;
    }
    else
    {
        _ASSERT(FALSE);
    }
}

void online_state::enter(state_i* s)
{
    UINT64 uTimeNow = _time64(NULL);

    // �����յ������ߵĻ�����Ϣ
    last_online_ack_time_ = uTimeNow;

    is_online_ = FALSE;
}

void online_state::on_event(FsmEvent* e, UINT& uNewStateID)
{
    push_client_imp* pclient = push_client_imp::instance();

    if (e->get_evt_type() == event_timer) 
    {
        // ����Ѿ������ˣ���Ҫÿ��30���ʱ���ٷ���
        if (is_online_) 
        {
            // ����30���ʱ����һ��������¼
            if (online_watch_.is_triggerd()) 
            {
                online_watch_.reset();
                pclient->send_online();
            }
        }
        else
        {
            // �����ɷ���
            online_watch_.reset();
            pclient->send_online();
        }

        // ����ʱ��δ�յ�������Ϣ�������������Ҫ���»�ȡ�߼���ַ
        UINT64 uTimeNow = _time64(NULL);
        if (uTimeNow - last_online_ack_time_ > DEFAULT_ONLINE_TIME_OUT) 
        {
            is_online_ = FALSE;
            uNewStateID = status_query_logic_addr;  // �л�����ѯ�߼�������״̬
            MyPrtLog("�л�����ѯ�߼���������ַ״̬!!!");
        }
    }
    else if (e->get_evt_type() == event_online_ack_msg)
    {
        // ����״̬
        is_online_ = TRUE;
        last_online_ack_time_ = _time64(NULL);
    }
    else
    {
        _ASSERT(FALSE);
    }
}

//----------------------------------------------------------------------
//
void push_client_imp::start()
{
    timer_mgr_.start();
    timer_mgr_.add(3000, std::bind(&push_client_imp::timer_check_status, this));

    client_.start(FALSE);
    client_.enable_udpreset(TRUE);
    client_.set_callback(std::bind(&push_client_imp::on_msg,
        this, _1, _2, _3, _4, _5));

    push_fsm_.start();

    verify_helper_.set_pub_info("BA3F7EFF8A7AD3A6A445FD8C16E373F43C45A81D23E27588E13FCB8CA8A44F6E0E60AFD2E880759FF916A11025B1682A318FF0ADB3F45B5EE559E3D402A844B5DBEF71273D21AB8CE46F775964CE80617C645DF0994274041D993257A7F90BD50F0CDC84EA6AA31C766B2EB9A9832C0D13EE59EFF745B638AA0CCBAE6A939835", 
        "010001");
}

void push_client_imp::stop()
{
}

push_client_status push_client_imp::get_status()
{
    return (push_client_status) push_fsm_.get_cur_state_id();
}

void push_client_imp::reinit()
{
    client_.reinit();
}

void push_client_imp::on_msg(udp_client* client_, UINT32 uAddr, USHORT uPort, char* buff, int iLen) 		///< UDP��Ϣ�ص��ӿ�
{
    MyPrtLog("message comming!!!");

    net_archive ar(buff, iLen);

    cmd_header header;
    ar >> header;

    if (header.cmd == CMD_QUERY_LOGIC_SERVER_ACK) 
    {
        on_query_logic_svr_ack(uAddr, uPort, header, ar);
    }
    else if (header.cmd == CMD_QUERY_CURRENT_VERSION_ACK) 
    {
        on_cur_ver_ack(uAddr, uPort, header, ar);
    }
    else if (header.cmd == CMD_MESSAGE_NOTIFY) 
    {
        on_msg_notify(uAddr, uPort, header, ar);
    }
    else if (header.cmd == CMD_MESSAGE_CONTENT) 
    {
        on_msg_content(uAddr, uPort, header, ar);
    }
    else if (header.cmd == CMD_ONLINE_ACK) 
    {
        on_online_msg_ack(uAddr, uPort, header, ar);
    }
}

void push_client_imp::send_query_logic_addr()
{
    MyPrtLog("���Ͳ�ѯ�߼���������%d��Ϣ...", app_data::instance()->get_svr_port());

    cmd_header header;
    PT_CMD_QUERY_LOGIC_SERVER ptQuery;

    header.cmd = CMD_QUERY_LOGIC_SERVER;
    ptQuery.uuid = 123456; // @todo ������Ҫ���¶���

    local_archive<> ar;
    ar << header;
    ar << ptQuery;

    BOOL bSendResult = client_.send_to(app_data::instance()->get_svr_domain().c_str(), 
        app_data::instance()->get_svr_port(), 
        ar.get_buff(), 
        ar.get_data_len());

    if (!bSendResult) {
        //@todo ���ض���ip��������
        //client_.send_to(BALANCE_SERVER_IP, BALANCE_SERVER_UDP_PORT, ar.GetBuff(), ar.GetDataLength());
    }
}

void push_client_imp::send_query_curver()
{
    MyPrtLog(_T("���Ͳ�ѯ���°汾��Ϣ"));

    cmd_header header(CMD_QUERY_CURRENT_VERSION);
    PT_CMD_QUERY_CURRENT_VERSION ptQueryCurVer;
    ptQueryCurVer.strVersion   = CURRENT_CLIENT_VERSION_STRING;
    ptQueryCurVer.uCurVerValue = CURRENT_CLIENT_VERSION_VALUE;

    local_archive<> ar;
    ar << header;
    ar << ptQueryCurVer;

    client_.send_to(app_data::instance()->get_logic_addr(), 
        app_data::instance()->get_logic_port(), 
        ar.get_buff(), 
        ar.get_data_len());
}

void push_client_imp::send_online()
{
    MyPrtLog("����������Ϣ...");
    app_data* data = app_data::instance();

    // ���岢���ṹ
    cmd_header header;
    PT_CMD_ONLINE ptOnline;

    header.cmd = CMD_ONLINE;
    ptOnline.mac = data->get_mac();
    ptOnline.channel = data->get_channel();
    ptOnline.last_msg_id = data->get_last_msg_id();
    ptOnline.login_name = data->get_user();
    ptOnline.version = CURRENT_CLIENT_VERSION_VALUE;
    ptOnline.account = data->get_user();

    // ���л�����������
    local_archive<> ar;
    ar << header;
    ar << ptOnline;

    // ���͵��߼�������
    client_.send_to(app_data::instance()->get_logic_addr(), 
        app_data::instance()->get_logic_port(),
        ar.get_buff(), 
        ar.get_data_len());
}

void push_client_imp::send_msg_ack(UINT64 uMsgID)
{
    MyPrtLog("������Ϣȷ����Ϣ...");

    // ���岢���ṹ
    cmd_header header;
    PT_CMD_MESSAGE_CONTENT_ACK ptMsgAck;

    header.cmd = CMD_MESSAGE_CONTENT_ACK;
    ptMsgAck.uMsgID = uMsgID;
    ptMsgAck.emOpResult = OP_SUCCESS;

    local_archive<> ar;
    ar << header;
    ar << ptMsgAck;

    // ���͵��߼�������
    client_.send_to(app_data::instance()->get_logic_addr(), 
        app_data::instance()->get_logic_port(), 
        ar.get_buff(), 
        ar.get_data_len());
}

void push_client_imp::on_query_logic_svr_ack(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
{
    MyPrtLog("�յ��߼���������ѯ�ظ���Ϣ...");

    PT_CMD_QUERY_LOGIC_SERVER_ACK ack;
    ar >> ack;

    if (ar.get_error()) {
        MyPrtLog(_T("��Ӧ��ѯ�߼���������ַ��Э�����!!!"));
        return;
    }

    _ASSERT(push_fsm_.get_cur_state_id() == status_query_logic_addr);
    if (push_fsm_.get_cur_state_id() == status_query_logic_addr) 
    {
        // ��÷������ĵ�ַ���˿�
        app_data::instance()->set_logic_addr(ack.uLogicIP);
        app_data::instance()->set_logic_port(ack.uLogicUdpPort);

        _ASSERT(ack.uLogicIP > 0);
        _ASSERT(ack.uLogicUdpPort > 0);

        // ״̬���¼� 
        message_event ev(event_query_logic_ack_msg);
        push_fsm_.on_event(&ev);

        MyPrtLog("�߼���������ַ��ȡ�ɹ�:%s:%d...", 
            inet_ntoa(*(in_addr*)& ack.uLogicIP), 
            ntohs(ack.uLogicUdpPort));
    }
}

void push_client_imp::on_online_msg_ack(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
{
    MyPrtLog(_T("�������ظ�������Ϣ..."));

    if (uAddr == app_data::instance()->get_logic_addr()) 
    {
        PT_CMD_ONLINE_ACK ptOnlineAck;
        ar >> ptOnlineAck;
        if (ar.get_error()) {
            MyPrtLog(_T("������Ϣ������������Ϣ����..."));
            return;
        }

        // ����������Ϣ
        message_event ev(event_online_ack_msg);
        push_fsm_.on_event(&ev);
        return;
    }

    MyPrtLog(_T("���������߼�����������Ϣ"));
}

void push_client_imp::on_msg_notify(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
{
    MyPrtLog(_T("��������Ϣ֪ͨ..."));
    _ASSERT(FALSE && _T("��δʵ��!!!"));

    PT_CMD_MESSAGE_NOTIFY ptNotify;

    ar  >> ptNotify;
    if (ar.get_error()) {
        MyPrtLog(_T("������Ϣ֪ͨ����"));
        return;
    }

    app_data* data = app_data::instance();
    if (ptNotify.msgID <= data->get_last_msg_id()){
        return;
    }
}

void push_client_imp::on_msg_content(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
{
    MyPrtLog(_T("��������Ϣ��������..."));
    PT_CMD_MESSAGE_CONTENT ptMsg;
    ar >> ptMsg;

    app_data* data_ = app_data::instance();

    if (ar.get_error()) {
        MyPrtLog(_T("������Ϣ���ݳ���..."));
        return;
    }

    if (uAddr != data_->get_logic_addr()) {
        MyPrtLog(_T("���������߼�����������Ϣ..."));
        return;
    }

    BOOL bSignResult = verify_helper_.verify_sign(ptMsg.strSign, ptMsg.strMsgContent);
    if (bSignResult) 
    {
        if (data_->get_last_msg_id() == ptMsg.uMsgID) {
            MyPrtLog(_T("pClientData->m_uLastMsgId == ptMsg.uMsgID �ѽ��չ�����Ϣ"));
            send_msg_ack(ptMsg.uMsgID);
            return;
        }

        // ���ͷ�����Ϣ
        send_msg_ack(ptMsg.uMsgID);

        // �������µ���ϢID
        data_->set_last_msg_id(ptMsg.uMsgID);
        data_->save();

        // ������Ϣ����
        push_msg_ptr msg_(new push_msg);
        if (nullptr == msg_) {
            MyPrtLog(_T("������Ϣ�ڴ�ռ�ʧ��!!!"));
            return;
        }

        // ��֤ͨ���󱣴浽��Ϣ�б���
        msg_->set_msg_id(ptMsg.uMsgID);
        msg_->set_msg_type(ptMsg.uMsgType);
        msg_->set_show_time(ptMsg.uShowTime);
        msg_->set_str_sign(std::move(ptMsg.strSign));
        msg_->set_content(std::move(ptMsg.strMsgContent));
        msg_->set_delay_fetch(ptMsg.uDelayFetch);
        msg_->set_delay_show(ptMsg.uDelayShow);
     
        // �ύ���ϲ㴦��
        auto& callback = data_->get_msg_callback();
        callback(msg_);
        return;
    }

    MyPrtLog(_T("�յ�����Ϣ��֤���ɹ����жϲ������Է�����"));
}

void push_client_imp::on_cur_ver_ack(UINT32 uAddr, USHORT uPort, cmd_header& header, net_archive& ar)
{
    PT_CMD_QUERY_CURRENT_VERSION_ACK ptCurVersionAck;
    app_data* data_ = app_data::instance();

    // ��ȡ��Ϣ����
    ar >> ptCurVersionAck;

    // ���Զ����£��Ҳ���ǿ�Ƹ��µĻ���ô����������
//     if (!pClientData->GetIsAutoUpdate() && !ptCurVersionAck.bForceUpdate) 
//     {
//         return;
//     }

    // ��Ҫ���и���
    if (ptCurVersionAck.uVersionValue > CURRENT_CLIENT_VERSION_VALUE) 
    {
        // ��ʼ�������µİ汾����ִ����������
        MyPrtLog(_T("���ָ��߼��İ汾,ptCurVersionAck.uVersionValue > CURRENT_CLIENT_VERSION_VALUE..."));

        // ����
    }

    MyPrtLog(_T("״̬ת����תΪ: state_query_new_ver_ok"));

    // ����״̬�¼�
    message_event ev(event_query_ver_ack_msg);
    push_fsm_.on_event(&ev);
}

bool push_client_imp::timer_check_status() 
{
    timer_event ev(3000);
    push_fsm_.on_event(&ev);

    return TRUE;
}