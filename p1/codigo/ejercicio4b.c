#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_PROC 3

/**
*   Cada proceso genera un hijo, independientemente de si tiene ya o no,
*   hasta que el padre alcanza NUM_PROC hijos.
*   Cada hijo imprime pid y ppid.
*   Para controlar huerfanos, cada padre imprime su pid.
*/

int main(){
    int pid;
    int i;
    int status;
    for(i = 0; i < NUM_PROC; i++){
        if((pid = fork()) < 0 ){
            printf("Error al emplear fork\n");
            exit(EXIT_FAILURE);
        }else if(pid == 0){
            printf("HIJO %d, pid %d ppid %d\n", i, getpid(), getppid());
        }else{
            /*Para ayudar a controlar la existencia de
            huerfanos, hago que los padres impriman su pid*/
            printf("PADRE %d, pid %d\n", i, getpid());
        }
    }
    wait(&status);
    
    exit(EXIT_SUCCESS);
}

/**************************************************************************/

/*
     Conclusion procesos huerfanos:
    Todos los "Hijos N" tienen un padre cuyo pid es de un "Padre N-1" o un 
    "Hijo N-1", parece ser que ningun hijo quedo huerfano.
    Esto tiene una explicacion: con la sentencia wait(), cada proceso espera
    a su proceso hijo para finalizar, de manera que en ningun proceso hijo
    se encuentra con que su ppid ya no corresponde a ningun proceso.
    Por tanto, este programa no genera hijos huerfanos.

*/
/**************************************************************************/
