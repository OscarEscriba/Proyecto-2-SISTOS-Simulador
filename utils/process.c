// utils/process.c

#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cargar_procesos_desde_archivo(const char *ruta, Process **procesos, int *cantidad) {
    FILE *archivo = fopen(ruta, "r");
    if (!archivo) {
        perror("Error al abrir el archivo de procesos");
        return -1;
    }

    int capacidad = 10;
    *procesos = malloc(sizeof(Process) * capacidad);
    *cantidad = 0;

    char linea[100];
    while (fgets(linea, sizeof(linea), archivo)) {
        if (*cantidad >= capacidad) {
            capacidad *= 2;
            *procesos = realloc(*procesos, sizeof(Process) * capacidad);
        }

        char pid[10];
        int bt, at, prio;
        if (sscanf(linea, "%[^,], %d, %d, %d", pid, &bt, &at, &prio) == 4) {
            strcpy((*procesos)[*cantidad].pid, pid);
            (*procesos)[*cantidad].burst_time = bt;
            (*procesos)[*cantidad].arrival_time = at;
            (*procesos)[*cantidad].priority = prio;
            (*cantidad)++;
        }
    }

    fclose(archivo);
    return 0;
}
