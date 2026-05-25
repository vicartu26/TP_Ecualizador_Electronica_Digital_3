# TP_Ecualizador_Electronica_Digital_3
README — Ecualizador Digital con LPC1769
Descripción general

Este proyecto consiste en el desarrollo de un ecualizador digital de audio implementado con un LPC1769.

La señal analógica de entrada es adquirida mediante el ADC del microcontrolador, procesada digitalmente aplicando filtros por banda previamente almacenados en memoria, y posteriormente reconstruida a través del DAC.

El sistema permite modificar dinámicamente la ganancia de distintas bandas de frecuencia.

Funcionamiento del sistema
La señal analógica ingresa al sistema por el ADC.
El microcontrolador procesa la señal utilizando filtros digitales por banda.
Mediante un teclado matricial, el usuario selecciona la banda de frecuencia que desea modificar.
La banda seleccionada se muestra por la pantalla LED.
La ganancia correspondiente a dicha banda se ingresa a través de la UART.
La señal filtrada es multiplicada por la ganancia configurada.
Finalmente, la señal procesada se envía al DAC para su salida analógica.
Periféricos utilizados:
  ADC
  GPDMA
  DAC
  UART
  GPIO
  Teclado matricial
  Pantalla LED
