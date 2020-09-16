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
    int arreglo_burst[255];
    int tiempo_cambio_burst; /**(es la suma del tiempo en que comienza el burst actual + lo que demora -> el instante de tiempo en que termina el burst actual)*/
}Process;


typedef struct cola
{
    Process* arreglo_procesos[255];
}Queue;


typedef struct cpu
{
    Process proceso_running[1];
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

        cola.arreglo_procesos[i] = proceso;
    }
      
    fclose(file);


// En este punto estan los structs procesos, falta realizar el orden de ejecucion
/**For grande, donde se maneja todo lo de meter y sacar procesos */
// Version penca de lo que hay que hacer, considerando una sola CPU (**me es mas facil pensarlo empezando asi u.u)
    int tiempo = 0;
    for (int i = 0; i < 255; ++i) //
    {
        for (int i = 0; i < cantidad_procesos; ++i) // Recorre cada proceso
        {
            //ASIGNACION DE CPU's
            for (int cpu = 0; cpu<=n_nucleos;++cpu) // Recorre cada CPU (¿crear un array de cpus?)
            {
                //IF CPU[i] in idle/not in use 
                if (cola.arreglo_procesos[i]->t_inicio == tiempo) // primera ejecucion (cuando todas las CPUs estan libres)
                {
                    cola.arreglo_procesos[i]->primera_ejecucion = 1; // primera ejecucion
                    cola.arreglo_procesos[i]->estado = RUNNING;
                    printf("PRIMERA EJECUCION del proceso %i\n",i);
                }
                if (cola.arreglo_procesos[i]->estado == READY)
                {
                    printf("Hay una CPU libre y hay un proceso esperando")
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
                
            


            
        }
        tiempo = tiempo + 1;

        /* code */
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

