#include "main.h"
#include "delay.h"

#include "TCP.h"

#include <string.h>

int main(void) {
	//TCP��˿ڼ���Ĭ������
	tcp_t tcp_config = {
		.socket_max = SOCKET_MAX,
		.socket_listen_max = SOCKET_LISTEN_MAX, .ip = {
		.ip_addr = IP_LADDR, 
		.gate_way = GATE_WAY, 
		.ip_mask = IP_MASK}, .listen = {{
		//�����Ķ˿ںţ�		��Ӧ��socket������								�˿����������
		.port = PORT_LADDR_0, 	.socketi = 0, 									.connect_num = CONNECT_NUM_0},{
		.port = PORT_LADDR_1, 	.socketi = 1 + CONNECT_NUM_0, 					.connect_num = CONNECT_NUM_1},{
		.port = PORT_LADDR_2, 	.socketi = 2 + CONNECT_NUM_0 + CONNECT_NUM_1, 	.connect_num = CONNECT_NUM_2}}
	};
	
	delay_init();
	
	//��ʼ��TCPServer ���Ϊ0��ɹ�
	init_tcp_server(&tcp_config);
	
	//����TCPServer���� ���Ϊ0��ɹ�
	listen_tcp(&tcp_config.listen[0]);
	listen_tcp(&tcp_config.listen[1]);
	listen_tcp(&tcp_config.listen[2]);
	
	while(1) {
		process_tcp_server(&tcp_config);
	}
}

void handle_listen_0_client(const uint8_t sockindex) {
	handle_client(sockindex);
}

void handle_listen_1_client(const uint8_t sockindex) {	
	uint16_t len = recv_data(sockindex, recv_buff, RECV_BUFF_LEN);
	
	send_data(sockindex, recv_buff, len);
}

void handle_listen_2_client(const uint8_t sockindex) {
	uint8_t buff[RECV_BUFF_LEN];
	uint16_t len = recv_data(sockindex, buff, RECV_BUFF_LEN);
	
	send_data(sockindex, buff, len);
}

/*
 * ����INT�ж����ţ���������ͻ��ˣ�
 */
void handle_INT(tcp_p tcp) {
	ch395_status = CH395CMDGetGlobIntStatus_ALL();
	
	/* ���� ����0 �ж� */
	for(int i = tcp->listen[0].socketi; i <= tcp->listen[0].socketi + tcp->listen[0].connect_num; i++) {
		if(ch395_status & (1 << (4 + i))) {
			handle_listen_0_client(i);
		}
	}
	/* ���� ����1 �ж� */
	for(int i = tcp->listen[1].socketi; i <= tcp->listen[1].socketi + tcp->listen[1].connect_num; i++) {
		if(ch395_status & (1 << (4 + i))) {
			handle_listen_1_client(i);
		}
	}
	/* ���� ����2 �ж� */
	for(int i = tcp->listen[2].socketi; i <= tcp->listen[2].socketi + tcp->listen[2].connect_num; i++) {
		if(ch395_status & (1 << (4 + i))) {
			handle_listen_2_client(i);
		}
	}
}
