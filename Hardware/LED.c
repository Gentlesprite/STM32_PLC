#include "led.h"

uint16_t LED_Hint_Counter =0;//LED提示灯0-600s
void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE, GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);
//	GPIO_ResetBits(GPIOF, GPIO_Pin_2);
}




void LED2_ON(void)
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_5);
}

void LED2_OFF(void)
{
	GPIO_SetBits(GPIOE, GPIO_Pin_5);
}

void LED2_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_5) == 0)
	{
		GPIO_SetBits(GPIOE, GPIO_Pin_5);
	}
	else
	{
		GPIO_ResetBits(GPIOE, GPIO_Pin_5);
	}
}

void RELAY_OFF(void)//继电器水泵 PE 2
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_2);
}

void RELAY_ON(void)
{
	
	GPIO_SetBits(GPIOE, GPIO_Pin_2);
}

void BUZZER_OFF(void) //蜂鸣器 PE 3
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_3);
}

void BUZZER_ON(void)
{
	GPIO_SetBits(GPIOE, GPIO_Pin_3);
}

void FAN_ON(void)  //风扇 PE 4 3.3V低电平触发
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_4);
}

void FAN_OFF(void)
{
	GPIO_SetBits(GPIOE, GPIO_Pin_4);
}

void LED_Hint(uint16_t timer)//LED提示灯
{
	LED2_OFF();
	LED2_ON();
	LED_Hint_Counter =timer;
	
}
