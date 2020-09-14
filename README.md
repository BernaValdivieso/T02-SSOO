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
int arreglo_burst[255];
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
Process arreglo_procesos[255];
}Queue;
```


```
typedef struct cpu
{
Process proceso_running;
}CPU;
```
```
CPU arreglo_cpu[n];
*** Tenemos un CPU que es un arreglo de procesos running. Como cada CPU puede correr un proceso a la vez, se tendrá que
    que el arreglo va a ser de largo uno. Tenemos también el arreglo_cpu, que es un que contiene cada CPU. El largo del 
    arreglo va a depender del input n. Por ejemplo, si nos piden dos CPUs, tendremos un arreglo de tamaño 2. Ese arreglo
    va a tener dos CPUs, y cada CPU es un arreglo que contiene a UN proceso en estado running.
```
- Pasos a seguir:

1. Recibir inputs:
    - Manejar de alguna manera para tener guardado el nombre del archivo input, el nombre del archivo output y la cantidad de CPUs.
    
2. Crear estructuras bases:
    - Creamos la cantidad de CPUs que nos piden y las guardamos en el arreglo_cpu que también creamos. Creamos el arreglo_procesos.
    
3. Leer archivo:
    - Segun el nombre de input que tenemos, leemos el archivo. Si se lee línea por línea, habrá que hacer un loop y en cada iteración
      separar la línea por espacios. Cada vez crear un Process con los datos obtenidos y agregarlo al arreglo_procesos. 
      
Ahora tenemos guardados todos los datos en sus estructuras. Desde aquí empieza un loop de 255 vueltas (se puede asumir que es máxima cantidad ráfagas CPU).
En cada vuelta se debe manejar cómo se van metiendo y sacando procesos a la CPU, cómo se le cambian los estados, cómo se ve cuál tiene más prioridad, ir
actualizando los distintos tiempos o atributos de cada proceso, etc.

4. Loop:

```
for( i = 0; i <= 255; i++){
      
   }
```
 
    





