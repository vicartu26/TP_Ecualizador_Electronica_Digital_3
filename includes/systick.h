#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

/* Configura el SysTick para que genere una interrupcion cada 1 ms.
 * Llamar una sola vez en main(), antes del while(1). */
void ConfSysTick(void);

/* Devuelve la cantidad de milisegundos transcurridos desde el reset
 * (o desde el ultimo overflow). Pensado para usarse en el antirrebote
 * de los pulsadores u otras mediciones de tiempo no bloqueantes. */
uint32_t SysTick_GetTicks(void);

#endif /* SYSTICK_H */