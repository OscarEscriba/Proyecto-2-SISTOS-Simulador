#ifndef SIMULACION_H
#define SIMULACION_H

#include "../utils/process.h"

typedef enum {
    SIM_TYPE_CALENDARIZACION,
    SIM_TYPE_SINCRONIZACION
} SimulationType;

typedef enum {
    ALG_FIFO = 1 << 0,
    ALG_SJF = 1 << 1,
    ALG_SRTF = 1 << 2,
    ALG_RR = 1 << 3,
    ALG_PRIORITY = 1 << 4,
} AlgorithmFlags;

typedef struct {
    char algoritmo[20];   // Nombre del algoritmo: "FIFO", "SJF", etc.
    int quantum;              // Para RR
    int selected_algorithms;  // Bitmask con algoritmos seleccionados
    SimulationType sim_type;
} SimConfig;

typedef struct {
    // Para cada paso de simulación podemos guardar datos a dibujar
    int tiempo_actual;
    int proceso_ejecutando;
    int estado_proceso; // por ejemplo 0 = listo, 1 = ejecutando, 2 = terminado
} SimStep;

typedef struct {
    int total_steps;
    SimStep *steps;      // arreglo dinámico de pasos
    float promedio_espera;
    float promedio_retorno;
} SimResult;

// Cargar procesos desde archivo (ya lo tienes en utils/process.h)
int cargar_procesos_desde_archivo(const char *ruta, Process **procesos, int *cantidad);

// Ejecutar la simulación y devolver resultados
SimResult* ejecutar_simulacion(const Process *procesos, int cantidad, SimConfig config);

// Liberar resultados
void liberar_sim_result(SimResult *result);

// Calcular y obtener métricas, retorna un string con información (memoria interna, no liberar)
const char* obtener_metricas(const Process *procesos, int cantidad);

#endif
