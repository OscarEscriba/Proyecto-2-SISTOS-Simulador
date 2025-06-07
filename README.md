# Proyecto Simulador de Planificación y Sincronización

**Curso:** Sistemas Operativos  
**Universidad del Valle de Guatemala – Sección 30**  
**Docente:** Juan Carlos Canteo Boix  
**Integrantes:** Cindy Gualim, Oscar Escriba 

**Fecha de entrega:** 6 de junio de 2025

---

## Descripción para el usuario

Este proyecto es una aplicación educativa diseñada para simular visualmente cómo funcionan distintos algoritmos de planificación de procesos en un sistema operativo. Está desarrollada en lenguaje C y utiliza la biblioteca gráfica GTK+ para ofrecer una interfaz amigable al usuario.

Su propósito principal es permitir que un usuario pueda observar de manera clara cómo se comportan los procesos en diferentes escenarios de ejecución, dependiendo del algoritmo de calendarización seleccionado. La simulación está pensada para reforzar el aprendizaje de temas como planificación de procesos, control de tiempos de llegada, duración y prioridad.

La aplicación permite cargar un archivo de texto con una lista de procesos definidos por su identificador, tiempo de ejecución (burst time), tiempo de llegada (arrival time) y prioridad. Luego, el usuario puede seleccionar uno o más algoritmos como FIFO, SJF, SRTF, Round Robin o Prioridad. Si se selecciona Round Robin, el usuario también puede configurar el valor del quantum.

Una vez cargados los procesos y seleccionado el algoritmo, el sistema ejecuta la simulación y muestra en pantalla un diagrama visual tipo Gantt, que representa cómo se asigna el CPU a cada proceso a lo largo del tiempo. También muestra estadísticas como el tiempo promedio de espera y el tiempo promedio de retorno de los procesos simulados.

El proyecto está diseñado para ser ampliado en el futuro con la simulación de mecanismos de sincronización como mutex y semáforos, aunque esa parte aún no está implementada. Además, incluye opciones para reiniciar la simulación, cambiar los parámetros y volver a cargar otros conjuntos de procesos.

---

## Funcionalidades implementadas

### A. Simulador de Algoritmos de Calendarización
- Algoritmos implementados:
  - FIFO (First-In First-Out)
  - SJF (Shortest Job First)
  - SRTF (Shortest Remaining Time First)
  - Round Robin (con quantum configurable)
  - Prioridad (pendiente implementación de envejecimiento)

- Carga dinámica de procesos desde archivos `.txt`:

<PID>, <BT>, <AT>, <Priority>
Ejemplo:
P1, 8, 0, 1

- Visualización dinámica de ejecución:
- Diagrama de Gantt generado visualmente
- Scroll vertical disponible
- Métricas mostradas al finalizar:
  - Tiempo promedio de espera
  - Tiempo promedio de retorno

### B. Simulador de Sincronización
- No implementado en esta versión (mutex/semaphore y acciones sobre recursos)

---

## Interfaz Gráfica (GUI)

- Selección entre calendarización y sincronización
- Elección de algoritmos mediante checkboxes
- Campo de entrada para configurar quantum
- Botones para:
- Cargar archivo de procesos
- Iniciar simulación
- Limpiar pantalla
- Área de dibujo del diagrama de ejecución
- Visualización de métricas dinámicas

---

## Formato del archivo de entrada

### Archivo de Procesos (`procesos.txt`)
```txt
P1, 5, 0, 2
P2, 3, 1, 1
P3, 7, 2, 3
Donde:

PID = nombre del proceso

BT = burst time (tiempo de ejecución)

AT = arrival time (tiempo de llegada)

Priority = prioridad del proceso (1 = más alta)


