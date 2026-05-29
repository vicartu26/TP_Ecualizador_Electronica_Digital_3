#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include "lpc17xx.h"         /* Registro base del microcontrolador */

/* =========================================================
 * config.h
 * Tipos, estructuras, buffers y variables globales
 * compartidos por todos los modulos del ecualizador.
 * ========================================================= */

/* ---------------------------------------------------------
 * Tamano de los buffers de audio (muestras por bloque)
 * --------------------------------------------------------- */
#define BUFFER_SIZE  64U

/* ---------------------------------------------------------
 * Tipo de banda activa (seleccionada por teclado)
 * --------------------------------------------------------- */
typedef enum {
    BANDA_BAJA,
    BANDA_MEDIA,
    BANDA_ALTA
} TIPO_BANDA;

/* ---------------------------------------------------------
 * Estructura del filtro IIR de segundo orden (Biquad)
 * Forma directa II transpuesta
 *   y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2]
 *         - a1*y[n-1] - a2*y[n-2]
 * --------------------------------------------------------- */
typedef struct {
    /* Coeficientes del numerador */
    float b0;
    float b1;
    float b2;

    /* Coeficientes del denominador (sin a0, normalizado) */
    float a1;
    float a2;

    /* Estados: entradas anteriores */
    float x1;
    float x2;

    /* Estados: salidas anteriores */
    float y1;
    float y2;
} FILTRO_IIR;

/* ---------------------------------------------------------
 * Buffers de audio (doble buffer ping-pong para ADC + DMA)
 * --------------------------------------------------------- */
extern volatile uint16_t BUFFER_SALIDA[BUFFER_SIZE];
extern volatile uint16_t BUFFERA[BUFFER_SIZE];
extern volatile uint16_t BUFFERB[BUFFER_SIZE];

/* Flags seteados por DMA_IRQHandler, leidos por DSP */
extern volatile uint8_t FLAGBUFFERA;
extern volatile uint8_t FLAGBUFFERB;

/* ---------------------------------------------------------
 * Filtros IIR para cada banda
 * Los coeficientes deben cargarse antes de procesar audio.
 * --------------------------------------------------------- */
extern volatile FILTRO_IIR FILTRO_IIR_BAJO;
extern volatile FILTRO_IIR FILTRO_IIR_MEDIA;
extern volatile FILTRO_IIR FILTRO_IIR_ALTA;

/* ---------------------------------------------------------
 * Ganancias por banda  [0.0 .. 1.0]
 * Modificadas desde UART1_IRQHandler
 * --------------------------------------------------------- */
extern volatile float G_B;   /* Ganancia banda baja  */
extern volatile float G_M;   /* Ganancia banda media */
extern volatile float G_A;   /* Ganancia banda alta  */

/* ---------------------------------------------------------
 * Banda actualmente seleccionada por el teclado
 * --------------------------------------------------------- */
extern volatile TIPO_BANDA BANDA_ACTIVA;

#endif /* CONFIG_H */
