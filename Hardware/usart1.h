#ifndef __USART1_H
#define __USART1_H

#include "stm32f10x.h"  // 假设使用STM32F10x系列，根据实际MCU调整
#include <stdio.h>      // 用于fputc的FILE类型

// 宏定义
#define USART1_REC_LEN  200     // 定义串口1最大接收字节数

// 全局变量声明
extern uint8_t USART1_RX_BUF[USART1_REC_LEN];  // 接收缓冲区
extern uint16_t USART1_RX_STA;                 // 接收状态标记
extern uint16_t USART1_RX_LEN;                 // 接收数据长度

// 函数声明
void usart1_init(uint32_t bound);
void usart1_sendstr(uint8_t *pData, uint16_t Size);
void Serial_SendByte(uint8_t Byte);
uint8_t Serial_GetRxData(void);

#endif

