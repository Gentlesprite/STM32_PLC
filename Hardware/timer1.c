#include "stm32f10x_tim.h"
#include "timer1.h"
#include "led.h"
extern u8 temperature;
extern u8 humidity_t;
extern int soilMoisture;
extern u8 humidity;
extern uint16_t co2;
extern u8 temp_threshold;
extern int soil_threshold;
extern uint16_t co2_threshold;
void tim1_init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 使能TIM1时钟（注意：TIM1在APB2总线上）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    
    // 定时器基础配置（500ms中断）
    TIM_TimeBaseStructure.TIM_Period = 4999;     // 自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = 7199;  // 预分频值（72MHz / 7200 = 10kHz）
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; // 高级定时器特有参数
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    
    // 使能更新中断（注意：TIM1需额外使能主输出）
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE); // 高级定时器特有
    
    // NVIC配置
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn; // TIM1中断通道不同！
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 启动定时器
    TIM_Cmd(TIM1, ENABLE);
}


void TIM1_UP_IRQHandler(void) {
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
        
        // 环境检查
        char sendBuffer[50];
			if (humidity > humidity_t)LED2_ON();
			if (humidity < humidity_t)LED2_OFF();
        if (temperature > temp_threshold) {
            //sprintf(sendBuffer, "温度超过阈值%d", temp_threshold);
            //esp_32c3_send_data((u8 *)sendBuffer, 50);
						//LED2_ON();
	
            // 操作继电器
        }
				if (temperature < temp_threshold)
					{
					//LED2_OFF();
					}
        if (soilMoisture > soil_threshold) {
            //sprintf(sendBuffer, "土壤湿度超过阈值%d", soil_threshold);
            //esp_32c3_send_data((u8 *)sendBuffer, 50);
            // 操作继电器
        }
        if (co2 > co2_threshold) {
            //sprintf(sendBuffer, "二氧化碳超过阈值%d", co2_threshold);
            //esp_32c3_send_data((u8 *)sendBuffer, 50);
            //LED1_ON();
            // 让蜂鸣器报警
        }
        if (co2 < co2_threshold) {
            //LED1_OFF();
        }
    }
}
