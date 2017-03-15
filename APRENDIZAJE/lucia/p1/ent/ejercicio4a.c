#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_PROC 3

/**
*   Cada procesi genera un hijo, tenga ya hijos o no.
*   Cada hijo imprime su pid y su ppid.
*   Adicionalmente, para controlar posibles huerfanos, cada padre imprime pid.
*/

int main(){
    int pid;
    int i;
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
    
    exit(EXIT_SUCCESS);
}

/***************************************************************************/
/* 
    Conclusion procesos huerfanos:
   En este programa si existen procesos huerfanos, y esto se refleja en la
   salida del programa.
   Los ppid de los "Hijo N" deberian ser el pid de un "Hijo N-1" o "Padre N-1".
   Sin embargo, hay algunos "Hijo N" cuyo ppid es, independientemente de
   la ejecucion, 1518 (no correspondiente a ningun "Hijo N-1 o "Padre N-1").

   Esto nos indica que su proceso padre finaliza antes que el hijo, por lo
   cual los hijos quedan huerfanos y se les es reasignado otro padre, el 1518

   (En nuestro programa puede tomarse N = 2, N = 1)

*/
/****************************************************************************/

