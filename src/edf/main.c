#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "args/args.h"


void run()
{

}


int main(int argc, char**argv)
{
    printf("Pusiste %i argumentos \n", argc);
    int n_arg = argc;
    char * inp = argv[1];
    char * out = argv[2];
    char * n_nucleos;
    printf("input : %s\n",inp);
    printf("output : %s\n",out);
    if (n_arg == 1) 
    {
        printf("ERROR\nNo has ingresado argumentos\n");
    }
    else if (n_arg > 3) 
    {
        n_nucleos = argv[3];
        printf("n_nucleos : %s\n", n_nucleos);
    }
    else 
    {
        n_nucleos = "1";
    }
    run();
    return 0;


}