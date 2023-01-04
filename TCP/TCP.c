#include "TCP.h"
#include "delay.h"

uint8_t recv_buff[RECV_BUFF_LEN];	//存储网络接收的数据
uint8_t buf[20];					//以太网模块临时获取的数据
int32_t ch395_status = 0;			//获取中断事件

/*
 * 初始化以太网模块
 */
static uint8_t init_eth() {
	uint8_t retry = 0;
	
	//初始化CH395使用的GPIO
	CH395_PORT_INIT();
	
	//复位CH395
	CH395_RST();
	mDelaymS(500);
	
	/*测试命令，按位取反返回说明测试通过*/
	while(CH395CMDCheckExist(0x55) != 0xaa) {
		delay_ms(10);
		if (retry++ > 3) {
			return ERR_ETH_SPI;
		}
	}
	delay_ms(10);
	
	//获取芯片版本
	while((CH395CMDGetVer()) < 0x44) {
		delay_ms(10);
		if (retry++ > 6) {
			return ERR_ETH_VER;
		}
	}
	
	//软复位
	CH395CMDReset();
	delay_ms(100);
	
	return 0;
}

/*
 * 初始化Socket 发送和接收缓存区，芯片有0-47(SOCKET_BUFFER_BLOCKS)个块，每个块512字节大小
 */
static void init_socket_buffer(const tcp_p tcp) {
	for (int i = 0; i < tcp->socket_max; i++) {
		/*Socket i*/
		CH395SetSocketRecvBuf(i, i * 6,		4);
		CH395SetSocketSendBuf(i, i * 6 + 4,	2);
	}
}

/*
 * 设置IP
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
 * 连接TCP
 */
static uint8_t dail_tcp(const tcp_p tcp) {
	/*让Socket0作为监听连接*/
	CH395SetSocketProtType(0, PROTO_TYPE_TCP); 		/* 协议类型 */
	CH395SetSocketSourPort(0, tcp->socket.port);  	/* 本地端口号 */
	if(CH395OpenSocket(0) != 0)                		/* 打开Socket */
	{
		return ERR_TCP_DAIL;
	}
	
	for (int i = 1; i < tcp->socket_max; i++) {
		/*其它Socket作为数据通信*/
		/*想要几路Socket客户端连接通信,就需要配置几个Socket,所以模块最多支持7个Socket TCP客户端通信*/
		CH395SetSocketProtType(i, PROTO_TYPE_TCP); 							/* 协议类型 */
		CH395SetSocketSourPort(i, tcp->socket.port);						/* 本地端口号 */
	}
	
	return 0;
}

/*
 * 启动TCP Server监听
 */
uint8_t listen_tcp(const tcp_p tcp) {
	/*配置模块启动Socket监听*/
	if (dail_tcp(tcp) != 0) {
		return ERR_TCP_DAIL;
	}
	//Socke 0 启动TCP监听
	if (CH395TCPListen(0) != 0) {
		return ERR_TCP_LISTEN;
	}
	return 0;
}

/*
 * 初始化TCP Server
 */
uint8_t init_tcp_server(const tcp_p tcp) {
	if (tcp->socket_max > 8) {
		return ERR_SOCKET_MAX;
	}
	
	//初始化以太网模块硬件设置:成功返回 0
	if (init_eth() != 0) {
		return ERR_ETH_INIT;
	}
	
	//初始化TCP SERVER多连接
	CH395SetStartPara(FUN_PARA_FLAG_TCP_SERVER);
	delay_ms(100);
	
	//初始化Socket缓冲区
	if (tcp->socket_max > 4) {
		init_socket_buffer(tcp);
	}
	delay_ms(100);
	
	//重置TCP MSS
	if (tcp->socket_max <= 4) {
		CH395SetTCPMss(MSS_MAX);
	}
	delay_ms(100);
	
	//初始化IP
	init_ip(&tcp->socket.ip);
	delay_ms(300);
	
	//初始化以太网模块软件设置:成功返回 0
	if (CH395CMDInitCH395() != 0) {
		return ERR_TCP_INIT;
	}
	
	return 0;
}

/*
 * 处理客户端（默认为echo服务器）
 */
