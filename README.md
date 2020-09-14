# T02-SSOO

Modelación del problema:

- Structs:
```
typedef struct procesos
{
int PID;
char *name;
int prioridad;
Estado estado;
int turnos_cpu;
int interrupciones;
int turnaround_time;
int response_time;
int waiting_time;
int finished;
int t_inicio;
int deadline;
int arreglo_burst;
}Process;
```
El estado es un int, porque podría ser más facil compararlo con ontros estados si lo usamos así como dijo el ayudante.
Para eso usamos esto:

```
typedef enum estados
{
RUNNING,
READY,
WAITING,
FINISHED,
}Estado;
```
La cola:

```
typedef struct cola
{
*** alguna forma de hacer la cola (un arreglo para todos los procesos, uno para cada uno o uno solo para los ready, etc.) ***
}Queue;
```


- Pasos a seguir:

1. Leer archivo:
  - Si se lee línea por línea, habrá que hacer un loop y en cada iteración separar la línea por espacios y guardarlo en el struct correspondiente.
    
