
#include "lpc17xx_timer.h"
uint32_t prescaleUS = 1;   // prescaler counts 1us 
uint32_t matchUS = 63;     // match value to 63us -> 15,87kHz
void ConfTIM0(void){
    TIM_TIMERCFG_Type timCfg;
    timCfg.prescaleOpt = TIM_US;
    timCfg.prescaleValue = prescaleUS;
    TIM_InitTimer(LPC_TIM0, &timCfg);

    TIM_MATCHCFG_T matchCfg;
    matchCfg.Channel = TIM_MATCH_1; // MAT0.1 (used for ADC trigger)
    matchCfg.intEn = DISABLE;
    matchCfg.stopEn = DISABLE;
    matchCfg.resetEn = ENABLE;
    matchCfg.extOpt = TIM_NOTHING;
    matchCfg.matchValue = 63;   
    TIM_ConfigMatch(LPC_TIM0, &matchCfg);

    TIM_Enable(LPC_TIM0);
}