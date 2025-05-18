#include "stm32f10x.h"
#include "Delay.h"
#include "led.h"
#include "Serial.h"
#include "DHT11.h"
#include "usart1.h"
#include "usart2.h"
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
uint16_t co2;
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
		for(i=0;i<4;i++)OLED_ShowCN(i*16,6,i+12,0);//显示中文：二氧化碳
	  OLED_ShowChar(64,0,':',2,0);
	  OLED_ShowChar(64,2,':',2,0);
    OLED_ShowChar(64,4,':',2,0);
		OLED_ShowChar(64,6,':',2,0);
}
void displayDHT11TempAndHumi(void)  //显示环境温湿度
{
		DHT11_Read_Data(&temperature,&humidity);
	  if(temperature>=setTempValue && shanshuo)
		{
			  OLED_ShowChar(78,0,' ',2,0);
				OLED_ShowChar(86,0,' ',2,0);
		}
		else
		{
				OLED_ShowChar(78,0,temperature/10+'0',2,0);
				OLED_ShowChar(86,0,temperature%10+'0',2,0);
		}
		OLED_ShowCentigrade(94, 0);
		OLED_ShowChar(78,2,humidity/10+'0',2,0);
		OLED_ShowChar(86,2,humidity%10+'0',2,0);
		OLED_ShowChar(94,2,'%',2,0);
}
void displaySoilMoisture(void)//显示土壤湿度
{

	   soilMoisture = 100-(Get_Adc_Average(ADC_Channel_8,10)*99/4096);
	   if(soilMoisture>99)soilMoisture=99;
		 if(soilMoisture<=setSoilMoisture && shanshuo)
		{
			  OLED_ShowChar(78,4,' ',2,0);
				OLED_ShowChar(86,4,' ',2,0);
		}
		else
		{
			 OLED_ShowChar(78,4,soilMoisture/10+'0',2,0);
			 OLED_ShowChar(86,4,soilMoisture%10+'0',2,0);
		}
  	 OLED_ShowChar(94,4,'%',2,0);
}

void displayCO2(void)    //显示二氧化碳浓度
{
    if(co2 > 999) co2 = 999;  //限制最大显示值
    OLED_ShowChar(78,6,(co2%1000)/100+'0',2,0); //百位
    OLED_ShowChar(86,6,(co2%100)/10+'0',2,0);  //十位
    OLED_ShowChar(94,6,co2%10+'0',2,0);       //个位
    OLED_ShowStr(102,6,"ppm",2,0);          //单位
}
int main(void)
{
	unsigned char i=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	DelayInit();
	LED_Init();
	I2C_Configuration();     //IIC初始化
	OLED_Init();             //OLED液晶初始化
	Adc_Init();
	OLED_CLS();              //清屏
	OLED_ShowStr(0, 3, "          ...", 2,0);//显示加载中
	for(i=0;i<5;i++)OLED_ShowCN(i*16,3,i+16,0);//显示中文：网络连接中
	usart1_init(115200);
	usart2_init(9600);
	usart3_init(115200);
	esp_32c3_init();
	esp_32c3_send_cmd("AT+CWQAP","0K",200);
	esp_32c3_quit_init();
	esp_32c3_start_init();
	DHT11_Init();
	OLED_CLS();              //清屏
	InitDisplay();
	while (1)
	{
		DHT11_Read_Data(&temperature,&humidity);
		CO2GetData(&co2);
		Main_DoSomething();
		displayDHT11TempAndHumi();
		displaySoilMoisture();
		displayCO2();
		}
}
void 	Main_DoSomething(void)
{
	sprintf(sendBuffer, "温度:%d℃ 湿度:%d%%RH 土壤湿度:%d%%RH 二氧化碳浓度:%dppm", temperature, humidity, soilMoisture,co2);
   esp_32c3_send_data(sendBuffer, 50);// 发送温湿度数据
}


