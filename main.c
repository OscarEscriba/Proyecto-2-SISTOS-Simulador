#include <gtk/gtk.h>
#include "./ui/gui.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);    // Inicializa GTK
    
    init_gui();               // Función que construye y muestra la GUI
    
    gtk_main();               // Bucle principal de eventos de GTK
    
    cleanup_gui();            // Limpiar recursos antes de salir
    
    return 0;
}