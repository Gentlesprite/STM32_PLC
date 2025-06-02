#include "usart1.h"
#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "led.h"
uint8_t Serial_RxData;//jieshou shuju
uint8_t Serial_RxFlag;//biaozhiwei
void usart1_init(u32 bound)//用户指定波特率，9600bps、115200bps等
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口1接受中断
  USART_Cmd(USART1, ENABLE); //使能串口1 
}


void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void  usart1_sendstr(uint8_t *pData, uint16_t Size)
{
	u16 t;
	for(t=0;t<Size;t++)
	{
		USART1->SR;//TC 和 TXE 标志位在复位的时候被置1，会导致发送第一个字节丢失。
//添加这句读取USARTx->SR，能避免这个错误  
		USART_SendData(USART1, pData[t]);//向串口x发送数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待字符发送结束
	}	
}

#define USART1_REC_LEN  		200  	//定义最大接收字节数 200
u8 USART1_RX_BUF[USART1_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.
u16 USART1_RX_STA=0;       //接收状态标记	  
u16 USART1_RX_LEN;       //USART1接收缓冲索引值
uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;
}


void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		Serial_RxData = USART_ReceiveData(USART1);
		Serial_SendByte(Serial_RxData);
		Serial_RxFlag = 1;
		
	}
	if (Serial_RxData == 'A')
		LED2_ON();

	if (Serial_RxData == 'B')
		LED2_OFF();

	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}


int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
