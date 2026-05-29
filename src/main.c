/* =========================================================
 * main.c
 * Ecualizador de audio de tres bandas - LPC1769
 *
 * Arquitectura:
 *   - ADC + DMA (Canal 1): captura muestras en doble buffer
 *     (BUFFERA / BUFFERB) a la frecuencia de muestreo.
 *   - DSP (loop principal): cuando un buffer esta lleno,
 *     aplica tres filtros IIR (bajo / medio / alto) y
 *     mezcla las salidas ponderadas en BUFFER_SALIDA.
 *   - DAC + DMA (Canal 0): reproduce BUFFER_SALIDA de forma
 *     continua y circular, cadenciado por el timer del DAC.
 *   - UART1: recibe ganancias por banda desde la PC.
 *   - Teclado (EINT3): selecciona la banda activa y
 *     notifica a la PC el cambio de seleccion.
 *
 * Orden de inicializacion (importante):
 *   1. ConfDAC              -> timer DAC listo antes que el DMA
 *   2. ConfDMA_ADC_BUFFERS  -> captura desde ADC habilitada
 *   3. ConfDMA_BUFFERSALIDA_DAC -> reproduccion hacia DAC
 *   4. ConfUART             -> comunicacion con PC
 *   5. ConfTeclado          -> seleccion de banda
 * ========================================================= */

#include "config.h"
#include "dac/dac.h"
#include "dma/dma.h"
#include "uart/uart.h"
#include "teclado/teclado.h"
#include "dsp/dsp.h"

int main(void)
{
    /* --- Inicializacion de perifericos --- */
    ConfDAC();                  /* 1. DAC: habilitar timer antes que el DMA */
    ConfDMA_ADC_BUFFERS();      /* 2. DMA canal 1: ADC -> BUFFERA/BUFFERB   */
    ConfDMA_BUFFERSALIDA_DAC(); /* 3. DMA canal 0: BUFFER_SALIDA -> DAC     */
    ConfUART();                 /* 4. UART1: recepcion de ganancias          */
    ConfTeclado();              /* 5. Teclado: seleccion de banda            */

    /* --- Loop principal ---
     * Procesamiento_Senales() verifica los flags de doble buffer
     * y procesa el bloque disponible. Si ningun flag esta activo
     * retorna inmediatamente sin hacer nada (sin bloqueo). */
    while (1)
    {
        Procesamiento_Senales();
    }

    /* Nunca se alcanza */
    return 0;
}
