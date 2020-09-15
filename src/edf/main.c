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
    char *name;
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
    Process arreglo_procesos[255];
}Queue;


typedef struct cpu
{
    Process proceso_running;
}CPU;


int main(int argc, char**argv)
{
    
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
    

    CPU* arreglo_cpu[n_nucleos];
    Queue cola;
    

    for (int i = 0; i < n_nucleos; ++i)
    {
        CPU* cpu = calloc(1, sizeof(CPU));
        arreglo_cpu[i] = cpu; 
    }


    FILE *file = fopen(inp, "r");
    char line[2048];
    
    fgets(line, sizeof(line), file);
    printf("Cantidad procesos = %s\n", line);
    int cantidad_procesos = atoi(line);

    for (int i = 0; i < cantidad_procesos; ++i)
    {
        fgets(line, sizeof(line), file);
        printf("Proceso: %s\n", line);

        Process proceso;

        char *token = strtok(line, " ");
        
        printf(">Nombre proceso: %s\n", token);
        proceso.name = token;
        token = strtok(NULL, " ");
        printf(">PID: %s\n", token);
        proceso.PID = atoi(token);
        token = strtok(NULL, " ");
        printf(">Tiempo inicio: %s\n", token);
        proceso.t_inicio = atoi(token);
        token = strtok(NULL, " ");
        printf(">Deadline: %s\n", token);
        proceso.deadline = atoi(token);
        token = strtok(NULL, " ");
        printf(">Cantidad burst: %d\n", (atoi(token)-1)*2 + 1);
        proceso.cantidad_burst = (atoi(token)-1)*2 + 1;
        token = strtok(NULL, " ");

        int i = 0;
        while (token != NULL)
        {
            printf("Burst = %s\n", token);
            proceso.arreglo_burst[i] = atoi(token);
            token = strtok(NULL, " ");
            i += 1;
        }

        cola.arreglo_procesos[i] = proceso;
    }
    


    /** Para comprobar si está bien. Un espacio y después printear los procesos guardados*/

    /*
    for (int i = 0; i < 3; ++i)
    {
        printf(" \n");
    }
    for (int i = 0; i < cantidad_procesos; ++i)
    {
        Process* proceso = &cola->arreglo_procesos[i];
        printf("Nombre = %s\n", proceso->name);
        printf("PID = %d\n", proceso->PID);
        printf("Tiempo inicio = %d\n", proceso->t_inicio);
        printf("Deadline = %d\n", proceso->deadline);
        printf("Cantidad burst = %d\n", proceso->cantidad_burst);
    }

    for (int i = 0; i < cantidad_procesos; ++i)
    {
        free(&cola->arreglo_procesos[i]);
    }
    */

    fclose(file);



    return 0;


}