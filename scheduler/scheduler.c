// scheduler/scheduler.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/process.h"
#include "../scheduler/fifo.h" 
#include "../scheduler/sjf.h" // Incluye el algoritmo SJF
#include "../scheduler/srtf.h"
#include "../scheduler/round_robin.h"
#include "../scheduler/priority.h" 

// Aquí podrías incluir más algoritmos luego

void mostrar_procesos(Process *procesos, int cantidad) {
    printf("Procesos cargados:\n");
    printf("PID\tBT\tAT\tPriority\n");
    for (int i = 0; i < cantidad; i++) {
        printf("%s\t%d\t%d\t%d\n",
               procesos[i].pid,
               procesos[i].burst_time,
               procesos[i].arrival_time,
               procesos[i].priority);
    }
}

void ejecutar_algoritmo(const char *algoritmo, Process *procesos, int cantidad, int quantum) {
    if (strcmp(algoritmo, "FIFO") == 0) {
        ejecutar_fifo(procesos, cantidad);
    } else if (strcmp(algoritmo, "SJF") == 0) {
        ejecutar_sjf(procesos, cantidad);
    } else if (strcmp(algoritmo, "SRT") == 0) {
        ejecutar_srt(procesos, cantidad);
    } else if (strcmp(algoritmo, "RR") == 0) {
        if (quantum <= 0) {
            printf("Por favor indica un quantum válido para Round Robin.\n");
            return;
        }
        ejecutar_round_robin(procesos, cantidad, quantum);
    } else if (strcmp(algoritmo, "PRIORITY") == 0) {
        ejecutar_priority(procesos, cantidad);
    } else {
        printf("[Simulación %s aún no implementada]\n", algoritmo);
    }
}

int main_simulacion(const char *archivo_procesos, const char *algoritmo, int quantum){
    Process *procesos = NULL;
    int cantidad = 0;

    if (cargar_procesos_desde_archivo(archivo_procesos, &procesos, &cantidad) != 0) {
        fprintf(stderr, "Error cargando procesos desde archivo.\n");
        return 1;
    }

    if (cantidad == 0) {
        printf("No se encontraron procesos en el archivo.\n");
        free(procesos);
        return 1;
    }

    mostrar_procesos(procesos, cantidad);

    printf("\n--- Ejecutando algoritmo %s ---\n", algoritmo);
    ejecutar_algoritmo(algoritmo, procesos, cantidad, quantum);
    free(procesos);
    return 0;
}
