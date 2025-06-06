// utils/process.h

#ifndef PROCESS_H
#define PROCESS_H

typedef struct {
    char pid[10];
    int burst_time;
    int arrival_time;
    int priority;

    // Campos adicionales para algoritmos
    int tiempo_restante;
    int tiempo_inicio;
    int tiempo_final;
    int tiempo_espera;
    int tiempo_retorno;
    int ejecutado; // 0 = no, 1 = sí. indica si el proceso ha sido ejecutado 
} Process;

int cargar_procesos_desde_archivo(const char *ruta, Process **procesos, int *cantidad);

#endif
