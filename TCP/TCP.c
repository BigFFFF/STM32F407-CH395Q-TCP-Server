#include "TCP.h"
#include "delay.h"

uint8_t recv_buff[RECV_BUFF_LEN];	//�洢������յ�����
uint8_t buf[20];					//��̫��ģ����ʱ��ȡ������
int32_t ch395_status = 0;			//��ȡ�ж��¼�

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
static uint8_t dail_tcp(const tcp_p tcp) {
	/*��Socket0��Ϊ��������*/
	CH395SetSocketProtType(0, PROTO_TYPE_TCP); 		/* Э������ */
	CH395SetSocketSourPort(0, tcp->socket.port);  	/* ���ض˿ں� */
	if(CH395OpenSocket(0) != 0)                		/* ��Socket */
	{
		return ERR_TCP_DAIL;
	}
	
	for (int i = 1; i < tcp->socket_max; i++) {
		/*����Socket��Ϊ����ͨ��*/
		/*��Ҫ��·Socket�ͻ�������ͨ��,����Ҫ���ü���Socket,����ģ�����֧��7��Socket TCP�ͻ���ͨ��*/
		CH395SetSocketProtType(i, PROTO_TYPE_TCP); 							/* Э������ */
		CH395SetSocketSourPort(i, tcp->socket.port);						/* ���ض˿ں� */
	}
	
	return 0;
}

/*
 * ����TCP Server����
 */
uint8_t listen_tcp(const tcp_p tcp) {
	/*����ģ������Socket����*/
	if (dail_tcp(tcp) != 0) {
		return ERR_TCP_DAIL;
	}
	//Socke 0 ����TCP����
	if (CH395TCPListen(0) != 0) {
		return ERR_TCP_LISTEN;
	}
	return 0;
}

/*
 * ��ʼ��TCP Server
 */
uint8_t init_tcp_server(const tcp_p tcp) {
	if (tcp->socket_max > 8) {
		return ERR_SOCKET_MAX;
	}
	
	//��ʼ����̫��ģ��Ӳ������:�ɹ����� 0
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
	if (tcp->socket_max <= 4) {
		CH395SetTCPMss(MSS_MAX);
	}
	delay_ms(100);
	
	//��ʼ��IP
	init_ip(&tcp->socket.ip);
	delay_ms(300);
	
	//��ʼ����̫��ģ���������:�ɹ����� 0
	if (CH395CMDInitCH395() != 0) {
		return ERR_TCP_INIT;
	}
	
	return 0;
}

/*
 * ����ͻ��ˣ�Ĭ��Ϊecho��������
 */
__weak void handle_client(const uint8_t sockindex) {	
	/* ��ȡsocket ���ж�״̬ */
	uint8_t sock_int_socket = CH395GetSocketInt(sockindex);
	
	/* ���ͻ��������У����Լ���д��Ҫ���͵����� */
	if(sock_int_socket & SINT_STAT_SENBUF_FREE) {
		
	}
	
	/* ��������ж� */
	if(sock_int_socket & SINT_STAT_SEND_OK) {
		
	}
	
	/* ���������ж� */
	if(sock_int_socket & SINT_STAT_RECV) {
		uint16_t len = CH395GetRecvLength(sockindex);/* ��ȡ��ǰ�����������ݳ��� */
		
		if(len == 0) {
			return;
		}
		
		if(len > RECV_BUFF_LEN) {
			len = RECV_BUFF_LEN;
		}
		
		/* ��ȡ���� */
		CH395GetRecvData(sockindex,len,recv_buff);
		
		/*�ѽ��յ����ݷ��͸�������*/
		CH395SendData(sockindex,recv_buff,len);
	}
	
	/* �����жϣ�����TCPģʽ����Ч*/
	if(sock_int_socket & SINT_STAT_CONNECT) {
//		printf("socket%d SINT_STAT_CONNECT\r\n",sockindex);
//		CH395CMDGetRemoteIPP(sockindex,buf);//��ȡ�ͻ�����Ϣ
//		printf("IP address = %d.%d.%d.%d\n",(UINT16)buf[0],(UINT16)buf[1],(UINT16)buf[2],(UINT16)buf[3]);    
//		printf("Port = %d\n",((buf[5]<<8) + buf[4]));
	}
	
	/* �Ͽ��жϣ�����TCPģʽ����Ч */
	if(sock_int_socket & SINT_STAT_DISCONNECT) {

	}
 
	/* ��ʱ�жϣ�����TCPģʽ����Ч*/
	if(sock_int_socket & SINT_STAT_TIM_OUT) {

	}
}

/*
 * ����INT�ж����ţ���������ͻ��ˣ�
 */
__weak void handle_INT(tcp_p tcp, void(*handle_client_p)(const uint8_t sockindex)) {
	ch395_status = CH395CMDGetGlobIntStatus_ALL();
	
	//����PHY�ı��ж�
	if(ch395_status & GINT_STAT_PHY_CHANGE) {
		//���߶Ͽ�
		if(CH395CMDGetPHYStatus() == PHY_DISCONN) {

		}
		//��������
		else {

		}
	}
	
	/* �����ɴ��жϣ���ȡ���ɴ���Ϣ */
	if(ch395_status & GINT_STAT_UNREACH) {
		CH395CMDGetUnreachIPPT(buf);
	}
	
	/* ����IP��ͻ�жϣ����������޸�CH395�� IP������ʼ��CH395*/
	if(ch395_status & GINT_STAT_IP_CONFLI) {
		static uint8_t ip_addr_3 = 1; 
		
		tcp->socket.ip.ip_addr[3] += ip_addr_3++;
		init_tcp_server(tcp);
		listen_tcp(tcp);
		
		return;
	}
	
	/* ���� SOCKi �ж� */
	for(int i = 0; i < tcp->socket_max; i++) {
		//if(ch395_status & GINT_STAT_SOCK0){}
		if(ch395_status & (1 << (4 + i))) {
			(*handle_client_p)(i);
		}
	}
}

/*
 * ����TCP Server����ѯ�������������жϣ�
 */
__weak void process_tcp_server(const tcp_p tcp) {
	//INT���Ų����͵�ƽ�ж��Ժ��ȥ�ж�
	if(Query395Interrupt()) {
		handle_INT(tcp, handle_client);
	}
}

/*
 * ���ݽ��գ��жϣ�
 */
uint8_t recv_data(uint8_t sockindex, uint8_t buf[], uint16_t buf_len) {
	if(!(CH395GetSocketInt(sockindex) & SINT_STAT_RECV)) {
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
void send_data(uint8_t sockindex, uint8_t buf[], const uint16_t buf_len) {
	CH395SendData(sockindex, buf, buf_len);
}

/*
 * �ر�����
 */
uint8_t close_tcp() {
	//Socke 0 �ر�TCP����
	if (CH395TCPDisconnect(0) != 0) {
		return ERR_TCP_CLOSE;
	}
	return 0;
}
