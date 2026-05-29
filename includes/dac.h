#ifndef DAC_H
#define DAC_H

#include "../config.h"
#include "lpc17xx_dac.h"

/* =========================================================
 * dac.h
 * Configuracion del DAC del LPC1769 para reproduccion
 * de audio via DMA.
 *
 * El DAC opera con su temporizador interno habilitado:
 * cada TIMEOUT genera una solicitud DMA para escribir
 * la siguiente muestra de BUFFER_SALIDA en DACR.
 *
 * Formula de frecuencia de muestreo:
 *   Fs = PCLK_DAC / (TIMEOUT + 1)
 *
 * Ejemplo con PCLK = 25 MHz y TIMEOUT = 25:
 *   Fs = 25 000 000 / 25 = 1 000 000 Hz  (ajustar segun diseño)
 *
 * NOTA: El valor de DAC_DMA_TIMEOUT debe coincidir con la
 * frecuencia de muestreo a la que se diseñaron los filtros IIR.
 * ========================================================= */

/* ---------------------------------------------------------
 * Valor del contador de tiempo del DAC.
 * Determina la frecuencia de muestreo de salida.
 * Ajustar segun PCLK_DAC y Fs deseada.
 * --------------------------------------------------------- */
#define DAC_DMA_TIMEOUT   25U

/* ---------------------------------------------------------
 * ConfDAC
 *
 * Inicializa el DAC con:
 *   - Double buffer: deshabilitado
 *   - Solicitud DMA: habilitada
 *   - Contador DMA:  habilitado
 *   - Timeout:       DAC_DMA_TIMEOUT ticks
 *
 * Debe llamarse ANTES de ConfDMA_BUFFERSALIDA_DAC para
 * que el temporizador del DAC ya este corriendo cuando
 * el DMA empiece a transferir.
 * --------------------------------------------------------- */
void ConfDAC(void);

#endif /* DAC_H */
