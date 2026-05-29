#ifndef DSP_H
#define DSP_H

#include "config.h"

/* =========================================================
 * dsp.h
 * Procesamiento Digital de Señales:
 *  - Filtro IIR biquad de segundo orden
 *  - Ecualizacion de tres bandas con ganancia variable
 * ========================================================= */

/* ---------------------------------------------------------
 * Procesamiento_Senales
 *
 * Debe llamarse en el loop principal (main).
 * Verifica FLAGBUFFERA / FLAGBUFFERB; cuando alguno esta
 * activo aplica los tres filtros IIR a cada muestra del
 * buffer correspondiente y escribe el resultado ponderado
 * en BUFFER_SALIDA.
 *
 * BUFFER_SALIDA[i] = BAJA*G_B + MEDIA*G_M + ALTA*G_A
 * --------------------------------------------------------- */
void Procesamiento_Senales(void);

/* ---------------------------------------------------------
 * FILTRO
 *
 * Ejecuta un paso del filtro IIR biquad sobre la muestra
 * de entrada x_0 y actualiza los estados internos del
 * filtro apuntado por f.
 *
 * @param f    Puntero al filtro IIR (coeficientes + estados)
 * @param x_0  Muestra de entrada actual
 * @return     Muestra de salida filtrada
 * --------------------------------------------------------- */
float FILTRO(volatile FILTRO_IIR *f, float x_0);

#endif /* DSP_H */
