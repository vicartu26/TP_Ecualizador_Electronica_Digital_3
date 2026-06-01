#include "lpc17xx_adc.h"

void ConfADC(void){
    ADC_Init(1000000);
    ADC_PowerUp();
    ADC_PinConfig(ADC_CHANNEL_0);       // P0.23 -> AD0.0 
    ADC_BurstDisable();                 // no burst, we use a trigger
    ADC_StartCmd(ADC_START_ON_MAT01);   // MAT0.1 triggers conversion 15,87kHz
    ADC_EdgeStartConfig(ADC_START_ON_RISING);
    ADC_ChannelEnable(ADC_CHANNEL_0);
}