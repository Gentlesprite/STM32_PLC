// debug_utils.c
#include "debug_utils.h"
#include <stdarg.h>

// ESP32专用发送函数（强制USART3）
void SendToESP32(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    wifi_usart3_printf(fmt, ap);  // 直接调用USART3发送
    va_end(ap);
}

// 调试输出函数（条件编译）
void DebugPrint(const char *fmt, ...) {
#if DEBUG_TO_USART1
    va_list ap;
    va_start(ap, fmt);
    printf(fmt, ap);  // 使用USART1（依赖fputc重定向）
    va_end(ap);
#endif
}