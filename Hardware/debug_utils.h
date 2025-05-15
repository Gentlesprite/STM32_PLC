// debug_utils.h
#ifndef __DEBUG_UTILS_H
#define __DEBUG_UTILS_H

#include "usart1.h"
#include "usart3.h"

// 调试输出开关（1=启用USART1调试，0=完全禁用）
#define DEBUG_TO_USART1 0  

// 强制使用USART3发送ESP32数据
void SendToESP32(const char *fmt, ...);

// 条件编译的调试输出（仅当DEBUG_TO_USART1=1时生效）
void DebugPrint(const char *fmt, ...);

#endif