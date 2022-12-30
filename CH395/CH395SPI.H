
#ifndef __CH395SPI_H_
#define __CH395SPI_H_

#include "CH395INC.H"

/*******************************����GPIO (�����Լ����޸�)****************************************/
//����ʹ�õ�SPI
#define USE_SPI SPI2
//SPI_CS  --  ����ģ��SCS����
#define CH395_CS_PORT    GPIOA
#define CH395_CS_PIN     GPIO_Pin_3
//SPI_CLK --   ����ģ��SCK����
#define CH395_CLK_PORT   GPIOB
#define CH395_CLK_PIN    GPIO_Pin_10
//SPI_MISO --  ����ģ��SDO����
#define CH395_MISO_PORT  GPIOC
#define CH395_MISO_PIN   GPIO_Pin_2
//SPI_MOSI --  ����ģ��SDI����
#define CH395_MOSI_PORT  GPIOC
#define CH395_MOSI_PIN   GPIO_Pin_3
//RST --  ����ģ��RST����
#define CH395_RST_PORT  GPIOA
#define CH395_RST_PIN   GPIO_Pin_4
//TX --  ����ģ��TX����
#define CH395_TX_PORT  GPIOA
#define CH395_TX_PIN   GPIO_Pin_5
//INT  -- ����ģ��INT���� (��⵽�����ŵ͵�ƽ�ź�֮���ٻ�ȡ����)
#define CH395_INT_PORT  GPIOA
#define CH395_INT_PIN   GPIO_Pin_6
/***********************************************************************/

#define CH395_SPI_CS_LOW()       {GPIO_ResetBits(CH395_CS_PORT,CH395_CS_PIN);}  /*CS�����*/
#define CH395_SPI_CS_HIGH()      {GPIO_SetBits(CH395_CS_PORT,CH395_CS_PIN);}    /*CS�����*/

#define CH395_RST_PIN_HIGH()     {GPIO_SetBits(CH395_RST_PORT,CH395_RST_PIN);}   /*RST�����*/
#define CH395_RST_PIN_LOW()      {GPIO_ResetBits(CH395_RST_PORT,CH395_RST_PIN);} /*RST�����*/

#define CH395_TX_PIN_HIGH()      {GPIO_SetBits(CH395_TX_PORT,CH395_TX_PIN);}    /*TX�����*/
#define CH395_TX_PIN_LOW()       {GPIO_ResetBits(CH395_TX_PORT,CH395_TX_PIN);}  /*TX�����*/

#define CH395_INT_PIN_INPUT()    GPIO_ReadInputDataBit(CH395_INT_PORT, CH395_INT_PIN) /* ��ȡINT��ƽ */   

/***************************************************************GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)***************
* Function Name  : Delay_uS
* Description    : ΢�뼶��ʱ����(����׼ȷ)
* Input          : delay---��ʱֵ
* Output         : None
* Return         : None
*******************************************************************************/
void mDelayuS( UINT32 delay );

/*******************************************************************************
* Function Name  : Delay_mS
* Description    : ���뼶��ʱ����(����׼ȷ)
* Input          : delay---��ʱֵ
* Output         : None
* Return         : None
*******************************************************************************/
void mDelaymS( UINT16 delay );

/*
 * FLASH_CS
 */
void W25QXX_DeInit( void );

/*******************************************************************************
* Function Name  : CH395_Port_Init
* Description    : CH395�˿ڳ�ʼ��
*                  ����ʹ��SPI��дʱ��,���Խ��г�ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH395_PORT_INIT( void );


/*******************************************************************************
* Function Name  : xWriteCH395Cmd
* Description    : ��CH395д����
* Input          : mCmd---��Ҫд��CH395��������
* Output         : None
* Return         : None
*******************************************************************************/
void xWriteCH395Cmd( UINT8 mCmd );



/*******************************************************************************
* Function Name  : xWriteCH395Data
* Description    : ��CH395д����
* Input          : mData---��Ҫд��CH395������
* Output         : None
* Return         : None
*******************************************************************************/
void xWriteCH395Data( UINT8 mData );

/*******************************************************************************
* Function Name  : xReadCH395Data
* Description    : ��CH395������
* Input          : None
* Output         : None
* Return         : ���ض�ȡ������
*******************************************************************************/
UINT8 xReadCH395Data( void );

/*******************************************************************************
* Function Name  : CH395_RST
* Description    : ��λ CH395
* Input          : None
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
void CH395_RST( void );


UINT8 Query395Interrupt( void );

#endif
