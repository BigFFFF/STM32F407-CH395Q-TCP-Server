#include "TCP.h"
#include "delay.h"

uint8_t 			recv_buff[RECV_BUFF_LEN];	//接收缓存区
volatile uint8_t 	socket_int[SOCKET_MAX];		//socket中断状态
volatile uint16_t 	ch395_status = 0;			//CH395中断状态

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
static uint8_t dail_tcp(const listen_p listen) {
	/*让listen->socketi作为监听连接*/
	CH395SetSocketProtType(listen->socketi, PROTO_TYPE_TCP); 		/* 协议类型 */
	CH395SetSocketSourPort(listen->socketi, listen->port); 			/* 本地端口号 */
	if(CH395OpenSocket(listen->socketi) != 0)                		/* 打开Socket */
	{
		return ERR_TCP_DAIL;
	}
	
	for (int i = listen->socketi + 1; i < listen->socketi + 1 + listen->connect_num; i++) {
		/*其它Socket作为数据通信*/
		/*想要几路Socket客户端连接通信,就需要配置几个Socket,所以模块最多支持7个Socket TCP客户端通信*/
		CH395SetSocketProtType(i, PROTO_TYPE_TCP); 						/* 协议类型 */
		CH395SetSocketSourPort(i, listen->port);						/* 本地端口号 */
	}
	
	return 0;
}

/*
 * 启动TCP Server监听
 */
uint8_t listen_tcp(const listen_p listen) {
	/*配置模块启动Socket监听*/
	if (dail_tcp(listen) != 0) {
		return ERR_TCP_DAIL;
	}
	//listen->socketi 启动TCP监听
	if (CH395TCPListen(listen->socketi) != 0) {
		return ERR_TCP_LISTEN;
	}
	return 0;
}


/*
 * 检查TCP最基本的配置
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
 * 初始化TCP Server
 */
uint8_t init_tcp_server(const tcp_p tcp) {
	//检查TCP的基本配置
	if (check_tcp_config(tcp) != 0) {
		return ERR_TCP_CONFIG;
	}
	
	//初始化以太网模块硬件设置
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
	CH395SetTCPMss(MSS_DEFAULT);
	delay_ms(100);
	
	//初始化IP
	init_ip(&tcp->ip);
	delay_ms(300);
	
	//初始化以太网模块软件设置
	if (CH395CMDInitCH395() != 0) {
		return ERR_TCP_INIT;
	}
	
	return 0;
}

/*
 * 运行TCP Server（轮询）（包括所有中断）
 */
void process_tcp_server(const tcp_p tcp) {
	//INT引脚产生低电平中断以后进去判断
	if(Query395Interrupt()) {
		//获取CH395全局中断状态
		ch395_status = CH395CMDGetGlobIntStatus_ALL();
		//处理中断
		handle_INT(tcp);
	}
}

/*
 * 监听端口接收（包括处理客户端）
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
 * 处理客户端（默认为echo服务器）
 */
void handle_client(const uint8_t sockindex) {	
	/* 发送完成中断 */
	if(socket_int[sockindex] & SINT_STAT_SEND_OK) {
		
	}
	
	/* 发送缓冲区空闲，可以继续写入要发送的数据 */
	if(socket_int[sockindex] & SINT_STAT_SENBUF_FREE) {
		
	}
	
	/* 接收数据中断 */
	if(socket_int[sockindex] & SINT_STAT_RECV) {
		uint16_t len = CH395GetRecvLength(sockindex);/* 获取当前缓冲区内数据长度 */
		
		if(len == 0) {
			return;
		}
		
		if(len > RECV_BUFF_LEN) {
			len = RECV_BUFF_LEN;
		}
		
		/* 读取数据 */
		CH395GetRecvData(sockindex, len, recv_buff);
		
		/* 发送数据 */
		CH395SendData(sockindex, recv_buff, len);
	}
	
	/* 连接中断，仅在TCP模式下有效 */
	if(socket_int[sockindex] & SINT_STAT_CONNECT) {

	}
	
	/* 断开中断，仅在TCP模式下有效 */
	if(socket_int[sockindex] & SINT_STAT_DISCONNECT) {

	}
 
	/* 超时中断，仅在TCP模式下有效 */
	if(socket_int[sockindex] & SINT_STAT_TIM_OUT) {

	}
}

/*
 * 处理INT中断引脚（包括处理监听的端口）
 */
__weak void handle_INT(tcp_p tcp) {
	/* 处理PHY改变中断 */
	if(ch395_status & GINT_STAT_PHY_CHANGE) {
		/* 网线断开 */
		if(CH395CMDGetPHYStatus() == PHY_DISCONN) {

		}
		/* 网线连接 */
		else {

		}
	}
	
	/* 处理不可达中断，读取不可达信息 */
	if(ch395_status & GINT_STAT_UNREACH) {

	}
	
	/* 处理IP冲突中断，建议重新修改CH395的 IP，并初始化CH395*/
	if(ch395_status & GINT_STAT_IP_CONFLI) {

	}
	/* 处理 监听到的客户端 中断 */
	listen_accept(&tcp->listen[0], handle_client);
}

/*
 * 数据接收（中断）
 */
uint16_t recv_data(const uint8_t sockindex, uint8_t buf[], uint16_t buf_len) {
	//socket端口接收到数据或者接收缓冲区不为空
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
	
	//读取数据
	CH395GetRecvData(sockindex, buf_len, buf);
	
	return len;
}

/*
 * 数据发送
 */
uint16_t send_data(const uint8_t sockindex, uint8_t buf[], uint16_t buf_len) {
	if (buf_len > MSS_DEFAULT) {
		buf_len = MSS_DEFAULT;
	}
	CH395SendData(sockindex, buf, buf_len);
	return buf_len;
}

/*
 * 关闭连接
 */
uint8_t close_tcp(const uint8_t sockindex) {
	//Socke 0 关闭TCP连接
	if (CH395TCPDisconnect(sockindex) != 0) {
		return ERR_TCP_CLOSE;
	}
	return 0;
}
