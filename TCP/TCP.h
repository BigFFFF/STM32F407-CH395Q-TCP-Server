#ifndef __TCP_H_
#define __TCP_H_

#include <stdint.h>

#include "CH395SPI.H"
#include "CH395INC.H"
#include "CH395CMD.H"

//socket����������(Socket ���ͺͽ��ջ�������оƬ��0-47���飬ÿ����512�ֽڴ�С)
#define SOCKET_BUFFER_BLOCKS	48

//MSS
#define MSS_MAX					1460
#define MSS_MIN					60
#define MSS_DEFAULT				980

//������
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

//�����޸ĵĽ��ջ�������С��socket���ֵ�����ض˿ںţ�IP��ַ�����أ���������
#define RECV_BUFF_LEN 			980
#define IP_LADDR				{192,168,1,100}
#define GATE_WAY				{192,1,1,1}
#define	IP_MASK					{255,255,255,0}

//socket��ΧΪ0-8�������˿ڵķ�ΧΪ0-4
#define SOCKET_MAX				8
#define SOCKET_LISTEN_MAX		3

//�����˿��ܺ���SOCKET_LISTEN_MAX�����������˿����ٱ�֤һ��������
#define PORT_LADDR_0			8080
#define PORT_LADDR_1			8081
#define PORT_LADDR_2			8082

//�������ܺ���SOCKET_MAX - SOCKET_LISTEN_MAX
#define CONNECT_NUM_0			3
#define CONNECT_NUM_1			1
#define CONNECT_NUM_2			1

//IP
typedef struct {
	uint8_t  	ip_addr[4];               			//IP��ַ
	uint8_t  	gate_way[4];             			//����
	uint8_t  	ip_mask[4];           				//��������
}ip_t, *ip_p;

//Socket
typedef struct {
	uint8_t  	ip_addr[4];
	uint16_t 	port;								//�˿ں�
	uint8_t		socketi;							//socket������0-SOCKET_LISTEN_MAX��
}socket_t, *socket_p;

//TCP Listen Port
typedef struct {
	uint16_t 	port;								//�˿ں�
	uint8_t		socketi;							//socket������0-SOCKET_LISTEN_MAX��
	uint8_t		connect_num;						//ÿ���˿ڵ����������
}listen_t, *listen_p;

//TCP Server
typedef struct {
	ip_t		ip;
	listen_t  	listen[SOCKET_LISTEN_MAX];
	uint8_t		socket_max;							//socket���ֵ
	uint8_t		socket_listen_max;					//socket�����˿����ֵ
}tcp_t, *tcp_p;


extern uint8_t 				recv_buff[RECV_BUFF_LEN];	//���ջ�����
extern volatile uint8_t 	socket_int[SOCKET_MAX];		//socket�ж�״̬
extern volatile uint16_t 	ch395_status;				//CH395�ж�״̬


/*******************************************************************************
* Description    : ��ʼ��TCP Server
* Arguments      : tcp_p�ṹ��ָ�루����TCP Server������Ϣ��
* Return         : 0Ϊ�ɹ�
*******************************************************************************/
uint8_t init_tcp_server(const tcp_p tcp);


/*******************************************************************************
* Description    : ����TCP Server����
* Arguments      : listen_p�ṹ��ָ�루��������˿�������Ϣ��
* Return         : 0Ϊ�ɹ�
*******************************************************************************/
uint8_t listen_tcp(const listen_p listen);


/*******************************************************************************
* Description    : ����TCP Server(��ѯ)
* Arguments      : tcp_p�ṹ��ָ�루����TCP Server������Ϣ��
* Return         : None
*******************************************************************************/
void process_tcp_server(const tcp_p tcp);


/*******************************************************************************
* Description    : ���ݽ��գ��жϣ�
* Arguments      : socket�����ţ������������ݳ��ȣ����ȴ��ڵ���RECV_BUFF_LENʱ�����������գ�
* Return         : ʵ�ʽ��յ����ݳ���
*******************************************************************************/
uint16_t recv_data(const uint8_t sockindex, uint8_t buf[], uint16_t buf_len);
 

/*******************************************************************************
* Description    : ���ݷ���
* Arguments      : socket�����ţ������������ݳ���
* Return         : ʵ�ʷ��͵����ݳ���
*******************************************************************************/
uint16_t send_data(const uint8_t sockindex, uint8_t buf[], uint16_t buf_len);
 
 
/*******************************************************************************
* Description    : �ر�����
* Arguments      : socket������
* Return         : 0Ϊ�ɹ�
*******************************************************************************/
uint8_t close_tcp(const uint8_t sockindex);


/*******************************************************************************
* Description    : ����ͻ���
* Arguments      : tcp_p�ṹ��ָ�루����TCP Server������Ϣ��
* Return         : None
*******************************************************************************/
void handle_client(const uint8_t sockindex);


/*******************************************************************************
* Description    : �������գ���������ͻ��ˣ�
* Arguments      : listen_p�ṹ��ָ�룬*handle_client_p����ָ�루ָ���ͻ��˴�������
* Return         : None
*******************************************************************************/
void listen_accept(listen_p listen,  void(*handle_client_p)(const uint8_t sockindex));


/*******************************************************************************
* Description    : ����INT�ж����ţ�������������˿ڣ�
* Arguments      : tcp_p�ṹ��ָ�루����TCP Server������Ϣ��
* Return         : None
*******************************************************************************/
void handle_INT(tcp_p tcp);


#endif
