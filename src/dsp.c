#include "dsp.h"

/* =========================================================
 * dsp.c
 * Implementacion del procesamiento digital de senales.
 * ========================================================= */

/* ---------------------------------------------------------
 * Definiciones de los buffers y variables globales.
 * Son declarados extern en config.h y definidos aqui
 * porque el DSP es el unico dueno de estos datos de audio.
 * --------------------------------------------------------- */
volatile uint16_t BUFFER_SALIDA[BUFFER_SIZE];
volatile uint16_t BUFFERA[BUFFER_SIZE];
volatile uint16_t BUFFERB[BUFFER_SIZE];

volatile uint8_t FLAGBUFFERA = 0;
volatile uint8_t FLAGBUFFERB = 0;

volatile FILTRO_IIR FILTRO_IIR_BAJO  = {0};
volatile FILTRO_IIR FILTRO_IIR_MEDIA = {0};
volatile FILTRO_IIR FILTRO_IIR_ALTA  = {0};

volatile float G_B = 1.0f;
volatile float G_M = 1.0f;
volatile float G_A = 1.0f;

/* ---------------------------------------------------------
 * FILTRO
 * Filtro IIR biquad de segundo orden, forma directa I.
 *
 *   y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2]
 *         - a1*y[n-1] - a2*y[n-2]
 * --------------------------------------------------------- */
float FILTRO(volatile FILTRO_IIR *f, float x_0)
{
    float y_0;

    y_0 = f->b0 * x_0 + f->b1 * f->x1 + f->b2 * f->x2 - f->a1 * f->y1 - f->a2 * f->y2;

    /* Actualizar estados de entrada */
    f->x2 = f->x1;
    f->x1 = x_0;

    /* Actualizar estados de salida */
    f->y2 = f->y1;
    f->y1 = y_0;

    return y_0;
}

/* ---------------------------------------------------------
 * Procesamiento_Senales
 * Procesa el buffer que este listo (ping-pong).
 * --------------------------------------------------------- */
void Procesamiento_Senales(void)
{
    float baja, media, alta;
    

    if (FLAGBUFFERA)
    {
        for (uint8_t i = 0; i < BUFFER_SIZE; i++)
        {
            baja  = FILTRO(&FILTRO_IIR_BAJO,  (float)BUFFERA[i]);
            media = FILTRO(&FILTRO_IIR_MEDIA, (float)BUFFERA[i]);
            alta  = FILTRO(&FILTRO_IIR_ALTA,  (float)BUFFERA[i]);

            BUFFER_SALIDA[i] = (uint16_t)(baja * G_B + media * G_M + alta * G_A);
        }
        FLAGBUFFERA = 0;   /* Limpiar flag DESPUES de procesar el bloque completo */
    }
    else if (FLAGBUFFERB)
    {
        for (i = 0; i < BUFFER_SIZE; i++)
        {
            baja  = FILTRO(&FILTRO_IIR_BAJO,  (float)BUFFERB[i]);
            media = FILTRO(&FILTRO_IIR_MEDIA, (float)BUFFERB[i]);
            alta  = FILTRO(&FILTRO_IIR_ALTA,  (float)BUFFERB[i]);

            BUFFER_SALIDA[i] = (uint16_t)(baja * G_B + media * G_M + alta * G_A);
        }
        FLAGBUFFERB = 0;   /* Limpiar flag DESPUES de procesar el bloque completo */
    }
}
