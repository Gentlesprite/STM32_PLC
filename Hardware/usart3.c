#include "usart3.h"
#include "LED.h"
u16 USART3_RX_STA;   //接收数据状态
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
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
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

//串口接收缓存区 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 	//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
u16 USART3_RX_STA=0; //接收到的数据状态
void USART3_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收到数据
	{	 
	   res =USART_ReceiveData(USART3);		
	if (res == 'A')LED2_ON();
		
	if (res == 'B')LED2_OFF();
		
	//如果用户发送设定阈值的数据
		/*
		temp:代表温度阈值
		soil:代表土壤湿度阈值
		co2:代表二氧化碳阈值
		命令格式
		temp 30 代表温度超过30度操作继电器:比如可以连一个风扇进行降温？
		soil 10 此处的土壤湿润应该是代表小于10的意思，代表土壤湿润如果小于10则继电器驱动水泵自动浇水
		co2 400 代表二氧化碳超过400 进行操作
		然后解析命令
		
		这是主函数在开头设定的默认报警阈值
		u8 temp_threshold = 40;
		int soil_threshold = 13;
		uint16_t co2_threshold = 400;
		
		把解析的命令赋值给对应的变量
		
		*/
		
		
	  if((USART3_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
	 { 
		if(USART3_RX_STA<USART3_MAX_RECV_LEN)		//还可以接收数据
		{
			USART3_RX_BUF[USART3_RX_STA++]=res;		//记录接收到的值	 
		}else 
		{
			USART3_RX_STA|=1<<15;					//强制标记接收完成
		} 
	}  	
  }										 
}  

void USART3_Print(char *str) {
    while (*str) {
        USART_SendData(USART3, *str++);
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    }
}

//cmd:发送的命令字符串;ack:期待的应答结果,如果为空,则表示不需要等待应答;waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果);1,发送失败

/*使用microLib的方法*/
