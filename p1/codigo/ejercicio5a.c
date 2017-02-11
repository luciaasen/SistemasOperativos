#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_PROC 3

int main (void){
    int pid=0;
    int i;
    /*Agregamos la condicion de que un proceso no tenga hijo para mantenerse*/
    for (i=0; i < NUM_PROC && pid==0; i++){
        if ((pid=fork()) <0 ){
            printf("Error al emplear fork\n");
            exit(EXIT_FAILURE);
        }else if (pid ==0){
            printf("Este es el proceso %d con padre %d\n",getpid(),getppid());
        }
    }
    /*Esperamos al proceso hijo para cerrar el proceso padre*/
    wait(NULL);
    exit(EXIT_SUCCESS);
}
