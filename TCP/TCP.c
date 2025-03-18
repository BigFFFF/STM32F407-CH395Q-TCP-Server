#include "TCP.h"
#include "delay.h"

uint8_t 			recv_buff[RECV_BUFF_LEN];	//���ջ�����
volatile uint8_t 	socket_int[SOCKET_MAX];		//socket�ж�״̬
volatile uint16_t 	ch395_status = 0;			//CH395�ж�״̬

/*
 * ��ʼ����̫��ģ��
 */
static uint8_t init_eth() {
	uint8_t retry = 0;
	
	//��ʼ��CH395ʹ�õ�GPIO
	CH395_PORT_INIT();
	
	//��λCH395
	CH395_RST();
	mDelaymS(500);
	
	/*���������λȡ������˵������ͨ��*/
	while(CH395CMDCheckExist(0x55) != 0xaa) {
		delay_ms(10);
		if (retry++ > 3) {
			return ERR_ETH_SPI;
		}
	}
	delay_ms(10);
	
	//��ȡоƬ�汾
	while((CH395CMDGetVer()) < 0x44) {
		delay_ms(10);
		if (retry++ > 6) {
			return ERR_ETH_VER;
		}
	}
	
	//��λ
	CH395CMDReset();
	delay_ms(100);
	
	return 0;
}

/*
 * ��ʼ��Socket ���ͺͽ��ջ�������оƬ��0-47(SOCKET_BUFFER_BLOCKS)���飬ÿ����512�ֽڴ�С
 */
static void init_socket_buffer(const tcp_p tcp) {
	for (int i = 0; i < tcp->socket_max; i++) {
		/*Socket i*/
		CH395SetSocketRecvBuf(i, i * 6,		4);
		CH395SetSocketSendBuf(i, i * 6 + 4,	2);
	}
}

/*
 * ����IP
 */
static void init_ip(const ip_p ip) {
	CH395CMDSetIPAddr(ip->ip_addr);
	delay_ms(10);
	CH395CMDSetGWIPAddr(ip->gate_way);
	delay_ms(10);
	CH395CMDSetMASKAddr(ip->ip_mask);
	delay_ms(10);
}

/*
 * ����TCP
 */
static uint8_t dail_tcp(const listen_p listen) {
	/*��listen->socketi��Ϊ��������*/
	CH395SetSocketProtType(listen->socketi, PROTO_TYPE_TCP); 		/* Э������ */
	CH395SetSocketSourPort(listen->socketi, listen->port); 			/* ���ض˿ں� */
	if(CH395OpenSocket(listen->socketi) != 0)                		/* ��Socket */
	{
		return ERR_TCP_DAIL;
	}
	
	for (int i = listen->socketi + 1; i < listen->socketi + 1 + listen->connect_num; i++) {
		/*����Socket��Ϊ����ͨ��*/
		/*��Ҫ��·Socket�ͻ�������ͨ��,����Ҫ���ü���Socket,����ģ�����֧��7��Socket TCP�ͻ���ͨ��*/
		CH395SetSocketProtType(i, PROTO_TYPE_TCP); 						/* Э������ */
		CH395SetSocketSourPort(i, listen->port);						/* ���ض˿ں� */
	}
	
	return 0;
}

/*
 * ����TCP Server����
 */
uint8_t listen_tcp(const listen_p listen) {
	/*����ģ������Socket����*/
	if (dail_tcp(listen) != 0) {
		return ERR_TCP_DAIL;
	}
	//listen->socketi ����TCP����
	if (CH395TCPListen(listen->socketi) != 0) {
		return ERR_TCP_LISTEN;
	}
	return 0;
}


/*
 * ���TCP�����������
 */
uint8_t check_tcp_config(const tcp_p tcp) {
	if (tcp == NULL) {
		return ERR_TCP_CONFIG;
	}
	if (tcp->socket_max > 8 || tcp->socket_max == 0) {
		return ERR_TCP_CONFIG;
	}
	if (tcp->socket_listen_max > 4 || tcp->socket_listen_max == 0) {
		return ERR_TCP_CONFIG;
	}
	if (tcp->ip.ip_addr[0] == 0 && tcp->ip.ip_addr[1] == 0 && tcp->ip.ip_addr[2] == 0 && tcp->ip.ip_addr[3] == 0) {
		return ERR_TCP_CONFIG;
	}
	if (tcp->ip.ip_mask[0] == 0 && tcp->ip.ip_mask[1] == 0 && tcp->ip.ip_mask[2] == 0 && tcp->ip.ip_mask[3] == 0) {
		return ERR_TCP_CONFIG;
	}
	if (&tcp->listen[0] == NULL) {
		return ERR_TCP_CONFIG;
	}
	if (tcp->listen[0].connect_num == 0) {
		return ERR_TCP_CONFIG;
	}
	if (tcp->listen[0].port == 0) {
		return ERR_TCP_CONFIG;
	}
	if (tcp->listen[0].socketi != 0) {
		return ERR_TCP_CONFIG;
	}
	return 0;
}

/*
 * ��ʼ��TCP Server
 */
