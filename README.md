# T02-SSOO

Modelación del problema:

- Structs:
```
typedef struct procesos
{
int PID;
char *name;
int prioridad; (lo mismo que deadline?)
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
int tiempo_cambio_burst; (es la suma del tiempo en que comienza el burst actual + lo que demora -> el instante de tiempo en que termina el burst actual)
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
      separar la línea por espacios. Cada vez crear un Process con los datos obtenidos y agregarlo al arreglo_procesos. Guardar 
      en una variable la cantidad de procesos que hay.
      
Ahora tenemos guardados todos los datos en sus estructuras. Desde aquí empieza un loop de 255 vueltas (se puede asumir que es máxima cantidad ráfagas CPU).
En cada vuelta se debe manejar cómo se van metiendo y sacando procesos a la CPU, cómo se le cambian los estados, cómo se ve cuál tiene más prioridad, ir
actualizando los distintos tiempos o atributos de cada proceso, etc.

4. Loop:

```-
for( i = 0; i < 255; i++){
    - Verificar en CPU si hay un proceso running y si debería cambiar su estado o no.
    - Verificar en arreglo_procesos si hay que cambiar algún estado (WAITING a READY). Se ve con tiempo_cambio_burst. Si nos encontramos 
      en ese instante de tiempo, se cambia a WAITING o READY según corresponda y se calcula el nevo tiempo_cambio_burst con la info del 
      burst siguiente.
    - Buscar en el arreglo de procesos, el más prioritario de los procesos según el deadline y que esté en estado ready. 
    - Ver si el proceso escogido ya debería haber empezado (t_actual >= t_inicio de proceso).
    - Comparar deadline del proceso con el que está corriendo en la CPU.
    - Si es más prioritario, cambiar el estado de RUNNING a WAITING, cambiar el estado del proceso prioritario de WAITING a RUNNING
      y agregarlo a la CPU.
    
      
   }
```
 
5. Escribir en archivo:
 

```
FILE *file fopen(NOMBRE_OUTPUT, "w");

for( i = 0; i < CANTIDAD_PROCESOS; i++){
    Process proceso = arreglo_procesos[i];
    fprint(file, "%s,%d,%d,%d,%d,%d,%d\n", proceso.name, proceso.turnos_cpu, proceso.interrupciones, proceso.turnaround_time, proceso.response_time, proceso.waiting_time,                  proceso.deadline);
}

fclose(file)
```
   