__weak void handle_client(const uint8_t sockindex) {	
	/* 获取socket 的中断状态 */
	uint8_t sock_int_socket = CH395GetSocketInt(sockindex);
	
	/* 发送缓冲区空闲，可以继续写入要发送的数据 */
	if(sock_int_socket & SINT_STAT_SENBUF_FREE) {
		
	}
	
	/* 发送完成中断 */
	if(sock_int_socket & SINT_STAT_SEND_OK) {
		
	}
	
	/* 接收数据中断 */
	if(sock_int_socket & SINT_STAT_RECV) {
		uint16_t len = CH395GetRecvLength(sockindex);/* 获取当前缓冲区内数据长度 */
		
		if(len == 0) {
			return;
		}
		
		if(len > RECV_BUFF_LEN) {
			len = RECV_BUFF_LEN;
		}
		
		/* 读取数据 */
		CH395GetRecvData(sockindex,len,recv_buff);
		
		/*把接收的数据发送给服务器*/
		CH395SendData(sockindex,recv_buff,len);
	}
	
	/* 连接中断，仅在TCP模式下有效*/
	if(sock_int_socket & SINT_STAT_CONNECT) {
//		printf("socket%d SINT_STAT_CONNECT\r\n",sockindex);
//		CH395CMDGetRemoteIPP(sockindex,buf);//获取客户端信息
//		printf("IP address = %d.%d.%d.%d\n",(UINT16)buf[0],(UINT16)buf[1],(UINT16)buf[2],(UINT16)buf[3]);    
//		printf("Port = %d\n",((buf[5]<<8) + buf[4]));
	}
	
	/* 断开中断，仅在TCP模式下有效 */
	if(sock_int_socket & SINT_STAT_DISCONNECT) {

	}
 
	/* 超时中断，仅在TCP模式下有效*/
	if(sock_int_socket & SINT_STAT_TIM_OUT) {

	}
}

/*
 * 处理INT中断引脚（包括处理客户端）
 */
__weak void handle_INT(tcp_p tcp, void(*handle_client_p)(const uint8_t sockindex)) {
	ch395_status = CH395CMDGetGlobIntStatus_ALL();
	
	//处理PHY改变中断
	if(ch395_status & GINT_STAT_PHY_CHANGE) {
		//网线断开
		if(CH395CMDGetPHYStatus() == PHY_DISCONN) {

		}
		//网线连接
		else {

		}
	}
	
	/* 处理不可达中断，读取不可达信息 */
	if(ch395_status & GINT_STAT_UNREACH) {
		CH395CMDGetUnreachIPPT(buf);
	}
	
	/* 处理IP冲突中断，建议重新修改CH395的 IP，并初始化CH395*/
	if(ch395_status & GINT_STAT_IP_CONFLI) {
		static uint8_t ip_addr_3 = 1; 
		
		tcp->socket.ip.ip_addr[3] += ip_addr_3++;
		init_tcp_server(tcp);
		listen_tcp(tcp);
		
		return;
	}
	
	/* 处理 SOCKi 中断 */
	for(int i = 0; i < tcp->socket_max; i++) {
		//if(ch395_status & GINT_STAT_SOCK0){}
		if(ch395_status & (1 << (4 + i))) {
			(*handle_client_p)(i);
		}
	}
}

/*
 * 运行TCP Server（轮询）（包括所有中断）
 */
__weak void process_tcp_server(const tcp_p tcp) {
	//INT引脚产生低电平中断以后进去判断
	if(Query395Interrupt()) {
		handle_INT(tcp, handle_client);
	}
}

/*
 * 数据接收（中断）
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
	
	//读取数据
	CH395GetRecvData(sockindex, buf_len, buf);
	
	return len;
}

/*
 * 数据发送
 */
void send_data(uint8_t sockindex, uint8_t buf[], const uint16_t buf_len) {
	CH395SendData(sockindex, buf, buf_len);
}

/*
 * 关闭连接
 */
uint8_t close_tcp() {
	//Socke 0 关闭TCP连接
	if (CH395TCPDisconnect(0) != 0) {
		return ERR_TCP_CLOSE;
	}
	return 0;
}
