#include "main.h"
#include "delay.h"

#include "TCP.h"

#include <string.h>

int main(void) {
	//TCP多端口监听默认配置
	tcp_t tcp_config = {
		.socket_max = SOCKET_MAX,
		.socket_listen_max = SOCKET_LISTEN_MAX, .ip = {
		.ip_addr = IP_LADDR, 
		.gate_way = GATE_WAY, 
		.ip_mask = IP_MASK}, .listen = {{
		//监听的端口号，		对应的socket索引，								端口最大连接数
		.port = PORT_LADDR_0, 	.socketi = 0, 									.connect_num = CONNECT_NUM_0},{
		.port = PORT_LADDR_1, 	.socketi = 1 + CONNECT_NUM_0, 					.connect_num = CONNECT_NUM_1},{
		.port = PORT_LADDR_2, 	.socketi = 2 + CONNECT_NUM_0 + CONNECT_NUM_1, 	.connect_num = CONNECT_NUM_2}}
	};
	
	delay_init();
	
	//初始化TCPServer 结果为0则成功
	init_tcp_server(&tcp_config);
	
	//启动TCPServer监听 结果为0则成功
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
 * 处理INT中断引脚（包括处理客户端）
 */
void handle_INT(tcp_p tcp) {
	ch395_status = CH395CMDGetGlobIntStatus_ALL();
	
	/* 处理 监听0 中断 */
	for(int i = tcp->listen[0].socketi; i <= tcp->listen[0].socketi + tcp->listen[0].connect_num; i++) {
		if(ch395_status & (1 << (4 + i))) {
			handle_listen_0_client(i);
		}
	}
	/* 处理 监听1 中断 */
	for(int i = tcp->listen[1].socketi; i <= tcp->listen[1].socketi + tcp->listen[1].connect_num; i++) {
		if(ch395_status & (1 << (4 + i))) {
			handle_listen_1_client(i);
		}
	}
	/* 处理 监听2 中断 */
	for(int i = tcp->listen[2].socketi; i <= tcp->listen[2].socketi + tcp->listen[2].connect_num; i++) {
		if(ch395_status & (1 << (4 + i))) {
			handle_listen_2_client(i);
		}
	}
}
