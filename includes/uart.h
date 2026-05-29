#ifndef UART_H
#define UART_H

#include "config.h"
#include "lpc17xx_uart.h"

/* =========================================================
 * uart.h
 * Configuracion de UART1 para recibir ganancias por banda
 * desde la PC y enviar el codigo de banda seleccionada
 * cuando el usuario pulsa una tecla.
 *
 * Pinout:
 *   TX -> P0.15  (UART1_TX)
 *   RX -> P0.16  (UART1_RX)
 *
 * Protocolo de recepcion (1 byte):
 *   valor / 255.0 -> ganancia de la banda activa [0.0 .. 1.0]
 *
 * Protocolo de envio (1 byte, definido en teclado.h):
 *   SELECCION_BAJOS   0x10
 *   SELECCION_MEDIOS  0x11
 *   SELECCION_AGUDOS  0x12
 * ========================================================= */

/* ---------------------------------------------------------
 * Parametros de la UART
 * --------------------------------------------------------- */
#define UART_BAUD_RATE     115200U
#define UART_DATA_BITS     UART_DBITS_8
#define UART_STOP_BITS     UART_STOPBIT_1
#define UART_PARITY_CFG    UART_PARITY_NONE

/* Nivel de FIFO: interrumpe apenas llega 1 byte */
#define UART_FIFO_LEVEL    UART_FIFO_TRGLEV0

/* Prioridad NVIC de la UART (menor que DMA y mayor que teclado) */
#define UART_NVIC_PRIORITY  5U

/* ---------------------------------------------------------
 * ConfUART
 *
 * Inicializa UART1 a 115200 8N1.
 * Habilita la interrupcion RBR (dato recibido).
 * --------------------------------------------------------- */
void ConfUART(void);

/* ---------------------------------------------------------
 * UART1_IRQHandler
 *
 * Lee el byte recibido y actualiza la ganancia de la
 * banda actualmente seleccionada (BANDA_ACTIVA).
 *
 *   ganancia = (float)dato / 255.0f
 * --------------------------------------------------------- */
void UART1_IRQHandler(void);

#endif /* UART_H */