uint8_t init_tcp_server(const tcp_p tcp) {
	//���TCP�Ļ�������
	if (check_tcp_config(tcp) != 0) {
		return ERR_TCP_CONFIG;
	}
	
	//��ʼ����̫��ģ��Ӳ������
	if (init_eth() != 0) {
		return ERR_ETH_INIT;
	}
	
	//��ʼ��TCP SERVER������
	CH395SetStartPara(FUN_PARA_FLAG_TCP_SERVER);
	delay_ms(100);
	
	//��ʼ��Socket������
	if (tcp->socket_max > 4) {
		init_socket_buffer(tcp);
	}
	delay_ms(100);
	
	//����TCP MSS
	CH395SetTCPMss(MSS_DEFAULT);
	delay_ms(100);
	
	//��ʼ��IP
	init_ip(&tcp->ip);
	delay_ms(300);
	
	//��ʼ����̫��ģ���������
	if (CH395CMDInitCH395() != 0) {
		return ERR_TCP_INIT;
	}
	
	return 0;
}

/*
 * ����TCP Server����ѯ�������������жϣ�
 */
void process_tcp_server(const tcp_p tcp) {
	//INT���Ų����͵�ƽ�ж��Ժ��ȥ�ж�
	if(Query395Interrupt()) {
		//��ȡCH395ȫ���ж�״̬
		ch395_status = CH395CMDGetGlobIntStatus_ALL();
		//�����ж�
		handle_INT(tcp);
	}
}

/*
 * �����˿ڽ��գ���������ͻ��ˣ�
 */
void listen_accept(listen_p listen,  void(*handle_client_p)(const uint8_t sockindex)) {
	for(int i = listen->socketi; i <= listen->socketi + listen->connect_num; i++) {
		if(ch395_status & (1 << (4 + i))) {
			socket_int[i] = CH395GetSocketInt(i);
			(*handle_client_p)(i);
		}
	}
}

/*
 * ����ͻ��ˣ�Ĭ��Ϊecho��������
 */
void handle_client(const uint8_t sockindex) {	
	/* ��������ж� */
	if(socket_int[sockindex] & SINT_STAT_SEND_OK) {
		
	}
	
	/* ���ͻ��������У����Լ���д��Ҫ���͵����� */
	if(socket_int[sockindex] & SINT_STAT_SENBUF_FREE) {
		
	}
	
	/* ���������ж� */
	if(socket_int[sockindex] & SINT_STAT_RECV) {
		uint16_t len = CH395GetRecvLength(sockindex);/* ��ȡ��ǰ�����������ݳ��� */
		
		if(len == 0) {
			return;
		}
		
		if(len > RECV_BUFF_LEN) {
			len = RECV_BUFF_LEN;
		}
		
		/* ��ȡ���� */
		CH395GetRecvData(sockindex, len, recv_buff);
		
		/* �������� */
		CH395SendData(sockindex, recv_buff, len);
	}
	
	/* �����жϣ�����TCPģʽ����Ч */
	if(socket_int[sockindex] & SINT_STAT_CONNECT) {

	}
	
	/* �Ͽ��жϣ�����TCPģʽ����Ч */
	if(socket_int[sockindex] & SINT_STAT_DISCONNECT) {

	}
 
	/* ��ʱ�жϣ�����TCPģʽ����Ч */
	if(socket_int[sockindex] & SINT_STAT_TIM_OUT) {

	}
}

/*
 * ����INT�ж����ţ�������������Ķ˿ڣ�
 */
__weak void handle_INT(tcp_p tcp) {
	/* ����PHY�ı��ж� */
	if(ch395_status & GINT_STAT_PHY_CHANGE) {
		/* ���߶Ͽ� */
		if(CH395CMDGetPHYStatus() == PHY_DISCONN) {

		}
		/* �������� */
		else {

		}
	}
	
	/* �����ɴ��жϣ���ȡ���ɴ���Ϣ */
	if(ch395_status & GINT_STAT_UNREACH) {

	}
	
	/* ����IP��ͻ�жϣ����������޸�CH395�� IP������ʼ��CH395*/
	if(ch395_status & GINT_STAT_IP_CONFLI) {

	}
	/* ���� �������Ŀͻ��� �ж� */
	listen_accept(&tcp->listen[0], handle_client);
}

/*
 * ���ݽ��գ��жϣ�
 */
uint16_t recv_data(const uint8_t sockindex, uint8_t buf[], uint16_t buf_len) {
	//socket�˿ڽ��յ����ݻ��߽��ջ�������Ϊ��
	if(!(socket_int[sockindex] & SINT_STAT_RECV)) {
		return 0;
	}
	
	uint16_t len = CH395GetRecvLength(sockindex);
	if(len == 0) {
		return 0;
	}
	if(len > RECV_BUFF_LEN) {
		len = RECV_BUFF_LEN;
	}

	if(buf_len >= RECV_BUFF_LEN) {
		buf_len = len;
	}
	
	//��ȡ����
	CH395GetRecvData(sockindex, buf_len, buf);
	
	return len;
}

/*
 * ���ݷ���
 */
uint16_t send_data(const uint8_t sockindex, uint8_t buf[], uint16_t buf_len) {
	if (buf_len > MSS_DEFAULT) {
		buf_len = MSS_DEFAULT;
	}
	CH395SendData(sockindex, buf, buf_len);
	return buf_len;
}

/*
 * �ر�����
 */
uint8_t close_tcp(const uint8_t sockindex) {
	//Socke 0 �ر�TCP����
	if (CH395TCPDisconnect(sockindex) != 0) {
		return ERR_TCP_CLOSE;
	}
	return 0;
}
