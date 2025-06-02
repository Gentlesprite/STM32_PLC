#include "usart3.h"
#include "LED.h"
#include <string.h>
#include <stdlib.h>
u16 USART3_RX_STA;   //接收数据状态、

u8 USART3_RX_CNT = 0;                   // 接收计数器
u8 USART3_RX_FLAG = 0;                  // 接收完成标志

void usart3_init(u32 bound) //bound:波特率	  
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE); //使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
 	USART_DeInit(USART3);  //复位串口3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ; //GPIOB11(USART3_TX)初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; //GPIOB11(USART3_RX)初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化PB10
	USART_InitStructure.USART_BaudRate = bound;//波特率 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//允许收发模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口3
	USART_Cmd(USART3, ENABLE);               //使能串口 
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口3接收中断   
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//开启串口3接收中断   
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//初始化串口3中断通道(根据指定的串口3中断通道（USART3_IRQn）参数初始化NVIC寄存器)
}

#define USART3_MAX_SEND_LEN		400		//最大发送缓存字节数
u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; //串口发送缓存区.最大USART3_MAX_SEND_LEN字节
void wifi_usart3_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);//此次发送数据的长度
	for(j=0;j<i;j++)//循环发送数据
	{
	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);  //等待上次传输完成 
	 USART_SendData(USART3,(uint8_t)USART3_TX_BUF[j]); //发送数据到串口3 
	}
}
void USART3_Print(char *str) {
    while (*str) {
        USART_SendData(USART3, *str++);
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    }
}

//串口接收缓存区 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 	//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
u16 USART3_RX_STA=0; //接收到的数据状态
void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {   //接收寄存器非空
					u8 res = USART_ReceiveData(USART3); //读取接收到的字节(USART_ReceiveData)
					// 简单处理LED控制
					if(res == 'A') LED2_ON();
					if(res == 'B') LED2_OFF();
					
					// 将数据存入缓冲区USART3_RX_BUF
					if(USART3_RX_STA < USART3_MAX_RECV_LEN) {
							USART3_RX_BUF[USART3_RX_STA++] = res;
        }
				USART_ClearITPendingBit(USART3, USART_IT_RXNE); //清除中断标志
    }
    
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) { //IDLE中断处理（检测到总线空闲）
			USART_ReceiveData(USART3); //读取数据寄存器以清除标志（虽然数据可能无效）
			USART3_RX_FLAG = 1; // 设置接收完成标志USART3_RX_FLAG
			USART_ClearITPendingBit(USART3, USART_IT_IDLE);//清除中断标志
    }
}  


//cmd:发送的命令字符串;ack:期待的应答结果,如果为空,则表示不需要等待应答;waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果);1,发送失败

/*使用microLib的方法*/
