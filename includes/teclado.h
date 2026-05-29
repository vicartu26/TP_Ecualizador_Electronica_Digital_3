#ifndef TECLADO_H
#define TECLADO_H

#include "config.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"

/* =========================================================
 * teclado.h
 * Manejo del teclado de tres botones para seleccion de
 * banda activa del ecualizador.
 *
 * Pinout:
 *   P0.0 -> Banda Baja
 *   P0.1 -> Banda Media
 *   P0.2 -> Banda Alta
 *
 * Logica: Pull-Up interna activa.
 *   Reposo  -> pin en '1' (VCC a traves de pull-up)
 *   Pulsado -> pin en '0' (cortocircuito a GND)
 *   Interrupcion por flanco de bajada (FALLING).
 *
 * NOTA: Se recomienda antirrebote por hardware (RC + Schmitt)
 * en cada boton para evitar multiples disparos.
 * ========================================================= */

/* ---------------------------------------------------------
 * Puerto y mascara de pines del teclado
 * --------------------------------------------------------- */
#define KEYBOARD_PORT   PORT_0
#define KEYBOARD_MASK   ((1U << 0) | (1U << 1) | (1U << 2))

/* ---------------------------------------------------------
 * Codigos de protocolo enviados por UART a la PC para
 * indicar que banda fue seleccionada.
 * --------------------------------------------------------- */
#define SELECCION_BAJOS    0x10U
#define SELECCION_MEDIOS   0x11U
#define SELECCION_AGUDOS   0x12U

/* ---------------------------------------------------------
 * Prioridad NVIC del teclado.
 * Menor prioridad que DMA (0) y UART (5).
 * --------------------------------------------------------- */
#define TECLADO_NVIC_PRIORITY  6U

/* ---------------------------------------------------------
 * ConfTeclado
 *
 * Configura P0.0, P0.1 y P0.2 como entradas GPIO con
 * Pull-Up e interrupcion por flanco de bajada (EINT3).
 * --------------------------------------------------------- */
void ConfTeclado(void);

/* ---------------------------------------------------------
 * EINT3_IRQHandler
 *
 * Manejador de interrupcion del GPIO Port 0 / Port 2
 * (linea EINT3 del LPC17xx).
 * Actualiza BANDA_ACTIVA y envia el codigo de seleccion
 * por UART1 hacia la PC.
 * --------------------------------------------------------- */
void EINT3_IRQHandler(void);

#endif /* TECLADO_H */
