#include "teclado.h"

/* =========================================================
 * teclado.c
 * Implementacion del teclado y su manejador de interrupcion.
 * ========================================================= */

/* ---------------------------------------------------------
 * Definicion de la banda activa.
 * Declarada extern en config.h; definida aqui porque el
 * teclado es el unico modulo que la escribe.
 * --------------------------------------------------------- */
volatile TIPO_BANDA BANDA_ACTIVA = BANDA_BAJA;

/* ---------------------------------------------------------
 * ConfTeclado
 * --------------------------------------------------------- */
void ConfTeclado(void)
{
    PINSEL_CFG_T pinCfg;

    /* 1. Configurar pines como GPIO con Pull-Up interna */
    pinCfg.port      = KEYBOARD_PORT;
    pinCfg.func      = PINSEL_FUNC_00;   /* Funcion 0 = GPIO */
    pinCfg.mode      = PINSEL_PULLUP;    /* Estado pasivo = '1' */
    pinCfg.openDrain = DISABLE;
    PINSEL_ConfigMultiplePins(&pinCfg, KEYBOARD_MASK);

    /* 2. Configurar como entradas */
    GPIO_SetDir(KEYBOARD_PORT, KEYBOARD_MASK, GPIO_INPUT);

    /* 3. Interrupcion por flanco de bajada
     *    (boton presionado: 1 -> 0) */
    GPIO_IntConfigPort(KEYBOARD_PORT, KEYBOARD_MASK, GPIO_INT_FALLING);

    /* 4. Habilitar en el NVIC (GPIO Port 0/2 comparten EINT3) */
    NVIC_SetPriority(EINT3_IRQn, TECLADO_NVIC_PRIORITY);
    NVIC_EnableIRQ(EINT3_IRQn);
}

/* ---------------------------------------------------------
 * EINT3_IRQHandler
 *
 * Identifica el pin que genero la interrupcion, actualiza
 * BANDA_ACTIVA y notifica a la PC por UART.
 * --------------------------------------------------------- */
void EINT3_IRQHandler(void)
{
    if (GPIO_GetPinIntStatus(KEYBOARD_PORT, PIN_0, GPIO_INT_FALLING))
    {
        BANDA_ACTIVA = BANDA_BAJA;
        UART_SendByte(LPC_UART1, SELECCION_BAJOS);
        GPIO_ClearInt(KEYBOARD_PORT, PIN_0);
    }
    else if (GPIO_GetPinIntStatus(KEYBOARD_PORT, PIN_1, GPIO_INT_FALLING))
    {
        BANDA_ACTIVA = BANDA_MEDIA;
        UART_SendByte(LPC_UART1, SELECCION_MEDIOS);
        GPIO_ClearInt(KEYBOARD_PORT, PIN_1);
    }
    else if (GPIO_GetPinIntStatus(KEYBOARD_PORT, PIN_2, GPIO_INT_FALLING))
    {
        BANDA_ACTIVA = BANDA_ALTA;
        UART_SendByte(LPC_UART1, SELECCION_AGUDOS);
        GPIO_ClearInt(KEYBOARD_PORT, PIN_2);
    }
}
