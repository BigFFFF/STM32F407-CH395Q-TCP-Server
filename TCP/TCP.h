#ifndef __TCP_H_
#define __TCP_H_

#include <stdint.h>

#include "CH395SPI.H"
#include "CH395INC.H"
#include "CH395CMD.H"

//socket����������(Socket ���ͺͽ��ջ�������оƬ��0-47���飬ÿ����512�ֽڴ�С)
#define SOCKET_BUFFER_BLOCKS	48

//MSS
#define MSS_MAX					1460U
#define MSS_MIN					60U
#define MSS_DEFAULT				800U

//������
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

//�����޸ĵĽ��ջ�������С��socket���ֵ�����ض˿ںţ�IP��ַ�����أ���������
#define RECV_BUFF_LEN 			1460U
#define SOCKET_MAX				8
#define PORT_LADDR				8080
#define IP_LADDR				{192,168,1,100}
#define GATE_WAY				{192,1,1,1}
#define	IP_MASK					{255,255,255,0}

extern uint8_t recv_buff[RECV_BUFF_LEN];	//�洢������յ�����
extern int32_t ch395_status;							//��ȡ�ж��¼�

//IP
typedef struct {
	uint8_t  	ip_addr[4];               	//IP��ַ
	uint8_t  	gate_way[4];             		//����
	uint8_t  	ip_mask[4];           			//��������
}ip_t, *ip_p;

//Socket
typedef struct {
	ip_t		ip;
	uint16_t 	port;												//�˿ں�
}socket_t, *socket_p;

//TCPServer
typedef struct {
	uint8_t		socket_max;									//socket���ֵ��Ĭ��Ϊ8����Χ0-8
	socket_t  	socket;
}tcp_t, *tcp_p;


/*******************************************************************************
* Description    : ��ʼ��TCP Server
* Arguments      : tcp_p�ṹ��ָ�루����TCP Server������Ϣ��
* Return         : 0Ϊ�ɹ�
*******************************************************************************/
uint8_t init_tcp_server(const tcp_p tcp);


/*******************************************************************************
* Description    : ����TCP Server����
* Arguments      : tcp_p�ṹ��ָ�루����TCP Server������Ϣ��
* Return         : 0Ϊ�ɹ�
*******************************************************************************/
uint8_t listen_tcp(const tcp_p tcp);


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
uint8_t recv_data(uint8_t sockindex, uint8_t buf[], uint16_t buf_len);
 

/*******************************************************************************
* Description    : ���ݷ���
* Arguments      : socket�����ţ������������ݳ���
* Return         : None
*******************************************************************************/
void send_data(uint8_t sockindex, uint8_t buf[], uint16_t buf_len);
 
 
/*******************************************************************************
* Description    : �ر�����
* Arguments      : None
* Return         : 0Ϊ�ɹ�
*******************************************************************************/
uint8_t close_tcp(void);


/*******************************************************************************
* Description    : ����ͻ���
* Arguments      : tcp_p�ṹ��ָ�루����TCP Server������Ϣ��
* Return         : None
*******************************************************************************/
void handle_client(const uint8_t sockindex);


/*******************************************************************************
* Description    : ����INT�ж����ţ���������ͻ��ˣ�
* Arguments      : tcp_p�ṹ��ָ�루����TCP Server������Ϣ����*handle_client_p����ָ�루ָ���ͻ��˴�������
* Return         : None
*******************************************************************************/
void handle_INT(tcp_p tcp, void(*handle_client_p)(const uint8_t sockindex));


#endif
