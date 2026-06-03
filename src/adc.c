#include "adc.h"

void ConfADC(void){
    ADC_Init(ADC_FS_HZ);                // rate in Hz (set to 64000)
    ADC_PowerUp();
    ADC_PinConfig(ADC_AUDIO_CHANNEL);   // P0.23 -> AD0.0 
    ADC_BurstEnable();                  // Burst mode: continous conversions
    ADC_ChannelEnable(ADC_AUDIO_CHANNEL);
}