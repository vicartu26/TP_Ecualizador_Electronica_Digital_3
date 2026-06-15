#include "dsp.h"
#include "dma.h"
//me parece que va a hacer falta un include de config
/* =========================================================
 * dsp.c
 * Implementacion del procesamiento digital de senales.
 * ========================================================= */

/* ---------------------------------------------------------
 * Definiciones de los buffers y variables globales.
 * Son declarados extern en config.h y definidos aqui
 * porque el DSP es el unico dueno de estos datos de audio.
 * --------------------------------------------------------- */
volatile uint16_t BUFFER_SALIDA_A[BUFFER_SIZE];
volatile uint16_t BUFFER_SALIDA_B[BUFFER_SIZE];
volatile uint16_t BUFFERA[BUFFER_SIZE];
volatile uint16_t BUFFERB[BUFFER_SIZE];

volatile uint8_t FLAGBUFFERA = 0;
volatile uint8_t FLAGBUFFERB = 0;
volatile uint8_t DAC_BUFFER_ACTIVO = 0;

volatile FILTRO_IIR FILTRO_IIR_BAJO  = {0};
volatile FILTRO_IIR FILTRO_IIR_MEDIA = {0};
volatile FILTRO_IIR FILTRO_IIR_ALTA  = {0};

volatile float G_B = 1.0f;
volatile float G_M = 1.0f;
volatile float G_A = 1.0f;

static uint8_t dac_iniciado = 0U;

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


void InicializarFiltros(void)
{
    // -------------------------------------------------------
    // BAJOS: Lowpass Butterworth fc=250 Hz, Q=0.707, Fs=44100
    // -------------------------------------------------------
    FILTRO_IIR_BAJO.b0 =  0.0003093542f;
    FILTRO_IIR_BAJO.b1 =  0.0006187085f;
    FILTRO_IIR_BAJO.b2 =  0.0003093542f;
    FILTRO_IIR_BAJO.a1 = -1.9496374359f;
    FILTRO_IIR_BAJO.a2 =  0.9508748529f;
    FILTRO_IIR_BAJO.x1 = 0.0f;
    FILTRO_IIR_BAJO.x2 = 0.0f;
    FILTRO_IIR_BAJO.y1 = 0.0f;
    FILTRO_IIR_BAJO.y2 = 0.0f;

    // -------------------------------------------------------
    // MEDIOS: Bandpass fc_central=1000 Hz, BW=250-4000 Hz, Fs=44100
    // -------------------------------------------------------
    FILTRO_IIR_MEDIA.b0 =  0.0560693041f;
    FILTRO_IIR_MEDIA.b1 =  0.0000000000f;
    FILTRO_IIR_MEDIA.b2 = -0.0560693041f;
    FILTRO_IIR_MEDIA.a1 = -1.5634760928f;
    FILTRO_IIR_MEDIA.a2 =  0.5794802192f;
    FILTRO_IIR_MEDIA.x1 = 0.0f;
    FILTRO_IIR_MEDIA.x2 = 0.0f;
    FILTRO_IIR_MEDIA.y1 = 0.0f;
    FILTRO_IIR_MEDIA.y2 = 0.0f;

    // -------------------------------------------------------
    // AGUDOS: Highpass Butterworth fc=4000 Hz, Q=0.707, Fs=44100
    // -------------------------------------------------------
    FILTRO_IIR_ALTA.b0 =  0.6666400407f;
    FILTRO_IIR_ALTA.b1 = -1.3332800815f;
    FILTRO_IIR_ALTA.b2 =  0.6666400407f;
    FILTRO_IIR_ALTA.a1 = -1.2188793364f;
    FILTRO_IIR_ALTA.a2 =  0.4476808265f;
    FILTRO_IIR_ALTA.x1 = 0.0f;
    FILTRO_IIR_ALTA.x2 = 0.0f;
    FILTRO_IIR_ALTA.y1 = 0.0f;
    FILTRO_IIR_ALTA.y2 = 0.0f;
}

/* ---------------------------------------------------------
 * Procesamiento_Senales
 * Procesa el buffer que este listo (ping-pong).
 * --------------------------------------------------------- */
void Procesamiento_Senales(void)
{
    float baja, media, alta;
    volatile uint16_t *buf_escritura;
    volatile uint16_t *buf_lectura;

    if (!FLAGBUFFERA && !FLAGBUFFERB) return;

    
    if (DAC_BUFFER_ACTIVO == 0U) {
        buf_escritura = BUFFER_SALIDA_B;
    } else {
        buf_escritura = BUFFER_SALIDA_A;
    }


    if (FLAGBUFFERA){
           buf_lectura = BUFFERA;
    }else{
           buf_lectura = BUFFERB;
    }


        for (uint8_t i = 0; i < BUFFER_SIZE; i++)
        {
            baja  = FILTRO(&FILTRO_IIR_BAJO,  (float)buf_lectura[i]);
            media = FILTRO(&FILTRO_IIR_MEDIA, (float)buf_lectura[i]);
            alta  = FILTRO(&FILTRO_IIR_ALTA,  (float)buf_lectura[i]);

            buf_escritura[i] = (uint16_t)(baja * G_B + media * G_M + alta * G_A);
        }
         /* Limpiar flag DESPUES de procesar el bloque completo */

    if(FLAGBUFFERA) FLAGBUFFERA = 0;
    else FLAGBUFFERB = 0;

    DAC_BUFFER_ACTIVO ^= 1U;

    if(!dac_iniciado){
    	ConfDMA_BUFFERSALIDA_DAC();
    	dac_iniciado = 1;
    }

    }


