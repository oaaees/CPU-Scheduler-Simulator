## Simulador de Planificación de Procesos 

Una herramienta de software que simula varios algoritmos de planificación de procesos comúnmente utilizados en sistemas operativos. El objetivo del proyecto es obtener experiencia práctica en entender, implementar y analizar el comportamiento de diferentes algoritmos de planificación de procesos.

### TO-DO

Deben implementar varios algoritmos de planificación de procesos, estos son:

- No Expulsivos
    - ~~Primero en llegar primero en ejecutar (FCFS)~~
    - ~~Primero el trabajo más corto (SJF)~~
    - ~~Selección aleatoria~~
- Expulsivos
    - Planificación basada en prioridades
    - Turno rotativo (round robin)
    - Primero el menor tiempo restante (SRTF)
    - Planificación basada en prioridades

El simulador debe contar con una interfaz fácil de usar que permitirá a los usuarios:

- Generar de manera aleatoria detalles de procesos como tiempo de procesamiento (burst time), tiempo de procesamiento de E/S (IO burst time) y prioridad.
- Seleccionar y configurar diferentes algoritmos de planificación para observar su comportamiento.
- Definir el tiempo de cada paso (tick) del procesador y el tiempo de llegada.
- Explicar brevemente los parámetros para generar los detalles de cada proceso.
- Explicar brevemente el algoritmo en cuestión.


El simulador proporcionará visualizaciones para ayudar a los usuarios a comprender mejor el proceso de planificación, estas visualizaciones deben mostrar las actualizaciones en tiempo real sobre el estado de los procesos en la cola de listos, bloqueados, terminados y la CPU.

El simulador incluirá estadísticas para analizar el rendimiento de los algoritmos de planificación, para ello deben calcular % de uso del procesador, tiempo promedio de espera, tiempo promedio de bloqueo, tiempo promedio de ejecución, total de procesos completados, arribo de nuevos procesos por paso y tiempo total. 
