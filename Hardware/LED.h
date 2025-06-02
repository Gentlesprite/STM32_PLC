#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"                  // Device header


void LED_Init(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED2_Turn(void);
void RELAY_ON(void);
void RELAY_OFF(void);
void BUZZER_OFF(void);
void BUZZER_ON(void);
void FAN_ON(void);
void FAN_OFF(void);
void LED_Hint(uint16_t timer);//LEDÃ· æµ∆
	
#endif
