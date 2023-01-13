#ifndef __TCP_H_
#define __TCP_H_

#include <stdint.h>

#include "CH395SPI.H"
#include "CH395INC.H"
#include "CH395CMD.H"

//socket缓冲区块数(Socket 发送和接收缓存区，芯片有0-47个块，每个块512字节大小)
#define SOCKET_BUFFER_BLOCKS	48

//MSS
#define MSS_MAX					1460
#define MSS_MIN					60
#define MSS_DEFAULT				980

//错误码
#define ERR_ETH_VER				11
#define ERR_ETH_SPI				12
#define ERR_ETH_INIT			13

#define ERR_TCP_CONFIG			21
#define ERR_TCP_INIT			22
#define ERR_TCP_DAIL			23
#define ERR_TCP_LISTEN			24
#define ERR_TCP_CLOSE			25
#define ERR_TCP_BUFF			26
#define ERR_TCP_RECV			27

//可以修改的接收缓冲区大小，socket最大值，本地端口号，IP地址，网关，子网掩码
#define RECV_BUFF_LEN 			980
#define IP_LADDR				{192,168,1,100}
#define GATE_WAY				{192,1,1,1}
#define	IP_MASK					{255,255,255,0}

//socket范围为0-8，监听端口的范围为0-4
#define SOCKET_MAX				8
#define SOCKET_LISTEN_MAX		3

//监听端口总和是SOCKET_LISTEN_MAX，单个监听端口至少保证一个连接数
#define PORT_LADDR_0			8080
#define PORT_LADDR_1			8081
#define PORT_LADDR_2			8082

//连接数总和是SOCKET_MAX - SOCKET_LISTEN_MAX
#define CONNECT_NUM_0			3
#define CONNECT_NUM_1			1
#define CONNECT_NUM_2			1

//IP
typedef struct {
	uint8_t  	ip_addr[4];               			//IP地址
	uint8_t  	gate_way[4];             			//网关
	uint8_t  	ip_mask[4];           				//子网掩码
}ip_t, *ip_p;

//Socket
typedef struct {
	uint8_t  	ip_addr[4];
	uint16_t 	port;								//端口号
	uint8_t		socketi;							//socket索引（0-SOCKET_LISTEN_MAX）
}socket_t, *socket_p;

//TCP Listen Port
typedef struct {
	uint16_t 	port;								//端口号
	uint8_t		socketi;							//socket索引（0-SOCKET_LISTEN_MAX）
	uint8_t		connect_num;						//每个端口的最大连接数
}listen_t, *listen_p;

//TCP Server
typedef struct {
	ip_t		ip;
	listen_t  	listen[SOCKET_LISTEN_MAX];
	uint8_t		socket_max;							//socket最大值
	uint8_t		socket_listen_max;					//socket监听端口最大值
}tcp_t, *tcp_p;


extern uint8_t 				recv_buff[RECV_BUFF_LEN];	//接收缓冲区
extern volatile uint8_t 	socket_int[SOCKET_MAX];		//socket中断状态
extern volatile uint16_t 	ch395_status;				//CH395中断状态


/*******************************************************************************
* Description    : 初始化TCP Server
* Arguments      : tcp_p结构体指针（储存TCP Server配置信息）
* Return         : 0为成功
*******************************************************************************/
uint8_t init_tcp_server(const tcp_p tcp);


/*******************************************************************************
* Description    : 启动TCP Server监听
* Arguments      : listen_p结构体指针（储存监听端口配置信息）
* Return         : 0为成功
*******************************************************************************/
uint8_t listen_tcp(const listen_p listen);


/*******************************************************************************
* Description    : 运行TCP Server(轮询)
* Arguments      : tcp_p结构体指针（储存TCP Server配置信息）
* Return         : None
*******************************************************************************/
void process_tcp_server(const tcp_p tcp);


/*******************************************************************************
* Description    : 数据接收（中断）
* Arguments      : socket索引号，缓冲区，数据长度（长度大于等于RECV_BUFF_LEN时，不定长接收）
* Return         : 实际接收的数据长度
*******************************************************************************/
uint16_t recv_data(const uint8_t sockindex, uint8_t buf[], uint16_t buf_len);
 

/*******************************************************************************
* Description    : 数据发送
* Arguments      : socket索引号，缓冲区，数据长度
* Return         : 实际发送的数据长度
*******************************************************************************/
uint16_t send_data(const uint8_t sockindex, uint8_t buf[], uint16_t buf_len);
 
 
/*******************************************************************************
* Description    : 关闭连接
* Arguments      : socket索引号
* Return         : 0为成功
*******************************************************************************/
uint8_t close_tcp(const uint8_t sockindex);


/*******************************************************************************
* Description    : 处理客户端
* Arguments      : tcp_p结构体指针（储存TCP Server配置信息）
* Return         : None
*******************************************************************************/
void handle_client(const uint8_t sockindex);


/*******************************************************************************
* Description    : 监听接收（包括处理客户端）
* Arguments      : listen_p结构体指针，*handle_client_p函数指针（指定客户端处理函数）
* Return         : None
*******************************************************************************/
void listen_accept(listen_p listen,  void(*handle_client_p)(const uint8_t sockindex));


/*******************************************************************************
* Description    : 处理INT中断引脚（包括处理监听端口）
* Arguments      : tcp_p结构体指针（储存TCP Server配置信息）
* Return         : None
*******************************************************************************/
void handle_INT(tcp_p tcp);


#endif
