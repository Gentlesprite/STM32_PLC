#ifndef _WIFI_USART_H
#define _WIFI_USART_H

#include "stm32f10x.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

#define USART3_MAX_RECV_LEN   400
#define USART3_MAX_SEND_LEN   400
#define USART3_RX_EN   1

extern u8 USART3_RX_BUF[USART3_MAX_RECV_LEN];
extern u8 USART3_TX_BUF[USART3_MAX_SEND_LEN];
extern u16 USART3_RX_STA;

void usart3_init(u32 bound);
void usart3_printf(char* fmt, ...);

#endif


