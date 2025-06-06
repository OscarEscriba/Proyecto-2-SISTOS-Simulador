// scheduler/algoritmos.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include "algoritmos.h"

#define MAX_PROCESOS 100

// Round Robin - Corregido para usar la estructura SimStep correcta
void ejecutar_round_robin(Process *procesos, int cantidad, int quantum, SimStep *pasos, int *num_pasos) {
    int tiempo = 0, completados = 0;
    int cola[MAX_PROCESOS];
    int frente = 0, final = 0;
    bool en_cola[cantidad];
    *num_pasos = 0;

    for (int i = 0; i < cantidad; i++) {
        en_cola[i] = false;
        procesos[i].tiempo_restante = procesos[i].burst_time;
        procesos[i].tiempo_inicio = -1;
        procesos[i].ejecutado = 0;
    }

    printf("\n--- Ejecutando algoritmo Round Robin ---\n");
    printf("Diagrama de ejecución (texto):\n");

    while (completados < cantidad) {
        // Añadir procesos que llegaron a la cola
        for (int i = 0; i < cantidad; i++) {
            if (procesos[i].arrival_time <= tiempo && !en_cola[i] && procesos[i].tiempo_restante > 0) {
                cola[final] = i;
                final = (final + 1) % MAX_PROCESOS;
                en_cola[i] = true;
            }
        }

        if (frente == final) {
            // Cola vacía, avanzar tiempo
            tiempo++;
            continue;
        }

        int idx = cola[frente];
        frente = (frente + 1) % MAX_PROCESOS;
        en_cola[idx] = false;

        if (procesos[idx].tiempo_inicio == -1)
            procesos[idx].tiempo_inicio = tiempo;

        int ejecucion = (procesos[idx].tiempo_restante > quantum) ? quantum : procesos[idx].tiempo_restante;

        if (ejecucion > 0) {
            printf("[%d - %d] %s\n", tiempo, tiempo + ejecucion, procesos[idx].pid);
            
            // Agregar pasos a la simulación - uno por cada unidad de tiempo
            for (int t = 0; t < ejecucion; t++) {
                if (*num_pasos < 1000) {
                    pasos[*num_pasos].tiempo_actual = tiempo + t;
                    pasos[*num_pasos].proceso_ejecutando = idx;
                    pasos[*num_pasos].estado_proceso = 1; // 1 = ejecutando
                    (*num_pasos)++;
                }
            }
        }

        procesos[idx].tiempo_restante -= ejecucion;
        tiempo += ejecucion;

        // Verificar nuevos procesos entrantes tras ejecutar
        for (int i = 0; i < cantidad; i++) {
            if (procesos[i].arrival_time <= tiempo && !en_cola[i] && procesos[i].tiempo_restante > 0) {
                cola[final] = i;
                final = (final + 1) % MAX_PROCESOS;
                en_cola[i] = true;
            }
        }

        if (procesos[idx].tiempo_restante > 0) {
            // Reinsertar proceso a la cola
            cola[final] = idx;
            final = (final + 1) % MAX_PROCESOS;
            en_cola[idx] = true;
        } else {
            procesos[idx].tiempo_final = tiempo;
            procesos[idx].tiempo_retorno = procesos[idx].tiempo_final - procesos[idx].arrival_time;
            procesos[idx].tiempo_espera = procesos[idx].tiempo_retorno - procesos[idx].burst_time;
            procesos[idx].ejecutado = 1;
            completados++;
        }
    }

    float total_espera = 0, total_finalizacion = 0;
    for (int i = 0; i < cantidad; i++) {
        total_espera += procesos[i].tiempo_espera;
        total_finalizacion += procesos[i].tiempo_final;
    }

    printf("\nMétricas:\n");
    printf("Tiempo promedio de espera: %.2f\n", total_espera / cantidad);
    printf("Tiempo promedio de finalización: %.2f\n", total_finalizacion / cantidad);
}

// FIFO (First In First Out) - Corregido para usar la estructura SimStep correcta
int comparar_por_arrival_time(const void *a, const void *b) {
    const Process *p1 = (const Process *)a;
    const Process *p2 = (const Process *)b;
    return p1->arrival_time - p2->arrival_time;
}

