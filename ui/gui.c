#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../utils/process.h"
#include "../simulation/simulacion.h"
#include "dibujar.h"

// Widgets globales
GtkWidget *window;
GtkWidget *algoritmo_rr_checkbox;
GtkWidget *quantum_entry;
GtkWidget *start_button;
GtkWidget *drawing_area;
GtkWidget *metrics_label;

GtkWidget *radio_calendarizacion;
GtkWidget *radio_sincronizacion;

GtkWidget *fifo_checkbox;
GtkWidget *sjf_checkbox;
GtkWidget *srtf_checkbox;
GtkWidget *priority_checkbox;

// Variables globales para procesos
Process *lista_procesos = NULL;
int cantidad_procesos = 0;
char ruta_archivo[256] = "";

// Variables para simulación
SimResult *resultado_simulacion = NULL;

// Controla habilitación quantum para RR
void on_algoritmo_rr_toggled(GtkToggleButton *togglebutton, gpointer user_data) {
    printf("DEBUG: on_algoritmo_rr_toggled called\n");
    gboolean active = gtk_toggle_button_get_active(togglebutton);
    gtk_widget_set_sensitive(quantum_entry, active);
}

// Carga procesos y ejecuta simulación
void on_start_button_clicked(GtkButton *button, gpointer user_data) {
    printf("DEBUG: on_start_button_clicked called\n");
    gtk_label_set_text(GTK_LABEL(metrics_label), "Cargando procesos...");

    if (strlen(ruta_archivo) == 0) {
        gtk_label_set_text(GTK_LABEL(metrics_label), "Primero seleccione un archivo.");
        return;
    }
    
    printf("DEBUG: Trying to load processes from: %s\n", ruta_archivo);
    if (cargar_procesos_desde_archivo(ruta_archivo, &lista_procesos, &cantidad_procesos) != 0) {
        gtk_label_set_text(GTK_LABEL(metrics_label), "Error cargando procesos.");
        return;
    }

    // Obtener configuración de simulación
    SimConfig config = {0};
    config.sim_type = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_calendarizacion)) ? SIM_TYPE_CALENDARIZACION : SIM_TYPE_SINCRONIZACION;

    config.selected_algorithms = 0;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fifo_checkbox))) config.selected_algorithms |= ALG_FIFO;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sjf_checkbox))) config.selected_algorithms |= ALG_SJF;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(srtf_checkbox))) config.selected_algorithms |= ALG_SRTF;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(algoritmo_rr_checkbox))) config.selected_algorithms |= ALG_RR;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priority_checkbox))) config.selected_algorithms |= ALG_PRIORITY;

    if (config.selected_algorithms == 0 && config.sim_type == SIM_TYPE_CALENDARIZACION) {
        gtk_label_set_text(GTK_LABEL(metrics_label), "Seleccione al menos un algoritmo.");
        return;
    }

    const char *quantum_text = gtk_entry_get_text(GTK_ENTRY(quantum_entry));
    config.quantum = atoi(quantum_text);
    if (config.quantum <= 0 && (config.selected_algorithms & ALG_RR)) {
        gtk_label_set_text(GTK_LABEL(metrics_label), "Ingrese quantum válido para Round Robin.");
        return;
    }

    // Ejecutar simulación
    printf("DEBUG: About to execute simulation\n");
    if (resultado_simulacion) {
        liberar_sim_result(resultado_simulacion);
        resultado_simulacion = NULL;
    }
    resultado_simulacion = ejecutar_simulacion(lista_procesos, cantidad_procesos, config);

    // Actualizar dibujo
    printf("DEBUG: About to update drawing\n");
    if (drawing_area) {
        dibujar_actualizar(drawing_area, resultado_simulacion);
    }

    // Mostrar métricas
    printf("DEBUG: About to show metrics\n");
    if (resultado_simulacion) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer),
                "Promedio de espera: %.2f\nPromedio de retorno: %.2f",
                resultado_simulacion->promedio_espera,
                resultado_simulacion->promedio_retorno);
        gtk_label_set_text(GTK_LABEL(metrics_label), buffer);
    }
}

// Selección archivo
void on_seleccionar_archivo_clicked(GtkButton *button, gpointer user_data) {
    printf("DEBUG: on_seleccionar_archivo_clicked called\n");
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Seleccionar archivo de procesos",
        GTK_WINDOW(window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancelar", GTK_RESPONSE_CANCEL,
        "_Abrir", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        strncpy(ruta_archivo, filename, sizeof(ruta_archivo) - 1);
        ruta_archivo[sizeof(ruta_archivo) - 1] = '\0';
        g_free(filename);

        printf("Archivo seleccionado: %s\n", ruta_archivo);
        gtk_label_set_text(GTK_LABEL(metrics_label), "Archivo seleccionado.");
    }
    gtk_widget_destroy(dialog);
}

void on_limpiar_clicked(GtkButton *button, gpointer user_data) {
    printf("DEBUG: on_limpiar_clicked called\n");
    // Limpiar simulación y procesos
    if (lista_procesos) {
        free(lista_procesos);
        lista_procesos = NULL;
        cantidad_procesos = 0;
    }
    if (resultado_simulacion) {
        liberar_sim_result(resultado_simulacion);
        resultado_simulacion = NULL;
    }
    ruta_archivo[0] = '\0';

    gtk_label_set_text(GTK_LABEL(metrics_label), "Simulación reiniciada.");
    if (drawing_area) {
        dibujar_actualizar(drawing_area, NULL);
    }
}


