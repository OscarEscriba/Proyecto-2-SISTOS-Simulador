#include "dibujar.h"
#include <cairo.h>
#include <string.h>

static SimResult *sim_result_global = NULL;
static GtkWidget *drawing_area_global = NULL;

void dibujar_inicializar(GtkWidget *drawing_area) {
    drawing_area_global = drawing_area;
    sim_result_global = NULL;
}

// Dibuja un cuadro representando un proceso en ejecución en la línea de tiempo
void dibujar_sim_step(cairo_t *cr, const SimStep *step, int ancho, int alto) {
    // Dibuja rectángulo para el proceso ejecutándose
    int box_width = 50;
    int x = step->tiempo_actual * box_width;
    int y = alto / 2 - 20;

    // Cambiar color según estado
    if (step->estado_proceso == 1) { // ejecutando
        cairo_set_source_rgb(cr, 0.1, 0.7, 0.1);
    } else if (step->estado_proceso == 2) { // terminado
        cairo_set_source_rgb(cr, 0.7, 0.1, 0.1);
    } else { // listo o espera
        cairo_set_source_rgb(cr, 0.1, 0.1, 0.7);
    }

    cairo_rectangle(cr, x, y, box_width - 5, 40);
    cairo_fill(cr);

    // Texto PID o índice del proceso
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_move_to(cr, x + 5, y + 25);
    char text[32];
    snprintf(text, sizeof(text), "P%d", step->proceso_ejecutando);
    cairo_show_text(cr, text);
}

// Callback de dibujo para GTK
gboolean on_draw_callback(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    if (!sim_result_global) {
        // Mensaje inicial
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_move_to(cr, 10, 20);
        cairo_show_text(cr, "Línea de tiempo - Inicia simulación");
        return FALSE;
    }

    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);
    int width = allocation.width;
    int height = allocation.height;

    // Fondo blanco
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    // Dibuja todos los pasos
    for (int i = 0; i < sim_result_global->total_steps; i++) {
        dibujar_sim_step(cr, &sim_result_global->steps[i], width, height);
    }

    return FALSE;
}

void dibujar_actualizar(GtkWidget *drawing_area, SimResult *result) {
    if (sim_result_global) {
        liberar_sim_result(sim_result_global);
    }
    sim_result_global = result;

    // Forzar redibujo
    gtk_widget_queue_draw(drawing_area);
}
