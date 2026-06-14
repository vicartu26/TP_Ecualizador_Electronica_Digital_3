# Filtros de señal de entrada

## Offset

Usamos un divisor resistivo (con la fuente de 3.3V y dos resistencias iguales tenemos un offset de 1.5V). El capacitor de acople (**C3 de 1uF**) bloquea la continua del celular y deja pasar únicamente la señal de audio. El **C1** estabiliza la referencia de 1.65V (eliminando ruido).

### Componentes

| Componente | Valor | Función |
|------------|-------|---------|
| V1 | 3.3V | Fuente de alimentación |
| R1, R2 | 6.8kΩ | Divisor resistivo (offset 1.65V) |
| C1 | 100nF | Estabilización de referencia |
| C3 | 1µF | Capacitor de acople (bloquea DC) |
| V2 | SINE(0 0.5 1000) | Señal de audio de entrada |

### Resultado

La señal de salida queda centrada en **1.65V** con la componente de audio superpuesta, lista para ingresar al ADC.

---

## Filtro pasa bajos

Filtro pasa bajos **activo de 2do orden Sallen-Key** con frecuencia de corte de **20kHz**.

Nuestra frecuencia de muestreo es de 44.1kHz, entonces para cumplir el **Teorema de Nyquist**, debemos recibir como máxima frecuencia de entrada 22kHz.

Se utilizó una página web para los cálculos y se realizó una simulación en **LTSpice**, verificando la frecuencia de corte (−3dB) en aproximadamente 20kHz.

### Componentes

| Componente | Valor | Función |
|------------|-------|---------|
| R1, R2 | 11kΩ | Resistencias del filtro |
| C1 | 1nF | Capacitor del filtro |
| C2 | 0.5nF | Capacitor del filtro |
| U1 | Op-Amp | Amplificador operacional (unity gain) |
| V2, V3 | 3.3V | Alimentación simétrica |

### Parámetros de simulación (AC)

```spice
.ac dec 100 10 1Meg
```

### Resultado de simulación

- **Frecuencia de corte:** ~20.58kHz
- **Atenuación en fc:** −3.065dB
- **Fase en fc:** −90.73°
- **Group Delay:** ~10.96µs

---

## Conexión de ambas partes

Se conecta el bloque de offset con el filtro pasa bajos. La simulación conjunta confirma la frecuencia de corte en ~20.58kHz con −2.04dB, manteniendo el comportamiento esperado del sistema completo.

### Parámetros de simulación

```spice
.ac dec 100 10 100k
```

---

# Filtro de reconstrucción y adaptación de señal de salida

Otro filtro de **segundo orden con frecuencia de corte de 22kHz**, ubicado a la salida del DAC para reconstruir la señal analógica.

### Componentes

| Componente | Valor | Función |
|------------|-------|---------|
| R, R1 | 10kΩ | Resistencias del filtro |
| C1 | 1nF | Capacitor del filtro |
| C2 | 0.5nF | Capacitor del filtro |
| U1 | Op-Amp | Amplificador operacional |
| C3 | 10µF | Capacitor de acople de salida |
| R2 | 10kΩ | Resistencia de carga (Jack) |
| V1 | 3.3V | Alimentación |
| V2 | 3.3V | Alimentación negativa |
| V3 | SINE(1.65 1) AC 1 | Señal de entrada centrada en 1.65V |

### Resultado de simulación

- **Frecuencia de corte:** ~22.62kHz
- **Atenuación en fc:** −3.05dB
- **Fase en fc:** −90.87°
- **Group Delay:** ~9.90µs

### Adaptación de salida

Se coloca un **capacitor en serie (C3 = 10µF)** y una **resistencia (R2 = 10kΩ)** para centrar la señal de vuelta en 0V. Estos componentes actúan como un **filtro pasa altos** con:

$$f_c \approx \frac{1}{2\pi \cdot R_2 \cdot C_3} = \frac{1}{2\pi \cdot 10k \cdot 10\mu} \approx 1.6\text{Hz}$$

Esto elimina el offset de 1.65V a la salida, entregando una señal de audio pura centrada en 0V al conector Jack.

---

## Resumen del sistema

```
[Celular/Audio] → [Offset + Acople] → [Filtro Pasa Bajos 20kHz] → [ADC 44.1kHz]
                                                                         ↓
[Jack Salida]  ← [Pasa Altos 1.6Hz] ← [Filtro Reconstrucción 22kHz] ← [DAC]
```

| Etapa | Tipo | Frecuencia de corte |
|-------|------|---------------------|
| Entrada | Offset + acople DC | — |
| Anti-aliasing | Pasa bajos Sallen-Key 2do orden | 20kHz |
| Reconstrucción | Pasa bajos Sallen-Key 2do orden | 22kHz |
| Salida | Pasa altos (acople AC) | ~1.6Hz |
