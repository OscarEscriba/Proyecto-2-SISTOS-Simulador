#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESOS 100
#define MAX_RECURSOS 100
#define MAX_ACCIONES 100

typedef struct {
    char pid[10];
    int tiempo_llegada;
    int duracion;
} Proceso;

typedef struct {
    char nombre[10];
    int contador; // Para semáforo: cantidad disponible; Para mutex será 1 o 0
} Recurso;

typedef struct {
    char pid[10];
    char accion[10];
    char recurso[10];
    int ciclo;
} Accion;

typedef struct {
    char pid[10];
    char recurso[10];
    int ciclo_liberacion;
} RecursoAsignado;

// Datos globales
Proceso procesos[MAX_PROCESOS];
int num_procesos = 0;

Recurso recursos[MAX_RECURSOS];
int num_recursos = 0;

Accion acciones[MAX_ACCIONES];
int num_acciones = 0;

RecursoAsignado recursos_asignados[MAX_ACCIONES];
int num_recursos_asignados = 0;

int ciclo_actual = 0;
int ciclo_maximo = 0;

typedef enum { MUTEX, SEMAFORO } TipoSincronizacion;
TipoSincronizacion tipo_sync;

// Funciones para cargar datos

void cargar_procesos(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error abriendo procesos");
        exit(1);
    }
    while (fscanf(file, "%s %d %d", procesos[num_procesos].pid,
                  &procesos[num_procesos].tiempo_llegada,
                  &procesos[num_procesos].duracion) == 3) {
        num_procesos++;
    }
    fclose(file);
}

void cargar_recursos(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error abriendo recursos");
        exit(1);
    }
    while (fscanf(file, "%s %d", recursos[num_recursos].nombre,
                  &recursos[num_recursos].contador) == 2) {
        num_recursos++;
    }
    fclose(file);
}

void cargar_acciones(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error abriendo acciones");
        exit(1);
    }
    while (fscanf(file, "%s %s %s %d", acciones[num_acciones].pid,
                  acciones[num_acciones].accion,
                  acciones[num_acciones].recurso,
                  &acciones[num_acciones].ciclo) == 4) {
        if (acciones[num_acciones].ciclo > ciclo_maximo)
            ciclo_maximo = acciones[num_acciones].ciclo;
        num_acciones++;
    }
    fclose(file);
}

int get_recurso_index(const char *nombre) {
    for (int i = 0; i < num_recursos; i++) {
        if (strcmp(recursos[i].nombre, nombre) == 0) return i;
    }
    return -1;
}

int recurso_ocupado(const char *recurso) {
    for (int i = 0; i < num_recursos_asignados; i++) {
        if (strcmp(recursos_asignados[i].recurso, recurso) == 0)
            return 1; // ocupado
    }
    return 0;
}

void liberar_recursos_ciclo(int ciclo) {
    for (int i = 0; i < num_recursos_asignados;) {
        if (recursos_asignados[i].ciclo_liberacion == ciclo) {
            int idx = get_recurso_index(recursos_asignados[i].recurso);
            if (idx != -1) {
                if (tipo_sync == SEMAFORO) {
                    recursos[idx].contador++;
                } else if (tipo_sync == MUTEX) {
                    recursos[idx].contador = 1; // liberar mutex
                }
            }
            // Eliminar recurso asignado desplazando el array
            for (int j = i; j < num_recursos_asignados - 1; j++)
                recursos_asignados[j] = recursos_asignados[j + 1];
            num_recursos_asignados--;
        } else {
            i++;
        }
    }
}

void asignar_recurso(const char *pid, const char *recurso, int ciclo) {
    recursos_asignados[num_recursos_asignados].ciclo_liberacion = ciclo + 2; // se libera 2 ciclos después
    strcpy(recursos_asignados[num_recursos_asignados].pid, pid);
    strcpy(recursos_asignados[num_recursos_asignados].recurso, recurso);
    num_recursos_asignados++;
}

void simular() {
    printf("\nSimulando sincronizacion con %s\n\n",
           tipo_sync == MUTEX ? "MUTEX" : "SEMAFOROS");

    for (ciclo_actual = 0; ciclo_actual <= ciclo_maximo + 2; ciclo_actual++) {
        printf("Ciclo %d:\n", ciclo_actual);

        // Liberar recursos si es necesario
        liberar_recursos_ciclo(ciclo_actual);

        for (int i = 0; i < num_procesos; i++) {
            // Mostrar solo procesos que han llegado
            if (procesos[i].tiempo_llegada > ciclo_actual) {
                printf("  Proceso %s: Aún no llega\n", procesos[i].pid);
                continue;
            }
            // Buscar acción para este proceso en este ciclo
            int accion_idx = -1;
            for (int j = 0; j < num_acciones; j++) {
                if (acciones[j].ciclo == ciclo_actual && strcmp(acciones[j].pid, procesos[i].pid) == 0) {
                    accion_idx = j;
                    break;
                }
            }
            if (accion_idx == -1) {
                printf("  Proceso %s: No realiza acción\n", procesos[i].pid);
                continue;
            }

            int idx_recurso = get_recurso_index(acciones[accion_idx].recurso);
            if (idx_recurso == -1) {
                printf("  Proceso %s: Recurso %s no encontrado\n", procesos[i].pid, acciones[accion_idx].recurso);
                continue;
            }

            // Intentar tomar recurso según sincronización
            int puede_tomar = 0;

            if (tipo_sync == MUTEX) {
                // Mutex: recurso disponible si contador == 1 y no está ocupado
                if (recursos[idx_recurso].contador == 1 && !recurso_ocupado(recursos[idx_recurso].nombre)) {
                    puede_tomar = 1;
                    recursos[idx_recurso].contador = 0;
                }
            } else {
                // Semáforo: recurso disponible si contador > 0
                if (recursos[idx_recurso].contador > 0) {
                    puede_tomar = 1;
                    recursos[idx_recurso].contador--;
                }
            }

            if (puede_tomar) {
                asignar_recurso(procesos[i].pid, recursos[idx_recurso].nombre, ciclo_actual);
                printf("  Proceso %s: Ejecuta acción '%s' en recurso '%s' (RECURSO ASIGNADO)\n",
                       procesos[i].pid, acciones[accion_idx].accion, recursos[idx_recurso].nombre);
            } else {
                printf("  Proceso %s: Esperando recurso '%s' (EN ESPERA)\n",
                       procesos[i].pid, recursos[idx_recurso].nombre);
            }
        }
        printf("\n");
    }
}

int main() {
    printf("Simulador de sincronizacion (MUTEX o SEMAFOROS)\n\n");

    cargar_procesos("procesos.txt");
    cargar_recursos("recursos.txt");
    cargar_acciones("acciones.txt");

    printf("Seleccione tipo de sincronizacion:\n");
    printf("1. Mutex Locks\n");
    printf("2. Semaforos\n");
    printf("Ingrese opcion (1 o 2): ");

    int opcion = 0;
    scanf("%d", &opcion);

    if (opcion == 1)
        tipo_sync = MUTEX;
    else if (opcion == 2)
        tipo_sync = SEMAFORO;
    else {
        printf("Opcion no valida. Terminando programa.\n");
        return 1;
    }

    simular();

    return 0;
}