// Esta función reemplaza al main() que estaba aquí
void init_gui() {
    printf("DEBUG: init_gui() started\n");
    
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    printf("DEBUG: Window created\n");
    
    gtk_window_set_title(GTK_WINDOW(window), "Simulador de Planificación y Sincronización");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    printf("DEBUG: Window configured\n");

    // Contenedor vertical principal
    GtkWidget *vbox_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox_main);
    printf("DEBUG: Main container created\n");

    // HBox para selección tipo simulación
    GtkWidget *hbox_sim_type = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox_main), hbox_sim_type, FALSE, FALSE, 5);

    radio_calendarizacion = gtk_radio_button_new_with_label(NULL, "Calendarización");
    gtk_box_pack_start(GTK_BOX(hbox_sim_type), radio_calendarizacion, FALSE, FALSE, 5);

    radio_sincronizacion = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_calendarizacion), "Sincronización");
    gtk_box_pack_start(GTK_BOX(hbox_sim_type), radio_sincronizacion, FALSE, FALSE, 5);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_calendarizacion), TRUE);
    printf("DEBUG: Radio buttons created\n");

    // Contenedor para selección algoritmos
    GtkWidget *frame_alg = gtk_frame_new("Algoritmos");
    gtk_box_pack_start(GTK_BOX(vbox_main), frame_alg, FALSE, FALSE, 5);
    GtkWidget *alg_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(frame_alg), alg_box);

    fifo_checkbox = gtk_check_button_new_with_label("FIFO");
    gtk_box_pack_start(GTK_BOX(alg_box), fifo_checkbox, FALSE, FALSE, 5);

    sjf_checkbox = gtk_check_button_new_with_label("SJF");
    gtk_box_pack_start(GTK_BOX(alg_box), sjf_checkbox, FALSE, FALSE, 5);

    srtf_checkbox = gtk_check_button_new_with_label("SRTF");
    gtk_box_pack_start(GTK_BOX(alg_box), srtf_checkbox, FALSE, FALSE, 5);

    algoritmo_rr_checkbox = gtk_check_button_new_with_label("Round Robin");
    gtk_box_pack_start(GTK_BOX(alg_box), algoritmo_rr_checkbox, FALSE, FALSE, 5);
    g_signal_connect(algoritmo_rr_checkbox, "toggled", G_CALLBACK(on_algoritmo_rr_toggled), NULL);

    priority_checkbox = gtk_check_button_new_with_label("Prioridad");
    gtk_box_pack_start(GTK_BOX(alg_box), priority_checkbox, FALSE, FALSE, 5);
    printf("DEBUG: Algorithm controls created\n");

    quantum_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(quantum_entry), "Quantum");
    gtk_widget_set_sensitive(quantum_entry, FALSE);
    gtk_box_pack_start(GTK_BOX(alg_box), quantum_entry, FALSE, FALSE, 5);


    // Botones acción
    GtkWidget *hbox_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox_main), hbox_buttons, FALSE, FALSE, 5);

    GtkWidget *seleccionar_archivo_btn = gtk_button_new_with_label("Seleccionar Archivo");
    gtk_box_pack_start(GTK_BOX(hbox_buttons), seleccionar_archivo_btn, FALSE, FALSE, 5);
    g_signal_connect(seleccionar_archivo_btn, "clicked", G_CALLBACK(on_seleccionar_archivo_clicked), NULL);

    start_button = gtk_button_new_with_label("Iniciar Simulación");
    gtk_box_pack_start(GTK_BOX(hbox_buttons), start_button, FALSE, FALSE, 5);
    g_signal_connect(start_button, "clicked", G_CALLBACK(on_start_button_clicked), NULL);

    GtkWidget *limpiar_btn = gtk_button_new_with_label("Limpiar");
    gtk_box_pack_start(GTK_BOX(hbox_buttons), limpiar_btn, FALSE, FALSE, 5);
    g_signal_connect(limpiar_btn, "clicked", G_CALLBACK(on_limpiar_clicked), NULL);
    printf("DEBUG: Buttons created\n");

    // Área de dibujo
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 1000, 500);
    gtk_box_pack_start(GTK_BOX(vbox_main), drawing_area, TRUE, TRUE, 5);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw_callback), NULL);
    dibujar_inicializar(drawing_area);
    printf("DEBUG: Drawing area created\n");

    // etiqueta de métricas
    metrics_label = gtk_label_new("Métricas: ");
    gtk_box_pack_start(GTK_BOX(vbox_main), metrics_label, FALSE, FALSE, 5);
    printf("DEBUG: Metrics label added\n");

    // Label para métricas
    metrics_label = gtk_label_new("Seleccione archivo y configure simulación.");
    gtk_box_pack_start(GTK_BOX(vbox_main), metrics_label, FALSE, FALSE, 5);
    printf("DEBUG: Metrics label created\n");

    gtk_widget_show_all(window);
    printf("DEBUG: init_gui() completed successfully\n");
    gtk_main();
    printf("DEBUG: GTK main loop exited\n");    
}

// Función para limpiar recursos al cerrar
void cleanup_gui() {
    printf("DEBUG: cleanup_gui() called\n");
    if (lista_procesos) {
        free(lista_procesos);
        lista_procesos = NULL;
    }
    if (resultado_simulacion) {
        liberar_sim_result(resultado_simulacion);
        resultado_simulacion = NULL;
    }
    printf("DEBUG: cleanup_gui() completed\n");
}