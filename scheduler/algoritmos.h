// scheduler/algoritmos.h
#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "../utils/process.h"
#include "../simulation/simulacion.h" // Para SimStep

void ejecutar_fifo(Process *procesos, int cantidad, SimStep *pasos, int *num_pasos);
void ejecutar_sjf(Process *procesos, int cantidad, SimStep *pasos, int *num_pasos);
void ejecutar_srt(Process *procesos, int cantidad, SimStep *pasos, int *num_pasos);
void ejecutar_round_robin(Process *procesos, int cantidad, int quantum, SimStep *pasos, int *num_pasos);
void ejecutar_priority(Process *procesos, int cantidad, SimStep *pasos, int *num_pasos);


// Agregaremos aquí los demás: SJF, SRTF, Prioridad, etc.

#endif
