#include "dac.h"

/* =========================================================
 * dac.c
 * Implementacion de la configuracion del DAC.
 * ========================================================= */

/* ---------------------------------------------------------
 * ConfDAC
 * --------------------------------------------------------- */
void ConfDAC(void)
{
    DAC_CONVERTER_CFG_T dacCfg;
    dacCfg.doubleBuffer = DISABLE;   /* Un solo registro de salida */
    dacCfg.dmaRequest   = ENABLE;    /* El DAC dispara solicitudes al DMA */
    dacCfg.dmaCounter   = ENABLE;    /* Temporizador interno del DAC activo */

    DAC_Init();
    DAC_ConfigDAConverterControl(&dacCfg);

    /* Timeout = DAC_DMA_TIMEOUT ticks de PCLK_DAC
     * Determina la frecuencia de muestreo de salida. */
    DAC_SetDMATimeOut(DAC_DMA_TIMEOUT);
}
