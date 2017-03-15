#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_PROC 3

/**
*   Proceso padre genera un proceso hijo, y su hijo genera otro, y asi
*   hasta que se llegue a NUM_PROC hijos.
*   Cada hijo imprime su pid y su ppid.
*   Adicionalmente, para controlar huerfanos, los padres tambien imprimen pid
*/

int main(){
    int pid;
    int i;
    int status, flag;
    for(i = 0, flag = 0; i < NUM_PROC && flag == 0; i++){
        if((pid = fork()) < 0 ){
            printf("Error al emplear fork\n");
            exit(EXIT_FAILURE);
        }else if(pid == 0){
            printf("HIJO %d, pid %d ppid %d\n", i, getpid(), getppid());
        }else{
            flag = 1;
            /*Para ayudar a controlar la existencia de
            huerfanos, hago que los padres impriman su pid*/
            printf("PADRE %d, pid %d\n", i, getpid());
        }
    }
    wait(&status);
    
    exit(EXIT_SUCCESS);
}

