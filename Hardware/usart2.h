#ifndef __USART2_H
#define __USART2_H

#include "stm32f10x.h"                  // Device header
#include "usart2.h"

extern uint8_t Usart2_RxPacket[9];				//定义接收数据包数组
extern uint8_t Usart2_RxFlag;

void usart2_init(u32 bound);
uint8_t Usart2_GetRxFlag(void);

void CO2GetData(uint16_t *data);

#endif


