#include "stm32f10x.h"
#include "Delay.h"
#include "led.h"
#include "Serial.h"
#include "DHT11.h"
#include "usart1.h"
#include "usart3.h"
#include "esp32_c3.h"
#include "LED.h"
#include "adc.h"
#include "OLED_I2C.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char sendBuffer[50];
void Main_DoSomething(void);
typedef unsigned char u8;
char rxdata[100]="n";
u8 temperature;
u8 humidity;
char display[16];
unsigned char setn=0;//记录设置键按下的次数
unsigned char temperature=0;
unsigned char humidity=0;
unsigned char setTempValue=35;        //温度设置值
unsigned int  light=0;
unsigned int  setSoilMoisture=10;
unsigned int  soilMoisture;           //土壤湿度
unsigned char setLightValue=20;       //光照设置值

bool shuaxin  = 0;
bool shanshuo = 0;
bool sendFlag = 1;
void InitDisplay(void)   //初始化显示
{
	  unsigned char i=0;
	  for(i=0;i<4;i++)OLED_ShowCN(i*16,0,i+0,0);//显示中文：环境温度
	  for(i=0;i<4;i++)OLED_ShowCN(i*16,2,i+4,0);//显示中文：环境湿度
	  for(i=0;i<4;i++)OLED_ShowCN(i*16,4,i+8,0);//显示中文：土壤湿度
	  OLED_ShowChar(64,0,':',2,0);
	  OLED_ShowChar(64,2,':',2,0);
    OLED_ShowChar(64,4,':',2,0);
}
void displayDHT11TempAndHumi(void)  //显示环境温湿度
{
		DHT11_Read_Data(&temperature,&humidity);
	  if(temperature>=setTempValue && shanshuo)
		{
			  OLED_ShowChar(80,0,' ',2,0);
				OLED_ShowChar(88,0,' ',2,0);
		}
		else
		{
				OLED_ShowChar(80,0,temperature/10+'0',2,0);
				OLED_ShowChar(88,0,temperature%10+'0',2,0);
		}
		OLED_ShowCentigrade(96, 0);
		OLED_ShowChar(80,2,humidity/10+'0',2,0);
		OLED_ShowChar(88,2,humidity%10+'0',2,0);
		OLED_ShowChar(96,2,'%',2,0);
}
void displaySoilMoisture(void)//显示土壤湿度
{

	   soilMoisture = 100-(Get_Adc_Average(ADC_Channel_8,10)*99/4096);
	   if(soilMoisture>99)soilMoisture=99;
		 if(soilMoisture<=setSoilMoisture && shanshuo)
		{
			  OLED_ShowChar(80,4,' ',2,0);
				OLED_ShowChar(88,4,' ',2,0);
		}
		else
		{
			 OLED_ShowChar(80,4,soilMoisture/10+'0',2,0);
			 OLED_ShowChar(88,4,soilMoisture%10+'0',2,0);
		}
  	 OLED_ShowChar(96,4,'%',2,0);
}
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	DelayInit();
	LED_Init();
		  I2C_Configuration();     //IIC初始化
	  OLED_Init();             //OLED液晶初始化
	  Adc_Init();
	  OLED_CLS();              //清屏
	//OLED_ShowStr(0, 2, "   loading...   ", 2,0);//显示加载中
	usart1_init(115200);
	usart3_init(115200);
	esp_32c3_init();
	esp_32c3_quit_init();
	esp_32c3_start_init();
	DHT11_Init();
	Main_DoSomething();
			OLED_CLS();              //清屏
	  InitDisplay();
	while (1)
	{
		DHT11_Read_Data(&temperature,&humidity);
		Main_DoSomething();
										 displayDHT11TempAndHumi();
							   displaySoilMoisture();
		}
}
void 	Main_DoSomething(void)
{
		esp_32c3_send_data("卢治宇 2024304066",50);// 将温度和湿度格式化为字符串
    sprintf(sendBuffer, "温度:%d℃ 湿度:%d%%RH", temperature, humidity);
    esp_32c3_send_data(sendBuffer, 50);// 发送温湿度数据
}


