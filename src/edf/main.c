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
        
        proceso->estado = 1;
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

        proceso->burst_actual = 0;
        proceso->tipo_burst = 0; //CPU
        proceso->tiempo_cambio_burst = proceso->t_inicio + proceso->arreglo_burst[proceso->burst_actual];
        proceso->estado = READY;

        cola.arreglo_procesos[i] = proceso;
    }
      
    fclose(file);


// En este punto estan los structs procesos, falta realizar el orden de ejecucion
/**For grande, donde se maneja todo lo de meter y sacar procesos */
    
    for (int tiempo = 0; tiempo < 255; ++tiempo) //
    {

        //verificar en CPU si hay un proceso RUNNING
        for (int cpu = 0; cpu < n_nucleos;++cpu)
        {
            if (arreglo_cpu[cpu]->proceso_running[0] != NULL) //Si la CPU no está vacía
            {
                int deadline_proceso = arreglo_cpu[cpu]->proceso_running[0]->deadline;
                if (deadline_proceso == tiempo) // si es que estamos en el deadline
                {
                    arreglo_cpu[cpu]->proceso_running[0]->estado = FINISHED; //cambiamos estado a FINISHED
                    // calculamos su turnaround_time
                    arreglo_cpu[cpu]->proceso_running[0]->turnaround_time = tiempo - arreglo_cpu[cpu]->proceso_running[0]->t_inicio;
                    arreglo_cpu[cpu]->proceso_running[0]->finished = 1; //Terminó bien, así que finished = 1
                }
            }
            //si debemos cambiarnos de burst. En este caso de CPU a I/O
            if (arreglo_cpu[cpu]->proceso_running[0]->tiempo_cambio_burst == tiempo)
            {
                arreglo_cpu[cpu]->proceso_running[0]->burst_actual += 1; //para leer en arreglo_burst
                arreglo_cpu[cpu]->proceso_running[0]->estado = WAITING; //cambiamos a waiting
                arreglo_cpu[cpu]->proceso_running[0]->tipo_burst = 1;
                //actualizamos el tiempo en que debería terminar el próximo burst
                arreglo_cpu[cpu]->proceso_running[0]->tiempo_cambio_burst = tiempo + arreglo_cpu[cpu]->proceso_running[0]->arreglo_burst[arreglo_cpu[cpu]->proceso_running[0]->burst_actual];
            }
        }




        //verificar en arreglo de procesos, si es que hay que cambiar algún estado
        for (int i = 0; i < cantidad_procesos; ++i)
        {
            //si el arreglo está en un instante donde debe cambiar de burst
            if (tiempo == cola.arreglo_procesos[i]->tiempo_cambio_burst)
            {
                cola.arreglo_procesos[i]->burst_actual += 1; //esto nos sive para leer en el arreglo_burst
                cola.arreglo_procesos[i]->estado = READY;
                cola.arreglo_procesos[i]->tipo_burst = 0; //burst estaba esperando (burst I/O) y ahora cambió a READY (burst CPU)  
                //actualizamos el tiempo en que debería terminar el próximo burst
                cola.arreglo_procesos[i]->tiempo_cambio_burst = tiempo + cola.arreglo_procesos[i]->arreglo_burst[cola.arreglo_procesos[i]->burst_actual];
            }
        }














        for (int i = 0; i < cantidad_procesos; ++i) // Recorre cada proceso
        {
            //ASIGNACION DE CPU's
            for (int cpu = 0; cpu < n_nucleos;++cpu) // Recorre cada CPU (¿crear un array de cpus?)
            {
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
 
    }







//write un csv de nombre out






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

