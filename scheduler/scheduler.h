/* scheduler.h - Cabecera para algoritmos de planificación */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_PROCESOS 100
#define MAX_LINEA 100

typedef struct {
    char pid[10];     // ID del proceso
    int bt;           // Burst Time
    int at;           // Arrival Time
    int prioridad;    // Prioridad
    int tiempo_restante; // Para algoritmos como SRT
    int tiempo_inicio;
    int tiempo_final;
    int tiempo_espera;
    int tiempo_retorno;
    int ejecutado;    // Flag para indicar si ya fue ejecutado
} Proceso;

typedef struct {
    int tiempo_inicio;
    int tiempo_fin;
    char pid[10];
} RangoEjecucion;
RangoEjecucion* round_robin(Proceso *procesos, int n, int quantum, int *num_rangos);


// Funciones generales
int cargar_procesos(const char* nombre_archivo, Proceso procesos[], int* cantidad);
void imprimir_procesos(Proceso procesos[], int cantidad);

// Algoritmos de scheduling
void ejecutar_fifo(Proceso procesos[], int cantidad);
void ejecutar_sjf(Proceso procesos[], int cantidad);
void ejecutar_srt(Proceso procesos[], int cantidad);
void ejecutar_round_robin(Proceso procesos[], int cantidad, int quantum);
void ejecutar_priority(Proceso procesos[], int cantidad);

#endif
// scheduler.h - Fin de la cabecera para algoritmos de planificación