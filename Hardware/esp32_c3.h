#ifndef __ESP_32C3_H
#define __ESP_32C3_H

#include "stm32f10x.h"
#include "delay.h"
#include "usart1.h"
#include "usart3.h"
#include "stdio.h"

extern char restart[];
extern char cwmode[];
extern char cwlap[];
extern char cwjap[];
extern char cifsr[];
extern char cipmux[];
extern char cipstart[];
extern char cipsend[];
extern char cipserver[];
extern char cwlif[];
extern char cipstatus[];
extern char cipsto[];
extern char cipmode[];
extern char test[];

u8 esp_32c3_send_cmd(u8 *cmd, u8 *ack, u16 waittime);
u8* esp_32c3_check_cmd(u8 *str);
u8* esp_32c3_send_data(u8 *string, u16 waittime);

void esp_32c3_init(void);
void esp_32c3_start_connect(void);
void esp_32c3_quit_init(void);
void esp_32c3_start_init(void);
void esp_32c3_ap(void);

u8 esp_32c3_quit_connect(void);

#endif 



