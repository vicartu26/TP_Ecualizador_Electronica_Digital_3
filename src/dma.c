#include "dma.h"

/* =========================================================
 * dma.c
 * Configuracion GPDMA para captura ADC (ping-pong)
 * y reproduccion DAC (circular).
 * ========================================================= */

/* ---------------------------------------------------------
 * LLIs estaticos: deben persistir durante toda la ejecucion
 * porque el DMA los referencia en memoria en cada ciclo.
 * --------------------------------------------------------- */
static GPDMA_LLI_T LLI_ADC_A;   /* ADC -> BUFFERA */
static GPDMA_LLI_T LLI_ADC_B;   /* ADC -> BUFFERB */
static GPDMA_LLI_T LLI_DAC;     /* BUFFER_SALIDA -> DAC */

/* ---------------------------------------------------------
 * ConfDMA_ADC_BUFFERS
 * Canal 1, P2M, lista enlazada circular (ping-pong).
 * Interrupcion TC habilitada.
 * --------------------------------------------------------- */
void ConfDMA_ADC_BUFFERS(void)
{
    /* --- Armar LLI ping-pong ---
     * Control word:
     *   bits[11:0] = BUFFER_SIZE  (cantidad de half-words)
     *   bit 18     = SRC burst 1
     *   bit 21     = DST burst 1
     *   bit 27     = DST increment (avanza en el buffer)
     */
    LLI_ADC_A.control = DMA_LLI_TRANSFER_SIZE(BUFFER_SIZE) | DMA_LLI_SRC_BSIZE_1 | DMA_LLI_DST_BSIZE_1 | DMA_LLI_DST_INC;
    LLI_ADC_A.srcAddr  = (uint32_t)(&LPC_ADC->ADGDR);
    LLI_ADC_A.dstAddr  = (uint32_t)(&BUFFERA[0]);
    LLI_ADC_A.nextLLI  = (uint32_t)(&LLI_ADC_B);

    LLI_ADC_B.control = DMA_LLI_TRANSFER_SIZE(BUFFER_SIZE) | DMA_LLI_SRC_BSIZE_1 | DMA_LLI_DST_BSIZE_1 | DMA_LLI_DST_INC;
    LLI_ADC_B.srcAddr  = (uint32_t)(&LPC_ADC->ADGDR);
    LLI_ADC_B.dstAddr  = (uint32_t)(&BUFFERB[0]);
    LLI_ADC_B.nextLLI  = (uint32_t)(&LLI_ADC_A);   /* Vuelve a A -> circular */

    /* --- Configurar canal DMA --- */
    GPDMA_Channel_CFG_T dmaCfg;
    dmaCfg.channelNum  = DMA_CH_ADC_CAPTURE;
    dmaCfg.transferSize = BUFFER_SIZE;
    dmaCfg.type        = GPDMA_P2M;
    dmaCfg.srcMemAddr  = (uint32_t)(&LPC_ADC->ADGDR);
    dmaCfg.dstMemAddr  = (uint32_t)(&BUFFERA[0]);
    dmaCfg.srcConn     = GPDMA_ADC;
    dmaCfg.dstConn     = 0;
    dmaCfg.intTC       = ENABLE;    /* Interrupcion al completar cada bloque */
    dmaCfg.intErr      = DISABLE;
    dmaCfg.linkedList  = (uint32_t)(&LLI_ADC_B);   /* Primera LLI encadenada */

    /* Ancho half-word, burst 1 */
    GPDMA_Endpoint_T endp;
    endp.width     = GPDMA_HALFWORD;
    endp.burst     = GPDMA_BSIZE_1;
    endp.increment = DISABLE;   /* Fuente (ADC ADGDR): no incrementar */
    dmaCfg.src     = endp;

    endp.increment = ENABLE;    /* Destino (buffer RAM): incrementar */
    dmaCfg.dst     = endp;

    GPDMA_Init();
    GPDMA_SetupChannel(&dmaCfg);

    NVIC_SetPriority(DMA_IRQn, DMA_NVIC_PRIORITY);
    NVIC_EnableIRQ(DMA_IRQn);
}

/* ---------------------------------------------------------
 * ConfDMA_BUFFERSALIDA_DAC
 * Canal 0, M2P, LLI circular (sin interrupcion).
 * El rate lo controla el temporizador interno del DAC.
 * --------------------------------------------------------- */
void ConfDMA_BUFFERSALIDA_DAC(void)
{
    /* Control word:
     *   bits[11:0] = BUFFER_SIZE
     *   bit 18     = SRC burst 1
     *   bit 21     = DST burst 1
     *   bit 26     = SRC increment (avanza en BUFFER_SALIDA)
     */
    LLI_DAC.control  = DMA_LLI_TRANSFER_SIZE(BUFFER_SIZE) | DMA_LLI_SRC_WIDTH | DMA_LLI_DST_WIDTH | DMA_LLI_SRC_INC;
    LLI_DAC.srcAddr  = (uint32_t)(&BUFFER_SALIDA[0]);
    LLI_DAC.dstAddr  = (uint32_t)(&LPC_DAC->DACR);
    LLI_DAC.nextLLI  = (uint32_t)(&LLI_DAC);       /* Se apunta a si mismo -> circular */

    GPDMA_Channel_CFG_T dmaCfg;
    dmaCfg.channelNum  = DMA_CH_DAC_SALIDA;
    dmaCfg.transferSize = BUFFER_SIZE;
    dmaCfg.type        = GPDMA_M2P;
    dmaCfg.srcMemAddr  = (uint32_t)(&BUFFER_SALIDA[0]);
    dmaCfg.dstMemAddr  = (uint32_t)(&LPC_DAC->DACR);
    dmaCfg.srcConn     = 0;
    dmaCfg.dstConn     = GPDMA_DAC;
    dmaCfg.intTC       = DISABLE;
    dmaCfg.intErr      = DISABLE;
    dmaCfg.linkedList  = (uint32_t)(&LLI_DAC);

    GPDMA_Endpoint_T endp;
    endp.width     = GPDMA_HALFWORD;
    endp.burst     = GPDMA_BSIZE_1;
    endp.increment = ENABLE;    /* Fuente (BUFFER_SALIDA): incrementar */
    dmaCfg.src     = endp;

    endp.increment = DISABLE;   /* Destino (DAC DACR): no incrementar */
    dmaCfg.dst     = endp;

    GPDMA_Init();
    GPDMA_SetupChannel(&dmaCfg);
    /* Sin NVIC: este canal no genera interrupcion */
}

/* ---------------------------------------------------------
 * DMA_IRQHandler
 * Alterna FLAGBUFFERA / FLAGBUFFERB para el doble buffer.
 * El flag se pone en 1 cuando el bloque correspondiente
 * esta lleno y listo para ser procesado por el DSP.
 * --------------------------------------------------------- */
void DMA_IRQHandler(void)
{
    static uint8_t direccion = 0U;

    if (direccion == 0U)
    {
        FLAGBUFFERA = 1U;
    }
    else
    {
        FLAGBUFFERB = 1U;
    }

    direccion ^= 1U;   /* Alterna entre 0 y 1 con XOR (mas claro que if/wrap) */
}
