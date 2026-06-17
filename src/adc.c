#include "adc.h"

uint16_t interrupcionADC = 0;
void ConfADC(void){
    ADC_Init(ADC_FS_HZ);                // rate in Hz (set to 64000)
    ADC_PowerUp();
    ADC_PinConfig(ADC_AUDIO_CHANNEL);   // P0.23 -> AD0.0 
    ADC_BurstEnable();                  // Burst mode: continous conversions
    ADC_ChannelEnable(ADC_AUDIO_CHANNEL);

    ADC_IntEnable(ADC_INT_CH0);
    NVIC_SetPriority(ADC_IRQn, 0U);     // Mayor prioridad (0) para evitar perder muestras
    NVIC_EnableIRQ(ADC_IRQn);

}

void ADC_IRQHandler(void)
{
    /* Leer el valor convertido (limpia el flag de interrupcion) */
    uint16_t valor = ADC_ChannelGetData(ADC_AUDIO_CHANNEL);
    interrupcionADC++;
}