#ifndef DIBUJAR_H
#define DIBUJAR_H

#include <gtk/gtk.h>
#include "../simulation/simulacion.h"

// Inicializa datos para dibujo
void dibujar_inicializar(GtkWidget *drawing_area);

// Función que dibuja un paso de simulación
void dibujar_sim_step(cairo_t *cr, const SimStep *step, int ancho, int alto);

// Actualiza la visualización con nuevos pasos
void dibujar_actualizar(GtkWidget *drawing_area, SimResult *result);

// ✅ Agrega esta declaración para que gui.c lo vea
gboolean on_draw_callback(GtkWidget *widget, cairo_t *cr, gpointer user_data);

#endif