void ejecutar_fifo(Process *procesos, int cantidad, SimStep *pasos, int *num_pasos) {
    printf("\n--- Ejecutando algoritmo FIFO ---\n");
    *num_pasos = 0;

    Process *procesos_ordenados = malloc(cantidad * sizeof(Process));
    for (int i = 0; i < cantidad; i++) {
        procesos_ordenados[i] = procesos[i];
        procesos_ordenados[i].ejecutado = 0;
    }

    qsort(procesos_ordenados, cantidad, sizeof(Process), comparar_por_arrival_time);

    int tiempo_actual = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    printf("Diagrama de ejecución (texto):\n");

    for (int i = 0; i < cantidad; i++) {
        Process *p = &procesos_ordenados[i];

        if (tiempo_actual < p->arrival_time) {
            tiempo_actual = p->arrival_time;
        }

        int waiting_time = tiempo_actual - p->arrival_time;
        int turnaround_time = waiting_time + p->burst_time;

        total_waiting_time += waiting_time;
        total_turnaround_time += turnaround_time;

        printf("[%d - %d] %s\n", tiempo_actual, tiempo_actual + p->burst_time, p->pid);
        
        // Agregar pasos a la simulación - uno por cada unidad de tiempo
        for (int t = 0; t < p->burst_time; t++) {
            if (*num_pasos < 1000) {
                pasos[*num_pasos].tiempo_actual = tiempo_actual + t;
                pasos[*num_pasos].proceso_ejecutando = i;
                pasos[*num_pasos].estado_proceso = 1; // 1 = ejecutando
                (*num_pasos)++;
            }
        }
        
        tiempo_actual += p->burst_time;
    }

    printf("\nMétricas:\n");
    printf("Tiempo promedio de espera: %.2f\n", (float)total_waiting_time / cantidad);
    printf("Tiempo promedio de finalización: %.2f\n", (float)total_turnaround_time / cantidad);

    free(procesos_ordenados);
}

// Prioridad (No Expropiativo) - Corregido para usar la estructura SimStep correcta
void ejecutar_priority(Process *procesos, int cantidad, SimStep *pasos, int *num_pasos) {
    int tiempo = 0, completados = 0;
    *num_pasos = 0;

    for (int i = 0; i < cantidad; i++) {
        procesos[i].ejecutado = 0;
        procesos[i].tiempo_inicio = -1;
    }

    printf("\n--- Ejecutando algoritmo por Prioridad (No Expropiativo) ---\n");
    printf("Diagrama de ejecución (texto):\n");

    while (completados < cantidad) {
        int idx = -1;
        int mejor_prioridad = INT_MAX;

        for (int i = 0; i < cantidad; i++) {
            if (!procesos[i].ejecutado && procesos[i].arrival_time <= tiempo && procesos[i].priority < mejor_prioridad) {
                mejor_prioridad = procesos[i].priority;
                idx = i;
            }
        }

        if (idx == -1) {
            tiempo++;
            continue;
        }

        procesos[idx].tiempo_inicio = tiempo;
        procesos[idx].tiempo_final = tiempo + procesos[idx].burst_time;
        procesos[idx].tiempo_retorno = procesos[idx].tiempo_final - procesos[idx].arrival_time;
        procesos[idx].tiempo_espera = procesos[idx].tiempo_retorno - procesos[idx].burst_time;
        procesos[idx].ejecutado = 1;

        printf("[%d - %d] %s\n", tiempo, procesos[idx].tiempo_final, procesos[idx].pid);
        
        // Agregar pasos a la simulación - uno por cada unidad de tiempo
        for (int t = 0; t < procesos[idx].burst_time; t++) {
            if (*num_pasos < 1000) {
                pasos[*num_pasos].tiempo_actual = tiempo + t;
                pasos[*num_pasos].proceso_ejecutando = idx;
                pasos[*num_pasos].estado_proceso = 1; // 1 = ejecutando
                (*num_pasos)++;
            }
        }

        tiempo += procesos[idx].burst_time;
        completados++;
    }

    float total_espera = 0, total_finalizacion = 0;
    for (int i = 0; i < cantidad; i++) {
        total_espera += procesos[i].tiempo_espera;
        total_finalizacion += procesos[i].tiempo_final;
    }

    printf("\nMétricas:\n");
    printf("Tiempo promedio de espera: %.2f\n", total_espera / cantidad);
    printf("Tiempo promedio de finalización: %.2f\n", total_finalizacion / cantidad);
}

