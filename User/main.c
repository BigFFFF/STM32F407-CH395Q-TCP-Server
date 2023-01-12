#include "main.h"
#include "delay.h"

#include "TCP.h"

int main(void) {
	//TCP多端口监听默认配置
tcp_t tcp_config = {
	.socket_max = SOCKET_MAX,
	.socket_listen_max = SOCKET_LISTEN_MAX, .ip = {
	.ip_addr = IP_LADDR, 
	.gate_way = GATE_WAY, 
	.ip_mask = IP_MASK}, .listen = {{
	//---------------------------------------------------------------------------------------
	//|监听的端口号			|socket索引										|端口最大连接数	|
	//|---------------------+-----------------------------------------------+---------------|
	//|	8080				|	0											|	3			|
	//|---------------------+-----------------------------------------------+---------------|
	//|	8081				|	4											|	1			|
	//|---------------------+-----------------------------------------------+---------------|
	//|	8082				|	6											|	1			|
	//---------------------------------------------------------------------------------------
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
		//运行TCP服务器（包括检查并处理INT中断引脚）
		process_tcp_server(&tcp_config);
	}
}

void handle_client_0(const uint8_t sockindex) {
	handle_client(sockindex);
}

void handle_client_1(const uint8_t sockindex) {
	uint16_t len = recv_data(sockindex, recv_buff, RECV_BUFF_LEN);
	send_data(sockindex, recv_buff, len);
}

void handle_client_2(const uint8_t sockindex) {
	uint8_t buff[RECV_BUFF_LEN];
	
	uint16_t len = recv_data(sockindex, buff, RECV_BUFF_LEN);
	send_data(sockindex, buff, len);
}

/*
 * 处理INT中断引脚（包括处理监听客户端）
 */
void handle_INT(tcp_p tcp) {
	//根据监听端口的不同，提供不同的客户端中断处理方式
	listen_accept(&tcp->listen[0], handle_client_0);
	listen_accept(&tcp->listen[1], handle_client_1);
	listen_accept(&tcp->listen[2], handle_client_2);
}
