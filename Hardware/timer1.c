#include "stm32f10x_tim.h"
#include "timer1.h"
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
