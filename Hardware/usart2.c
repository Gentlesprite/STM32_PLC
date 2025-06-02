#include "usart2.h"	


uint8_t Usart2_RxPacket[9];				//定义接收数据包数组
uint8_t Usart2_RxFlag;					//定义接收数据包标志位

void usart2_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART2，GPIOA时钟
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能GPIOA时钟
	
	//USART2_TX   GPIOA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
   
    //USART2_RX	  GPIOA。3初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOB.3

  //Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级4
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART2, ENABLE);                    //使能串口2 

}

void USART2_IRQHandler(void)                	//串口2中断服务程序
{
		uint8_t Res;
		static uint8_t RxState = 0;		//当前状态机状态
		static uint8_t pRxPacket = 0;	//当前接收数据位置

		if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
			Res =USART_ReceiveData(USART2);	//读取接收到的数据	
			switch (RxState)
			{
				case 0: 
					if (Res == 0x2C)
					{
						Usart2_RxPacket[pRxPacket] = Res;	//将数据存入数组
						pRxPacket++;
						RxState = 1;

					}
					else
					{
						pRxPacket = 0;
						RxState = 0;
					}
					break;
				case 1: 
						Usart2_RxPacket[pRxPacket] = Res;	//将数据存入数组
						pRxPacket++;
						if(pRxPacket >= 9)
						{
							pRxPacket = 0;
							RxState = 2;
						}
					break;
				case 2:
					if (Usart2_RxPacket[8] == (uint8_t)(Usart2_RxPacket[0] + Usart2_RxPacket[1]	//验证接收到的数据是否正确
						+ Usart2_RxPacket[2] + Usart2_RxPacket[3] + Usart2_RxPacket[4]+ Usart2_RxPacket[5]+ Usart2_RxPacket[6]+ Usart2_RxPacket[7]))
					{
						
						RxState = 0;
						pRxPacket = 0;
						Usart2_RxFlag = 1;		//接收数据包标志位置1，成功接收一个数据包
					}
					else
					{
						pRxPacket = 0;
						RxState = 0;
					}
					break;
			}
			
			USART_ClearITPendingBit(USART2, USART_IT_RXNE);		//清除标志位
		} 
} 


void CO2GetData(uint16_t *data)
{
	if (Usart2_RxFlag == 1)
	{
		Usart2_RxFlag = 0;
		*data = Usart2_RxPacket[6] * 256 + Usart2_RxPacket[7];
	}
}

