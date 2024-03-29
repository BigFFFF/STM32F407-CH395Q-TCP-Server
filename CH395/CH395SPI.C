/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI_HW.C
* Author             : WXF
* Version            : V1.0
* Date               : 2013/12/19
* Description        : CH395оƬ CH395оƬ Ӳ����׼SPI�������ӵ�Ӳ������� V1.0
*                      �ṩI/O�ӿ��ӳ���
*******************************************************************************/
#include "CH395SPI.H"
#include "delay.h"
#include "CH395INC.H"

/*******************************************************************************
* Function Name  : Delay_uS
* Description    : ΢�뼶��ʱ����(����׼ȷ)
* Input          : delay---��ʱֵ
* Output         : None
* Return         : None
*******************************************************************************/
void mDelayuS(UINT32 delay)
{
	//�滻�Լ�����ʱus����
	delay_us(delay);
}

/*******************************************************************************
* Function Name  : Delay_mS
* Description    : ���뼶��ʱ����(����׼ȷ)
* Input          : delay---��ʱֵ
* Output         : None
* Return         : None
*******************************************************************************/
void mDelaymS(UINT16 delay)
{
	//�滻�Լ�����ʱms����
	delay_ms(delay);
}
				 
/*
 * FLASH_CS
 */
void W25QXX_DeInit(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
 	GPIO_SetBits(GPIOE, GPIO_Pin_3);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : ��ʼ������SSI�Ĺܽ�
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void SPI2_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_DeInit(SPI2);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//MOSI---C3
	GPIO_InitStructure.GPIO_Pin = CH395_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(CH395_MOSI_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(CH395_MOSI_PORT, GPIO_PinSource3, GPIO_AF_SPI2);
	
	//MISO---C2
	GPIO_InitStructure.GPIO_Pin = CH395_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(CH395_MISO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(CH395_MISO_PORT, GPIO_PinSource2, GPIO_AF_SPI2); 
	
	//SCK---B10
	GPIO_InitStructure.GPIO_Pin = CH395_CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(CH395_CLK_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(CH395_CLK_PORT, GPIO_PinSource10, GPIO_AF_SPI2); 

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; 
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; 
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI_InitStructure);
	
	/* Enable SPI2 */ 
	SPI_Cmd(SPI2, ENABLE);
}

/*******************************************************************************
* Function Name  : CH395_Port_Init
* Description    : CH395�˿ڳ�ʼ��
*                  ����ʹ��SPI��дʱ��,���Խ��г�ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH395_PORT_INIT( void )  
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* ��ʼ��GPIO�ӿ� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	
	
	// Configure pins: CS
	GPIO_InitStructure.GPIO_Pin = CH395_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;/* ����������ù��� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(CH395_CS_PORT, &GPIO_InitStructure);
	
	// Configure pins: TX
	GPIO_InitStructure.GPIO_Pin = CH395_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;/* ����������ù��� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(CH395_TX_PORT, &GPIO_InitStructure);
	
	// Configure pins: RST
	GPIO_InitStructure.GPIO_Pin = CH395_RST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;/* ����������ù��� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(CH395_RST_PORT, &GPIO_InitStructure);
	
	//TX low
	CH395_TX_PIN_LOW();
	/*CS high */
	CH395_SPI_CS_HIGH();
	
	SPI2_Init();
	
	/* ��ʼ���ж����� */
	GPIO_InitStructure.GPIO_Pin = CH395_INT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  				 /* �������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(CH395_INT_PORT, &GPIO_InitStructure );		
}

/*******************************************************************************
* Function Name  : Spi395Exchange
* Description    : Ӳ��SPI���������8��λ����
* Input          : d---��Ҫ���뵽CH395������
* Output         : None
* Return         : SPI���յ�����
*******************************************************************************/
UINT8 Spi395Exchange( UINT8 d )  
{	
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(USE_SPI, SPI_I2S_FLAG_TXE) == RESET);	
	
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(USE_SPI, d);

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(USE_SPI, SPI_I2S_FLAG_RXNE) == RESET);

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(USE_SPI);
}

/*******************************************************************************
* Function Name  : xWriteCH395Cmd
* Description    : ��CH395д����
* Input          : mCmd---��Ҫд��CH395��������
* Output         : None
* Return         : None
*******************************************************************************/
void xWriteCH395Cmd(UINT8 mCmd)  
{
	CH395_SPI_CS_HIGH();  									 /* ��ֹ֮ǰδͨ��xEndCH395Cmd��ֹSPIƬѡ */
	mDelayuS(1);
	/* ����˫��I/O����ģ��SPI�ӿ�,��ô����ȷ���Ѿ�����SPI_SCS,SPI_SCK,SPI_SDIΪ�������,SPI_SDOΪ���뷽�� */
	CH395_SPI_CS_LOW();  										 /* SPIƬѡ��Ч */
	
	/* ���������� */
	Spi395Exchange(mCmd);  									 /* ���������� */
	mDelayuS(2);  											 /* ��ʱ1.5uSȷ����д���ڴ���1.5uS */
}

/*******************************************************************************
* Function Name  : xWriteCH395Data
* Description    : ��CH395д����
* Input          : mData---��Ҫд��CH395������
* Output         : None
* Return         : None
*******************************************************************************/
void xWriteCH395Data(UINT8 mData) 
{
	Spi395Exchange(mData);  									 /* �������� */

}

/*******************************************************************************
* Function Name  : xReadCH395Data
* Description    : ��CH395������
* Input          : None
* Output         : None
* Return         : ���ض�ȡ������
*******************************************************************************/
UINT8 xReadCH395Data( void ) 
{
	return(Spi395Exchange(0xFF));  	
}

#define	xEndCH395Cmd()	CH395_SPI_CS_HIGH()  			 /* SPIƬѡ��Ч,����CH395����,������SPI�ӿڷ�ʽ */

/*******************************************************************************
* Function Name  : Query395Interrupt
* Description    : ��ѯCH395�ж�(INT#�͵�ƽ)
* Input          : None
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 Query395Interrupt( void )
{
	return(CH395_INT_PIN_INPUT() ? FALSE : TRUE);  
}

/*******************************************************************************
* Function Name  : CH395_RST
* Description    : ��λ CH395
* Input          : None
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
void CH395_RST( void )
{
	CH395_RST_PIN_LOW();
	mDelaymS(10);
	CH395_RST_PIN_HIGH();
}


