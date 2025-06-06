#include "simulacion.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../scheduler/algoritmos.h"
#include "../utils/process.h"

SimResult* ejecutar_simulacion(const Process *procesos_original, int cantidad, SimConfig config) {
    // Clonamos procesos para no alterar los originales
    Process *procesos = malloc(sizeof(Process) * cantidad);
    if (!procesos) {
        printf("Error: No se pudo asignar memoria para procesos\n");
        return NULL;
    }
    memcpy(procesos, procesos_original, sizeof(Process) * cantidad);

    // Reservamos espacio dinámico para los pasos
    SimStep *pasos = malloc(sizeof(SimStep) * 1000);
    if (!pasos) {
        printf("Error: No se pudo asignar memoria para pasos\n");
        free(procesos);
        return NULL;
    }
    int num_pasos = 0;

    // Lógica de selección de algoritmo basada en flags
    int executed = 0;
    
    if (config.selected_algorithms & ALG_FIFO) {
        ejecutar_fifo(procesos, cantidad, pasos, &num_pasos);
        executed = 1;
    } else if (config.selected_algorithms & ALG_SJF) {
        ejecutar_sjf(procesos, cantidad, pasos, &num_pasos);
        executed = 1;
    } else if (config.selected_algorithms & ALG_SRTF) {
        ejecutar_srt(procesos, cantidad, pasos, &num_pasos);
        executed = 1;
    } else if (config.selected_algorithms & ALG_RR) {
        ejecutar_round_robin(procesos, cantidad, config.quantum, pasos, &num_pasos);
        executed = 1;
    } else if (config.selected_algorithms & ALG_PRIORITY) {
        ejecutar_priority(procesos, cantidad, pasos, &num_pasos);
        executed = 1;
    }
    
    if (!executed) {
        printf("Error: No se seleccionó ningún algoritmo válido\n");
        free(pasos);
        free(procesos);
        return NULL;
    }

    // Empaquetamos los resultados
    SimResult *result = malloc(sizeof(SimResult));
    if (!result) {
        printf("Error: No se pudo asignar memoria para resultado\n");
        free(pasos);
        free(procesos);
        return NULL;
    }
    
    result->total_steps = num_pasos;
    result->steps = malloc(sizeof(SimStep) * num_pasos);
    if (!result->steps) {
        printf("Error: No se pudo asignar memoria para steps del resultado\n");
        free(result);
        free(pasos);
        free(procesos);
        return NULL;
    }
    
    memcpy(result->steps, pasos, sizeof(SimStep) * num_pasos);

    // Liberar memoria temporal
    free(pasos);
    free(procesos);

    // Calcular métricas
        int total_espera = 0;
        int total_retorno = 0;

        for (int i = 0; i < cantidad; i++) {
            total_espera += procesos[i].tiempo_espera;
            total_retorno += procesos[i].tiempo_retorno;
        }

        result->promedio_espera = cantidad > 0 ? (float)total_espera / cantidad : 0.0;
        result->promedio_retorno = cantidad > 0 ? (float)total_retorno / cantidad : 0.0;

    return result;
}

void liberar_sim_result(SimResult *result) {
    if (result) {
        if (result->steps) {
            free(result->steps);
        }
        free(result);
    }
}

const char* obtener_metricas(const Process *procesos, int cantidad) {
    static char buffer[256];
    int tiempo_total = 0;

    for (int i = 0; i < cantidad; i++) {
        tiempo_total += procesos[i].tiempo_retorno; // o tiempo_final - tiempo_llegada
    }

    snprintf(buffer, sizeof(buffer), 
        "Promedio de retorno: %.2f", 
        cantidad > 0 ? (float)tiempo_total / cantidad : 0.0);
    
    return buffer;
}