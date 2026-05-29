#ifndef DMA_H
#define DMA_H

#include "config.h"
#include "lpc17xx_gpdma.h"

/* =========================================================
 * dma.h
 * Configuracion del controlador GPDMA para:
 *   Canal 1 -> ADC a BUFFERA/BUFFERB (ping-pong, P2M)
 *   Canal 0 -> BUFFER_SALIDA a DAC   (circular,  M2P)
 * ========================================================= */

/* ---------------------------------------------------------
 * Bits del campo CONTROL de un LLI (tabla 24-4 del manual)
 * Usados para armar el descriptor de transferencia.
 * --------------------------------------------------------- */

/* Bits [11:0]  Transfer size (cantidad de items) */
#define DMA_LLI_TRANSFER_SIZE(n)   ((n) & 0xFFFU)

/* Bit 18: Source transfer width = 1 (SWidth) */
#define DMA_LLI_SRC_WIDTH      (1U << 18)

/* Bit 21: Destination transfer width = 1 (DWidth) */
#define DMA_LLI_DST_WIDTH      (1U << 21)

/* Bit 26: Destination increment enable */
#define DMA_LLI_DST_INC            (1U << 26)

/* Bit 27: Source increment enable */
#define DMA_LLI_SRC_INC            (1U << 27)

/* ---------------------------------------------------------
 * Canal DMA asignado a cada funcion
 * --------------------------------------------------------- */
#define DMA_CH_DAC_SALIDA   GPDMA_CH_0   /* BUFFER_SALIDA -> DAC (sin interrupcion) */
#define DMA_CH_ADC_CAPTURE  GPDMA_CH_1   /* ADC -> BUFFERA/BUFFERB (con interrupcion) */

/* ---------------------------------------------------------
 * Prioridad NVIC del canal DMA de captura ADC.
 * Debe ser la mas alta del sistema (0) para no perder
 * muestras de audio.
 * --------------------------------------------------------- */
#define DMA_NVIC_PRIORITY   0U

/* ---------------------------------------------------------
 * ConfDMA_ADC_BUFFERS
 *
 * Configura el Canal 1 del GPDMA en modo P2M con lista
 * enlazada circular (LLI ping-pong):
 *   LLI1: ADC -> BUFFERA  (al terminar, salta a LLI2)
 *   LLI2: ADC -> BUFFERB  (al terminar, salta a LLI1)
 *
 * Genera interrupcion al completar cada bloque (TC).
 * Habilita DMA_IRQn con prioridad DMA_NVIC_PRIORITY.
 * --------------------------------------------------------- */
void ConfDMA_ADC_BUFFERS(void);

/* ---------------------------------------------------------
 * ConfDMA_BUFFERSALIDA_DAC
 *
 * Configura el Canal 0 del GPDMA en modo M2P con LLI
 * circular (se realimenta a si mismo):
 *   BUFFER_SALIDA -> DAC  (cadena infinita, sin interrupcion)
 *
 * El temporizador interno del DAC controla el rate de
 * transferencia (configurado en ConfDAC).
 * --------------------------------------------------------- */
void ConfDMA_BUFFERSALIDA_DAC(void);

/* ---------------------------------------------------------
 * DMA_IRQHandler
 *
 * Manejador de interrupcion del GPDMA.
 * Alterna entre FLAGBUFFERA y FLAGBUFFERB para indicarle
 * al loop principal cual bloque esta listo para procesar.
 * --------------------------------------------------------- */
void DMA_IRQHandler(void);

#endif /* DMA_H */
