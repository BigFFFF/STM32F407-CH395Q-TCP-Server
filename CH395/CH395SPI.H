
#ifndef __CH395SPI_H_
#define __CH395SPI_H_

#include "CH395INC.H"

/*******************************配置GPIO (根据自己的修改)****************************************/
//设置使用的SPI
#define USE_SPI SPI2
//SPI_CS  --  连接模块SCS引脚
#define CH395_CS_PORT    GPIOA
#define CH395_CS_PIN     GPIO_Pin_3
//SPI_CLK --   连接模块SCK引脚
#define CH395_CLK_PORT   GPIOB
#define CH395_CLK_PIN    GPIO_Pin_10
//SPI_MISO --  连接模块SDO引脚
#define CH395_MISO_PORT  GPIOC
#define CH395_MISO_PIN   GPIO_Pin_2
//SPI_MOSI --  连接模块SDI引脚
#define CH395_MOSI_PORT  GPIOC
#define CH395_MOSI_PIN   GPIO_Pin_3
//RST --  连接模块RST引脚
#define CH395_RST_PORT  GPIOA
#define CH395_RST_PIN   GPIO_Pin_4
//TX --  连接模块TX引脚
#define CH395_TX_PORT  GPIOA
#define CH395_TX_PIN   GPIO_Pin_5
//INT  -- 连接模块INT引脚 (检测到该引脚低电平信号之后再获取数据)
#define CH395_INT_PORT  GPIOA
#define CH395_INT_PIN   GPIO_Pin_6
/***********************************************************************/

#define CH395_SPI_CS_LOW()       {GPIO_ResetBits(CH395_CS_PORT,CH395_CS_PIN);}  /*CS输出低*/
#define CH395_SPI_CS_HIGH()      {GPIO_SetBits(CH395_CS_PORT,CH395_CS_PIN);}    /*CS输出高*/

#define CH395_RST_PIN_HIGH()     {GPIO_SetBits(CH395_RST_PORT,CH395_RST_PIN);}   /*RST输出高*/
#define CH395_RST_PIN_LOW()      {GPIO_ResetBits(CH395_RST_PORT,CH395_RST_PIN);} /*RST输出低*/

#define CH395_TX_PIN_HIGH()      {GPIO_SetBits(CH395_TX_PORT,CH395_TX_PIN);}    /*TX输出高*/
#define CH395_TX_PIN_LOW()       {GPIO_ResetBits(CH395_TX_PORT,CH395_TX_PIN);}  /*TX输出低*/

#define CH395_INT_PIN_INPUT()    GPIO_ReadInputDataBit(CH395_INT_PORT, CH395_INT_PIN) /* 获取INT电平 */   

/***************************************************************GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)***************
* Function Name  : Delay_uS
* Description    : 微秒级延时函数(基本准确)
* Input          : delay---延时值
* Output         : None
* Return         : None
*******************************************************************************/
void mDelayuS( UINT32 delay );

/*******************************************************************************
* Function Name  : Delay_mS
* Description    : 毫秒级延时函数(基本准确)
* Input          : delay---延时值
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
* Description    : CH395端口初始化
*                  由于使用SPI读写时序,所以进行初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH395_PORT_INIT( void );


/*******************************************************************************
* Function Name  : xWriteCH395Cmd
* Description    : 向CH395写命令
* Input          : mCmd---将要写入CH395的命令码
* Output         : None
* Return         : None
*******************************************************************************/
void xWriteCH395Cmd( UINT8 mCmd );



/*******************************************************************************
* Function Name  : xWriteCH395Data
* Description    : 向CH395写数据
* Input          : mData---将要写入CH395的数据
* Output         : None
* Return         : None
*******************************************************************************/
void xWriteCH395Data( UINT8 mData );

/*******************************************************************************
* Function Name  : xReadCH395Data
* Description    : 从CH395读数据
* Input          : None
* Output         : None
* Return         : 返回读取的数据
*******************************************************************************/
UINT8 xReadCH395Data( void );

/*******************************************************************************
* Function Name  : CH395_RST
* Description    : 复位 CH395
* Input          : None
* Output         : None
* Return         : 返回中断状态
*******************************************************************************/
void CH395_RST( void );


UINT8 Query395Interrupt( void );

#endif

