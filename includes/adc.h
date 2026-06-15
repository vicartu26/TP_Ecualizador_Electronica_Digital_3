#ifndef ADC_H
#define ADC_H

#include "config.h"
#include "lpc17xx_adc.h"

/* =========================================================
 * adc.h
 * Configuracion del ADC del LPC1769 para captura de audio
 * mediante DMA.
 *
 * El ADC opera en modo BURST:
 * realiza conversiones continuas sobre el canal habilitado
 * sin necesidad de disparos externos por temporizador.
 *
 * La frecuencia de conversion se configura mediante
 * ADC_Init(rate), donde rate representa la cantidad
 * aproximada de conversiones por segundo deseadas.
 *
 * Para captura de audio con un unico canal:
 *      Fs ≈ ADC_FS_HZ
 *
 * NOTA:
 * - Si se habilitan varios canales ADC en modo BURST,
 *   la frecuencia efectiva por canal disminuye al
 *   repartirse las conversiones entre ellos.
 * - La frecuencia de muestreo utilizada para diseñar
 *   los filtros IIR debe coincidir con ADC_FS_HZ y con
 *   la frecuencia configurada en el DAC.
 * ========================================================= */

/* ---------------------------------------------------------
 * Frecuencia de muestreo del sistema de audio.
 *
 * Determina:
 *   - Frecuencia de conversion ADC
 *   - Frecuencia de diseño de los filtros IIR
 *   - Frecuencia de reproduccion DAC
 *
 * --------------------------------------------------------- */
#define ADC_FS_HZ    44100U

/* ---------------------------------------------------------
 * Canal ADC utilizado para captura de audio.
 *
 * P0.23 -> AD0.0
 * --------------------------------------------------------- */
#define ADC_AUDIO_CHANNEL    ADC_CHANNEL_0

/* ---------------------------------------------------------
 * ConfADC
 *
 * Inicializa el ADC con:
 *   - Alimentacion habilitada
 *   - Canal de audio configurado
 *   - Modo BURST habilitado
 *   - Conversion continua
 *
 * Debe llamarse antes de habilitar el DMA de captura
 * para asegurar que el ADC ya este generando muestras.
 * --------------------------------------------------------- */
void ConfADC(void);

#endif /* ADC_H */