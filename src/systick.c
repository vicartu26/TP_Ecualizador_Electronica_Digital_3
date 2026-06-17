#include "systick.h"
#include "lpc17xx_systick.h"   /* Driver propio del proyecto */

/* Contador interno de milisegundos. Es 'static' para que solo este
 * archivo pueda modificarlo: el resto del proyecto lo consulta a
 * traves de SysTick_GetTicks(). */
static volatile uint32_t msTicks = 0;

void ConfSysTick(void)
{
    /* Configura el LOAD para un periodo de 1 ms usando el clock interno */
    SYSTICK_InternalInit(1);

    /* Habilita la excepcion de SysTick (llama a SysTick_Handler al llegar a 0) */
    SYSTICK_IntCmd(ENABLE);

    /* Arranca el contador */
    SYSTICK_Cmd(ENABLE);

    NVIC_SetPriority(SysTick_IRQn, 7U);  /* Menor prioridad que DMA (0) y UART (5) */
    NVIC_EnableIRQ(SysTick_IRQn);
}

uint32_t SysTick_GetTicks(void)
{
    return msTicks;
}

void SysTick_Handler(void)
{
    msTicks++;  /* Incrementa el contador cada vez que se genera la interrupcion (cada 1 ms) */
}