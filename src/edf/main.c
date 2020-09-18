#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "args/args.h"


typedef enum estados
{
    RUNNING,
    READY,
    WAITING,
    FINISHED,
}Estado;


typedef struct procesos
{
    int PID;
    char name[32];
    Estado estado;
    int turnos_cpu;
    int interrupciones;
    int turnaround_time;
    int primera_ejecucion; /**(0 si no ha entrado en la CPU, 1 si ya entró alguna vez)*/
    int response_time;
    int waiting_time;
    int finished;
    int t_inicio;
    int deadline;
    int cantidad_burst;
    int burst_actual;
    int tipo_burst; //0 = CPU, 1 = I/O
    int arreglo_burst[255];
    int tiempo_cambio_burst; /**(es la suma del tiempo en que comienza el burst actual + lo que demora -> el instante de tiempo en que termina el burst actual)*/
    int size_arreglo_burst;
}Process;


typedef struct cola
{
    Process* arreglo_procesos[255];
}Queue;


typedef struct cpu
{
    Process* proceso_running[1];
}CPU;







int main(int argc, char**argv)
{
/**Aquí se leen los argumentos de consola */    
    int  n_arg = argc - 1;
    char * inp = argv[1];
    char * out = argv[2];
    int  n_nucleos;

    if (n_arg == 0) 
    {
        printf(">>ERROR: No has ingresado argumentos\n");
        exit(0);
    }
    else if (n_arg == 1) 
    {
        printf(">>ERROR: Falta ingresar archivo output\n");
        exit(0);
    }
    else if (n_arg == 3) 
    {
        n_nucleos = atoi(argv[3]);
        if (n_nucleos == 0)
        {
            printf(">>ERROR: Ingresar cantidad válida de núcleos\n");
            exit(0);
        }
    }
    else 
    {
        n_nucleos = 1;
    }

    printf("Pusiste %d argumentos \n", argc - 1);
    printf("input : %s\n",inp);
    printf("output : %s\n",out);
    printf("n_nucleos : %d\n", n_nucleos);
    
/**Creamos nuestro arreglo que contiene a las distintos CPUs y a la cola con TODOS los procesos*/
    CPU* arreglo_cpu[n_nucleos];
    Queue cola;
    

/**Creamos la cantidad de CPUs que nos piden y lo guardamos en el arreglo */
    for (int i = 0; i < n_nucleos; ++i)
    {
        CPU* cpu = calloc(1, sizeof(CPU));
        cpu->proceso_running[0] = NULL;
        arreglo_cpu[i] = cpu; 
    }

/**Abrimos el archivo, leemos, vamos creando los procesos y los guardamos */
    FILE *file = fopen(inp, "r");
    char line[2048];
    
    fgets(line, sizeof(line), file);
    printf("Cantidad procesos = %s\n", line);
    int cantidad_procesos = atoi(line);

    for (int i = 0; i < cantidad_procesos; ++i)
    {
        fgets(line, sizeof(line), file);
        printf("Proceso: %s\n", line);

        Process* proceso = calloc(1, sizeof(Process));

        char *token = strtok(line, " ");
        
        proceso->estado = READY;
        printf(">Nombre proceso: %s\n", token);
        strcpy(proceso->name, token);

        token = strtok(NULL, " ");
        printf(">PID: %s\n", token);
        proceso->PID = atoi(token);

        token = strtok(NULL, " ");
        printf(">Tiempo inicio: %s\n", token);
        proceso->t_inicio = atoi(token);

        token = strtok(NULL, " ");
        printf(">Deadline: %s\n", token);
        proceso->deadline = atoi(token);

        token = strtok(NULL, " ");
        printf(">Cantidad burst: %d\n", (atoi(token)-1)*2 + 1);
        proceso->cantidad_burst = (atoi(token)-1)*2 + 1;

        token = strtok(NULL, " ");
        int j = 0;
        while (token != NULL)
        {
            printf("Burst = %s\n", token);
            proceso->arreglo_burst[j] = atoi(token);
            token = strtok(NULL, " ");
            j += 1;
        }
        proceso -> size_arreglo_burst = j; // para saber si se termino de ejecutar
        proceso->waiting_time = 0;
        proceso->turnos_cpu = 0;
        proceso->interrupciones = 0;
        proceso->primera_ejecucion = 0;
        proceso->burst_actual = 0;
        proceso->tipo_burst = 0; //CPU
        proceso->tiempo_cambio_burst = proceso->t_inicio + proceso->arreglo_burst[proceso->burst_actual];
        proceso->turnaround_time = 0;

        cola.arreglo_procesos[i] = proceso;
    }
      
    fclose(file);


// En este punto estan los structs procesos, falta realizar el orden de ejecucion
/**For grande, donde se maneja todo lo de meter y sacar procesos */
    
    for (int tiempo = 0; tiempo < 255; ++tiempo) //
    {
        printf("%i\n",tiempo);
        //verificar en CPU si hay un proceso RUNNING
        printf("Verificando CPU\n");
        for (int cpu = 0; cpu < n_nucleos;++cpu)
       {    
            printf("CPU: %d\n", cpu);
            //si el la CPU está vacía, nos saltamos esta CPU y revisamos la siguiente
            if (arreglo_cpu[cpu]->proceso_running[0] == NULL)
            {
                printf("CPU vacía\n");
                continue;
            }
            if (arreglo_cpu[cpu]->proceso_running[0] != NULL) //Si la CPU no está vacía
            {
                printf("CPU no vacía\n");
                int deadline_proceso = arreglo_cpu[cpu]->proceso_running[0]->deadline;
                if (deadline_proceso == tiempo) // si es que estamos en el deadline
                {
                    printf("Proceso dentro de deadline\n");
                    arreglo_cpu[cpu]->proceso_running[0]->estado = FINISHED; //cambiamos estado a FINISHED
                    // calculamos su turnaround_time
                    arreglo_cpu[cpu]->proceso_running[0]->turnaround_time = tiempo - arreglo_cpu[cpu]->proceso_running[0]->t_inicio;
                    arreglo_cpu[cpu]->proceso_running[0]->finished = 0; //NO Terminó bien, así que finished = 0

                    arreglo_cpu[cpu]->proceso_running[0] = NULL;//liberar cpu
                    printf("Cpu liberada x deadline\n");
                }
            }   
            //si debemos cambiarnos de burst. En este caso de CPU a I/O
            if (arreglo_cpu[cpu]->proceso_running[0] != NULL && arreglo_cpu[cpu]->proceso_running[0]->tiempo_cambio_burst == tiempo)
            {
                printf("Proceso dentro CPU cambia de bursts\n");
                arreglo_cpu[cpu]->proceso_running[0]->burst_actual += 1; //para leer en arreglo_burst
                arreglo_cpu[cpu]->proceso_running[0]->estado = WAITING; //cambiamos a waiting
                arreglo_cpu[cpu]->proceso_running[0]->tipo_burst = 1;
                //actualizamos el tiempo en que debería terminar el próximo burst
                arreglo_cpu[cpu]->proceso_running[0]->tiempo_cambio_burst = tiempo + arreglo_cpu[cpu]->proceso_running[0]->arreglo_burst[arreglo_cpu[cpu]->proceso_running[0]->burst_actual];
                
                printf("Cpu liberada del proceso : %i por burst \n",arreglo_cpu[cpu]->proceso_running[0]->PID);
                arreglo_cpu[cpu]->proceso_running[0] = NULL; //liberar cpu
            }
        }


        //verificar en arreglo de procesos, si es que hay que cambiar algún estado
        printf("Verificar cambio de estado entre procesos\n");
        for (int i = 0; i < cantidad_procesos; ++i)
        {
            //si el arreglo está en un instante donde debe cambiar de burst
            if (tiempo == cola.arreglo_procesos[i]->tiempo_cambio_burst ) 
            {
                printf("Proceso %d debe cambiar de burst\n", cola.arreglo_procesos[i]->PID);
                cola.arreglo_procesos[i]->burst_actual += 1; //esto nos sive para leer en el arreglo_burst
                cola.arreglo_procesos[i]->estado = READY;
                cola.arreglo_procesos[i]->tipo_burst = 0; //burst estaba esperando (burst I/O) y ahora cambió a READY (burst CPU)  
                //actualizamos el tiempo en que debería terminar el próximo burst
                if (cola.arreglo_procesos[i]->burst_actual >= cola.arreglo_procesos[i]->size_arreglo_burst && cola.arreglo_procesos[i]->primera_ejecucion == 1) // si completo su ultimo burst
                {
                    cola.arreglo_procesos[i]->estado = FINISHED;
                    cola.arreglo_procesos[i]->finished = 1; //termino la ejecucion correctamente, finished = 1
                }
                cola.arreglo_procesos[i]->tiempo_cambio_burst = tiempo + cola.arreglo_procesos[i]->arreglo_burst[cola.arreglo_procesos[i]->burst_actual];
            }

            
        }

        //Buscar en el arreglo de procesos, el más prioritario de los procesos según el deadline y que esté en estado ready
        //Comparamos los procesos hasta elegir el que esté READY con menor deadline
        Process* proceso_prioritario;
        printf("Buscando proceso prioritario\n");
    //Buscamos el primero que esté READY, para usarlo como referencia
            for (int j = 0; j < cantidad_procesos; ++j)
            {
                if (cola.arreglo_procesos[j]->estado == READY)
                {
                    proceso_prioritario = cola.arreglo_procesos[j];
                    printf("Proceso para referencia es %d con deadline %d\n", cola.arreglo_procesos[j]->PID, cola.arreglo_procesos[j]->deadline);
                    break;
                }
            }
        for (int i = 0; i < cantidad_procesos; ++i)
        {

            
            //Ahora comparamos el que tenemos de referencia con los otros
            if (cola.arreglo_procesos[i]->estado == READY)
            {
                   if (cola.arreglo_procesos[i]->deadline < proceso_prioritario->deadline)
                   {
                        proceso_prioritario = cola.arreglo_procesos[i];  
                   }
            }
        }
        printf("Proceso prioritario es %d y su t_inicio = %d\n", proceso_prioritario->PID, proceso_prioritario->t_inicio);
        //Ver si el proceso escogido ya debería haber empezado (t_actual >= t_inicio de proceso).
        if (proceso_prioritario->t_inicio <= tiempo)
        {
            printf("PROCESO SELECCIONADO pid: %i \n",proceso_prioritario->PID);
            printf("Buscando CPU... \n");
            //ver si el proceso es más prioritario que el de la CPU o la CPU está vacía
            for (int cpu = 0; cpu < n_nucleos; ++cpu)
            {   
                //No hay nada en esa CPU
                if (arreglo_cpu[cpu]->proceso_running[0] == NULL)
                {
                    printf("CPU %d está vacía\n", cpu);
                    //cambiar el estado de READY a RUNNING y agregarlo a la CPU
                    proceso_prioritario->estado = RUNNING;
                    printf("Proceso prioritario en RUNNING, pid: %i\n",proceso_prioritario->PID);
                    arreglo_cpu[cpu]->proceso_running[0] = proceso_prioritario;
                    //aumentar en 1 cantidad turno la cantidad de turnos CPU
                    proceso_prioritario->turnos_cpu += 1;
                    //si proceso no ha entrado a la CPU, cambiarle primera_ejecucion y calcular su response_time
                    if (proceso_prioritario->primera_ejecucion == 0)
                    {
                        printf("Primera vez que el proceso entra a CPU\n");
                        proceso_prioritario->primera_ejecucion = 1;
                        proceso_prioritario->response_time = tiempo - proceso_prioritario->t_inicio;
                    }
                    break;

                }
                if (arreglo_cpu[cpu]->proceso_running[0] != NULL)
                {
                    printf("CPU no disponible\n");
                }
                 
                //Si hay algo en la CPU
                if (arreglo_cpu[cpu]->proceso_running[0] != NULL)
                {
                    if (proceso_prioritario->deadline < arreglo_cpu[cpu]->proceso_running[0]-> deadline)
                    {

                        printf("Proceso es más prioritario que el de CPU\n");
                        //Cambiar el proceso de la CPU de RUNNING a READY
                        arreglo_cpu[cpu]->proceso_running[0]->estado = READY;
                        arreglo_cpu[cpu]->proceso_running[0]->interrupciones += 1;
                        printf("Interrupción del proceso pid : %i   ",arreglo_cpu[cpu]->proceso_running[0]->PID);
                        arreglo_cpu[cpu]->proceso_running[0] = NULL;

                        //cambiar el estado de WAITING a RUNNING y agregarlo a la CPU
                        proceso_prioritario->estado = RUNNING;
                        arreglo_cpu[cpu]->proceso_running[0] = proceso_prioritario;
                        printf("Proceso %i entra a CPU\n",arreglo_cpu[cpu]->proceso_running[0]->PID);
                        //aumentar en 1 cantidad turno la cantidad de turnos CPU
                        proceso_prioritario->turnos_cpu += 1;
                        //si proceso no ha entrado a la CPU, cambiarle primera_ejecucion y calcular su response_time
                        if (proceso_prioritario->primera_ejecucion == 0)
                        {
                            printf("Primera vez que el proceso entra a CPU\n");
                            proceso_prioritario->primera_ejecucion =1;
                            proceso_prioritario->response_time = tiempo - proceso_prioritario->t_inicio;
                        }
                        break;

                    }
                }
            }
        }

        //revisar de nuevo el arreglo de procesos y sumarle 1 al waiting_time de todos los que estén en estado READY o WAITING
        for (int i = 0; i < cantidad_procesos; ++i)
        {
            if (cola.arreglo_procesos[i]->estado == READY || cola.arreglo_procesos[i]->estado == WAITING)
            {
                if (tiempo >= cola.arreglo_procesos[i]->t_inicio)
                {
                    printf("Agregando tiempo de espera a proceso %d\n", cola.arreglo_procesos[i]->PID);
                    cola.arreglo_procesos[i]->waiting_time += 1;    
                }
                
            }
            //Si un proceso ya pasó su deadline y no está en estado FINISHED, ponerle finished = 0, porque no terminó bien
            if (cola.arreglo_procesos[i]->deadline < tiempo && cola.arreglo_procesos[i]->estado != FINISHED)
            {
                printf("El proceso %d pasó su deadline y no está terminado\n", cola.arreglo_procesos[i]->PID);
                cola.arreglo_procesos[i]->finished = 0;
                cola.arreglo_procesos[i]-> estado = FINISHED;
            }
            //si estamos en ready, no deberíamos consumir burst
            if (cola.arreglo_procesos[i]->estado == READY)
            {
                printf("El proceso %d está READY y no debería consumir burst\n", cola.arreglo_procesos[i]->PID);
                cola.arreglo_procesos[i]->tiempo_cambio_burst += 1;
            }
        }










/*


        for (int i = 0; i < cantidad_procesos; ++i) // Recorre cada proceso
        {
            //ASIGNACION DE CPU's
            for (int cpu = 0; cpu < n_nucleos;++cpu) // Recorre cada CPU (¿crear un array de cpus?)
            {-
                if (arreglo_cpu[cpu]->proceso_running[0] == NULL) //Si la CPU está vacía
                {
                    
                }
                //IF arreglo_cpu[i] in idle/not in use 
                if (cola.arreglo_procesos[i]->t_inicio == tiempo) // primera ejecucion (cuando todas las CPUs estan libres)
                {
                    cola.arreglo_procesos[i]->primera_ejecucion = 1; // primera ejecucion
                    cola.arreglo_procesos[i]->estado = RUNNING;
                    printf("PRIMERA EJECUCION del proceso %i\n",i);
                }
                if (cola.arreglo_procesos[i]->estado == READY)
                {
                    printf("Hay una CPU libre y hay un proceso esperando\n");
                    //CPU.estado EN USO
                    //cola.arreglo_procesos[i]->estado == RUNNING
                    //Numero de veces elegido para usar alguna CPU +1 
                    //Waiting time se le agrega el nuevo tiempo
                }
            }

            //VER ESTADO DE CADA PROCESO 
            if (cola.arreglo_procesos[i]->deadline == tiempo) 
            {
                cola.arreglo_procesos[i]->primera_ejecucion = 1;
                printf("El proceso %s con PID %i ha llegado a su deadline en el tiempo %i\n",cola.arreglo_procesos[i]->name,cola.arreglo_procesos[i]->PID,tiempo);
                //Se termino el proceso antes de su deadline? 1 -> si, 0-> no
            }
            //Bursts
            for (int j = 1; j<= cola.arreglo_procesos[i]->cantidad_burst;++j) // Esto quizas esta mas implementado pero el codigo sirve para asignarle los burst al proceso
            {
                if (cola.arreglo_procesos[i]->arreglo_burst[j] == tiempo) //no deberia ser con tiempo, creo que deberia tener algo como un contador interno a partir del tiempo global
                    {
                        if (j%2==0) printf("PID : %i Burst en el tiempo : %i\n", cola.arreglo_procesos[i]->PID, tiempo); // Si es par es B_N
                        else printf("Rafaga de cpu\n"); // Si es impar es As_N
                    }
            }

                
            


            
        }
 */
    }




//write un csv de nombre out
    FILE *file_o = fopen(out, "w");
    
    for (int i = 0; i < cantidad_procesos; ++i)
    {
        int turnos_cpu = cola.arreglo_procesos[i]->turnos_cpu;
        int interrupciones = cola.arreglo_procesos[i]->interrupciones;
        int turnaround_time = cola.arreglo_procesos[i]->turnaround_time;
        int response_time = cola.arreglo_procesos[i]->response_time;
        int waiting_time = cola.arreglo_procesos[i]->waiting_time;
        int deadline = cola.arreglo_procesos[i]->finished;

        fprintf(file_o, "%s,%d,%d,%d,%d,%d,%d\n", cola.arreglo_procesos[i]->name, turnos_cpu, interrupciones, turnaround_time, response_time, waiting_time, deadline);        
    }
    
    fclose(file_o);





/** Última parte. Liberar la memoria para no tener leaks*/
    for (int i = 0; i < n_nucleos; ++i)
    {
        free(arreglo_cpu[i]);
    }
    for (int i = 0; i < cantidad_procesos; ++i)
    {
        free(cola.arreglo_procesos[i]);
    }

    return 0;


}