// SJF (Shortest Job First) - No preemptivo - Corregido para usar la estructura SimStep correcta
void ejecutar_sjf(Process *procesos, int cantidad, SimStep *pasos, int *num_pasos) {
    int tiempo_actual = 0;
    int completados = 0;
    bool ejecutado[cantidad];
    *num_pasos = 0;
    
    for (int i = 0; i < cantidad; i++) ejecutado[i] = false;

    printf("\n--- Ejecutando algoritmo SJF (No Preemptive) ---\n");
    printf("Diagrama de ejecución (texto):\n");

    while (completados < cantidad) {
        int idx_menor_bt = -1;
        int menor_bt = INT_MAX;

        for (int i = 0; i < cantidad; i++) {
            if (!ejecutado[i] && procesos[i].arrival_time <= tiempo_actual && procesos[i].burst_time < menor_bt) {
                menor_bt = procesos[i].burst_time;
                idx_menor_bt = i;
            }
        }

        if (idx_menor_bt == -1) {
            tiempo_actual++;
            continue;
        }

        procesos[idx_menor_bt].tiempo_inicio = tiempo_actual;
        tiempo_actual += procesos[idx_menor_bt].burst_time;
        procesos[idx_menor_bt].tiempo_final = tiempo_actual;

        procesos[idx_menor_bt].tiempo_espera = procesos[idx_menor_bt].tiempo_inicio - procesos[idx_menor_bt].arrival_time;
        procesos[idx_menor_bt].tiempo_retorno = procesos[idx_menor_bt].tiempo_final - procesos[idx_menor_bt].arrival_time;

        ejecutado[idx_menor_bt] = true;
        completados++;

        printf("[%d - %d] %s\n", procesos[idx_menor_bt].tiempo_inicio, procesos[idx_menor_bt].tiempo_final, procesos[idx_menor_bt].pid);
        
        // Agregar pasos a la simulación - uno por cada unidad de tiempo
        for (int t = 0; t < procesos[idx_menor_bt].burst_time; t++) {
            if (*num_pasos < 1000) {
                pasos[*num_pasos].tiempo_actual = procesos[idx_menor_bt].tiempo_inicio + t;
                pasos[*num_pasos].proceso_ejecutando = idx_menor_bt;
                pasos[*num_pasos].estado_proceso = 1; // 1 = ejecutando
                (*num_pasos)++;
            }
        }
    }

    // Calcular métricas promedio
    float total_espera = 0, total_finalizacion = 0;
    for (int i = 0; i < cantidad; i++) {
        total_espera += procesos[i].tiempo_espera;
        total_finalizacion += procesos[i].tiempo_final;
    }

    printf("\nMétricas:\n");
    printf("Tiempo promedio de espera: %.2f\n", total_espera / cantidad);
    printf("Tiempo promedio de finalización: %.2f\n", total_finalizacion / cantidad);
}

// SRTF (Shortest Remaining Time First) - Preemptive SJF - Corregido para usar la estructura SimStep correcta
void ejecutar_srt(Process *procesos, int cantidad, SimStep *pasos, int *num_pasos) {
    int tiempo = 0, completados = 0;
    int tiempo_restante[cantidad];
    *num_pasos = 0;

    // Inicializar tiempos restantes y tiempos de inicio
    for (int i = 0; i < cantidad; i++) {
        tiempo_restante[i] = procesos[i].burst_time;
        procesos[i].tiempo_inicio = -1;
        procesos[i].tiempo_final = 0;
        procesos[i].tiempo_espera = 0;
        procesos[i].tiempo_retorno = 0;
    }

    printf("\n--- Ejecutando algoritmo SRTF (Shortest Remaining Time First) ---\n\n");
    printf("Diagrama de ejecución (texto):\n");

    while (completados < cantidad) {
        int idx_menor = -1;
        int menor_tiempo = INT_MAX;

        // Buscar proceso con menor tiempo restante que haya llegado
        for (int i = 0; i < cantidad; i++) {
            if (procesos[i].arrival_time <= tiempo && tiempo_restante[i] > 0 && tiempo_restante[i] < menor_tiempo) {
                menor_tiempo = tiempo_restante[i];
                idx_menor = i;
            }
        }

        if (idx_menor == -1) {
            // No hay proceso listo, avanzar tiempo
            tiempo++;
            continue;
        }

        // Marcar tiempo de inicio si es la primera ejecución del proceso
        if (procesos[idx_menor].tiempo_inicio == -1) {
            procesos[idx_menor].tiempo_inicio = tiempo;
        }

        // Ejecutar 1 unidad de tiempo
        printf("[%d - %d] %s\n", tiempo, tiempo + 1, procesos[idx_menor].pid);

        // Agregar paso a la simulación
        if (*num_pasos < 1000) {
            pasos[*num_pasos].tiempo_actual = tiempo;
            pasos[*num_pasos].proceso_ejecutando = idx_menor;
            pasos[*num_pasos].estado_proceso = 1; // 1 = ejecutando
            (*num_pasos)++;
        }

        tiempo_restante[idx_menor]--;
        tiempo++;

        // Si el proceso terminó, actualizar métricas
        if (tiempo_restante[idx_menor] == 0) {
            procesos[idx_menor].tiempo_final = tiempo;
            procesos[idx_menor].tiempo_retorno = procesos[idx_menor].tiempo_final - procesos[idx_menor].arrival_time;
            procesos[idx_menor].tiempo_espera = procesos[idx_menor].tiempo_retorno - procesos[idx_menor].burst_time;
            completados++;
        }
    }

    // Calcular y mostrar métricas promedio
    float total_espera = 0, total_finalizacion = 0;
    for (int i = 0; i < cantidad; i++) {
        total_espera += procesos[i].tiempo_espera;
        total_finalizacion += procesos[i].tiempo_final;
    }

    printf("\nMétricas:\n");
    printf("Tiempo promedio de espera: %.2f\n", total_espera / cantidad);
    printf("Tiempo promedio de finalización: %.2f\n", total_finalizacion / cantidad);
}