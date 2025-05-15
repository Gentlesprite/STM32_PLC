#include "esp32_c3.h"

char restart[] = "AT+RST";
char cwmode[] = "AT+CWMODE=1";
char cwlap[] = "AT+CWLAP";
char cwjap[] = "AT+CWJAP";
char cifsr[] = "AT+CIFSR";
char cipmux[] = "AT+CIPMUX=0";
char cipstart[] = "AT+CIPSTART";
char cipsend[] = "AT+CIPSEND";
char cipserver[] = "AT+CIPSERVER";
char cwlif[] = "AT+CWLIF";
char cipstatus[] = "AT+CIPSTATUS";
char cipsto[] = "AT+CIPSTO";
char cipmode[] = "AT+CIPMODE=1";
char test[] = "AT";

void esp_32c3_quit_init(void)
{
    // 例如：退出透传模式的初始化
    esp_32c3_quit_connect();
}

void esp_32c3_start_init(void)
{
    // 例如：启动连接的初始化
    esp_32c3_start_connect();
	//esp_32c3_ap();
}

u8 esp_32c3_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	wifi_usart3_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			Delay_ms(10);
			if(esp_32c3_check_cmd(ack)!=NULL)
			{
				printf("ack:%s\r\n",(u8*)ack);
				break;//得到有效数据 
			}
		}
		USART3_RX_STA=0;
		if(waittime==0)
		{
			res=1;
		}
	}
	return res;
} 

//str:期待的应答结果
//返回值:0,没有得到期待的应答结果;其他,期待应答结果的位置(str的位置)
u8* esp_32c3_check_cmd(u8 *str)
{
	char *strx=0;
	strx=strstr((const char*)USART3_RX_BUF,(const char*)str);//判断接收的应答信息是否有效，有效，则直接返回应答信息，无效，则返回NULL
	return (u8*)strx;
}

//string:发送的字符串;waittime:等待时间(单位:10ms)
//返回值:发送数据后，服务器的返回验证码
u8* esp_32c3_send_data(u8 *string,u16 waittime)
{
	char temp[5];
	char *ack=temp;
	USART3_RX_STA=0;
	wifi_usart3_printf((char *)string);
	if(waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			Delay_ms(10);
		}
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		ack=(char*)USART3_RX_BUF;
		printf("ack:%s\r\n",(u8*)ack);
		USART3_RX_STA=0;
	}
	return (u8*)ack;
}

void esp_32c3_init(void)
{
	//设置工作模式 1：station模式   2：AP模式  3：兼容 AP+station模式
	esp_32c3_send_cmd("AT+CWMODE=1","OK",50);
	//让Wifi模块重启的命令
	esp_32c3_send_cmd("AT+RST","ready",20);
	Delay_ms(1000);         //延时3S等待重启成功
	Delay_ms(1000);
	Delay_ms(1000);
	Delay_ms(1000);
}

void esp_32c3_ap(void)
{
    // 设置WiFi模式为AP模式 (1=STA, 2=AP, 3=STA+AP)
    //esp_32c3_send_cmd("AT+CWMODE=2", "OK", 200);
    
    // 配置AP参数: SSID,密码,通道号,加密方式
    // SSID: ESP32_AP
    // 密码: 12345678
    // 通道: 5
    // 加密方式: WPA2_PSK (3)
    esp_32c3_send_cmd("AT+CWSAP=\"LZY\",\"12345678\",5,3", "OK", 500);
    
    // 启用多连接
    esp_32c3_send_cmd("AT+CIPMUX=1", "OK", 200);
    
    // 启动服务器，端口8080
    while(esp_32c3_send_cmd("AT+CIPSERVER=1,8080", "OK", 200));
	
		//是否开启透传模式  0：表示关闭 1：表示开启透传
	esp_32c3_send_cmd("AT+CIPMODE=1","OK",200);
	
	//透传模式下 开始发送数据的指令 这个指令之后就可以直接发数据了
	esp_32c3_send_cmd("AT+CIPSEND","OK",50);

}

void esp_32c3_start_connect(void){
	
	//让模块连接上路由（用户自己定义）
//其中TP-LINK_123456为WiFi名，123456为密码，将其替换为你的WiFi
//while();  在stm32中为等待含义
	while(esp_32c3_send_cmd("AT+CWJAP=\"Reno\",\"luzhiyu666\"", "WIFI GOT IP",600));
	
	//=0：单路连接模式     =1：多路连接模式
	esp_32c3_send_cmd("AT+CIPMUX=0","OK",20);
	
	//建立TCP连接  192.168.0.101和8080为TCP Server的ip(手机IP)和端口
while(esp_32c3_send_cmd("AT+CIPSTART=\"TCP\",\"192.168.0.123\",8086","CONNECT",200));
	
	//是否开启透传模式  0：表示关闭 1：表示开启透传
	esp_32c3_send_cmd("AT+CIPMODE=1","OK",200);
	
	//透传模式下 开始发送数据的指令 这个指令之后就可以直接发数据了
	esp_32c3_send_cmd("AT+CIPSEND","OK",50);
}


//esp_32c3退出透传模式   返回值:0,退出成功;1,退出失败
//通过向wifi模块连续发送3个+（每个+号之间 超过10ms,这样认为是连续三次发送+）
u8 esp_32c3_quit_connect(void)
{
	u8 result=1;
	wifi_usart3_printf("+++");//退出透传模式
	Delay_ms(1000);	//等待500ms太少 要1000ms才可以退出
	do
	{
Delay_ms(2000);	//等待2000ms
		result=esp_32c3_send_cmd((u8 *)"AT+RST",(u8 *)"OK",20);//判断退出透传是否成功.0-成功，1-失败	
		if(result)
			printf("quit_trans failed!\r\n");
		else
		 {
printf("quit_trans success!\r\n");
break;
}
	}while(result);
	return result;
}
