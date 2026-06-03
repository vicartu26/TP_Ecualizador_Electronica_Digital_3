#include "lpc17xx_adc.h"

#define ADC_RATE 16000U  // Frecuencia de muestreo en Hz (max 4.5MHz)

void ConfADC(void){
    ADC_Init(ADC_RATE);                 // rate in Hz (set to 16000)
    ADC_PowerUp();
    ADC_PinConfig(ADC_CHANNEL_0);       // P0.23 -> AD0.0 
    ADC_BurstEnable();                  // Burst mode: continous conversions
    ADC_ChannelEnable(ADC_CHANNEL_0);
}