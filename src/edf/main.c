#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
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
    
    return 0;


}