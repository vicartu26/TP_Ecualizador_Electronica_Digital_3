#include "uart.h"

/* =========================================================
 * uart.c
 * Implementacion de la configuracion UART1 y su handler.
 * ========================================================= */

/* ---------------------------------------------------------
 * ConfUART
 * --------------------------------------------------------- */
void ConfUART(void)
{
    /* Configuracion del frame */
    UART_CFG_T uartCfg;
    uartCfg.baudRate = UART_BAUD_RATE;
    uartCfg.dataBits = UART_DATA_BITS;
    uartCfg.stopBits = UART_STOP_BITS;
    uartCfg.parity   = UART_PARITY_CFG;

    /* Configuracion de la FIFO */
    UART_FIFO_CFG_T fifoCfg;
    fifoCfg.level      = UART_FIFO_LEVEL;
    fifoCfg.resetRxBuf = ENABLE;   /* Limpia el buffer de recepcion al iniciar */
    fifoCfg.resetTxBuf = ENABLE;   /* Limpia el buffer de transmision al iniciar */
    fifoCfg.dmaMode    = DISABLE;  /* DMA no usado para UART */

    /* Asignacion de pines fisicos */
    UART_PinConfig(UART_TX1_P0_15);
    UART_PinConfig(UART_RX1_P0_16);

    /* Inicializacion del periferico */
    UART_Init(LPC_UART1, &uartCfg);
    UART_FIFOConfig(LPC_UART1, &fifoCfg);

    /* Habilitar transmision por hardware */
    UART_TxEnable(LPC_UART1);

    /* Habilitar interrupcion por dato recibido (RBR) */
    UART_IntConfig(LPC_UART1, UART_INT_RBR, ENABLE);

    /* Configurar NVIC */
    NVIC_SetPriority(UART1_IRQn, UART_NVIC_PRIORITY);
    NVIC_EnableIRQ(UART1_IRQn);
}

/* ---------------------------------------------------------
 * UART1_IRQHandler
 *
 * Convierte el byte recibido en ganancia y lo asigna
 * a la banda activa.
 *
 * Escala:
 *   0x00 (  0) -> 0.0  (silencio)
 *   0x7F (127) -> ~0.5 (mitad de amplitud)
 *   0xFF (255) -> 1.0  (amplitud maxima sin saturar)
 *
 * NOTA: si en el futuro se necesita amplificacion (>1.0),
 * ajustar el divisor o usar un rango distinto.
 * --------------------------------------------------------- */
void UART1_IRQHandler(void)
{
    uint8_t dato = UART_ReceiveByte(LPC_UART1);

    float nueva_ganancia = (float)dato / 255.0f;

    switch (BANDA_ACTIVA)
    {
        case BANDA_BAJA:   G_B = nueva_ganancia; break;
        case BANDA_MEDIA:  G_M = nueva_ganancia; break;
        case BANDA_ALTA:   G_A = nueva_ganancia; break;
        default: break;
    }
}
