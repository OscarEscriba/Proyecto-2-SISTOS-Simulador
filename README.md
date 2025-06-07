
- Visualización dinámica de ejecución:
- Diagrama de Gantt generado visualmente
- Scroll vertical disponible
- Métricas mostradas al finalizar:
  - Tiempo promedio de espera
  - Tiempo promedio de retorno

### B. Simulador de Sincronización
- No implementado en esta versión (mutex/semaphore y acciones sobre recursos)

---

## Interfaz Gráfica (GUI)

- Selección entre calendarización y sincronización
- Elección de algoritmos mediante checkboxes
- Campo de entrada para configurar quantum
- Botones para:
- Cargar archivo de procesos
- Iniciar simulación
- Limpiar pantalla
- Área de dibujo del diagrama de ejecución
- Visualización de métricas dinámicas

---

## Formato del archivo de entrada

### Archivo de Procesos (`procesos.txt`)
```txt
P1, 5, 0, 2
P2, 3, 1, 1
P3, 7, 2, 3
