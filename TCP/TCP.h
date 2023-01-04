#ifndef __TCP_H_
#define __TCP_H_

#include <stdint.h>

#include "CH395SPI.H"
#include "CH395INC.H"
#include "CH395CMD.H"

//socket缓冲区块数(Socket 发送和接收缓存区，芯片有0-47个块，每个块512字节大小)
#define SOCKET_BUFFER_BLOCKS	48

//MSS
#define MSS_MAX					1460U
#define MSS_MIN					60U
#define MSS_DEFAULT				800U

//错误码
#define ERR_ETH_VER				11
#define ERR_ETH_SPI				12
#define ERR_ETH_INIT			13

#define ERR_SOCKET_MAX			21

#define ERR_TCP_INIT			31
#define ERR_TCP_DAIL			32
#define ERR_TCP_LISTEN			33
#define ERR_TCP_CLOSE			34
#define ERR_TCP_BUFF			35
#define ERR_TCP_RECV			36

//可以修改的接收缓冲区大小，socket最大值，本地端口号，IP地址，网关，子网掩码
#define RECV_BUFF_LEN 			1460U
#define SOCKET_MAX				8
#define PORT_LADDR				8080
#define IP_LADDR				{192,168,1,100}
#define GATE_WAY				{192,1,1,1}
#define	IP_MASK					{255,255,255,0}

extern uint8_t recv_buff[RECV_BUFF_LEN];	//存储网络接收的数据
extern int32_t ch395_status;							//获取中断事件

//IP
typedef struct {
	uint8_t  	ip_addr[4];               	//IP地址
	uint8_t  	gate_way[4];             		//网关
	uint8_t  	ip_mask[4];           			//子网掩码
}ip_t, *ip_p;

//Socket
typedef struct {
	ip_t		ip;
	uint16_t 	port;												//端口号
}socket_t, *socket_p;

//TCPServer
typedef struct {
	uint8_t		socket_max;									//socket最大值，默认为8，范围0-8
	socket_t  	socket;
}tcp_t, *tcp_p;


/*******************************************************************************
* Description    : 初始化TCP Server
* Arguments      : tcp_p结构体指针（储存TCP Server配置信息）
* Return         : 0为成功
*******************************************************************************/
uint8_t init_tcp_server(const tcp_p tcp);


/*******************************************************************************
* Description    : 启动TCP Server监听
* Arguments      : tcp_p结构体指针（储存TCP Server配置信息）
* Return         : 0为成功
*******************************************************************************/
uint8_t listen_tcp(const tcp_p tcp);


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
uint8_t recv_data(uint8_t sockindex, uint8_t buf[], uint16_t buf_len);
 

/*******************************************************************************
* Description    : 数据发送
* Arguments      : socket索引号，缓冲区，数据长度
* Return         : None
*******************************************************************************/
void send_data(uint8_t sockindex, uint8_t buf[], uint16_t buf_len);
 
 
/*******************************************************************************
* Description    : 关闭连接
* Arguments      : None
* Return         : 0为成功
*******************************************************************************/
uint8_t close_tcp(void);


/*******************************************************************************
* Description    : 处理客户端
* Arguments      : tcp_p结构体指针（储存TCP Server配置信息）
* Return         : None
*******************************************************************************/
void handle_client(const uint8_t sockindex);


/*******************************************************************************
* Description    : 处理INT中断引脚（包括处理客户端）
* Arguments      : tcp_p结构体指针（储存TCP Server配置信息），*handle_client_p函数指针（指定客户端处理函数）
* Return         : None
*******************************************************************************/
void handle_INT(tcp_p tcp, void(*handle_client_p)(const uint8_t sockindex));


#endif
