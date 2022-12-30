#include "main.h"
#include "delay.h"

#include "TCP.h"

//TCP默认配置
tcp_t tcp_config = {
	.socket_max = SOCKET_MAX, .socket = {
	.port = PORT_LADDR, .ip = {
	.ip_addr = IP_LADDR, 
	.gate_way = GATE_WAY, 
	.ip_mask = IP_MASK}}
};

int main(void) {
	delay_init();
	
	//初始化TCPServer 结果为0则成功
	init_tcp_server(&tcp_config);
	
	//启动TCPServer监听 结果为0则成功
	listen_tcp(&tcp_config);
	
	while(1) {
		process_tcp_server(&tcp_config);
	}
}

void handld_client(const uint8_t sockindex) {
	uint16_t len = recv_data(sockindex, recv_buff, RECV_BUFF_LEN);
		
	CH395SendData(sockindex, recv_buff, len);
}



