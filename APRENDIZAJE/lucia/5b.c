#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_PROC 3

/* 
   El cambio introducido para que solo haya un padre:
    1) Añadimos un fork antes del for (el primero siempre se ejecuta)
    2) Dentro del for, el fork solo se ejecuta si eres un proceso padre
    3) Añadimos un flag que se activa cuando un hijo ya ha sido escrito
       por pantalla, para no repetir su escritura.

   El cambio introducido para que el padre espere a todos los hijos:
    1) En vez de un solo wait, se hace un do while de waits que cesa cuando
       un wait devuelve algo < 0 (es decir, no ha podido esperar a nadie)
*/

int main(){
    int pid;
    int i;
    int flag;
    int status;

    pid = fork();
    flag = 0;
    for(i = 0; i < NUM_PROC && flag == 0; i++){
        if(pid < 0 ){
            printf("Error al emplear fork\n");
            exit(EXIT_FAILURE);
        }else if(pid == 0){
            printf("HIJO %d, pid %d ppid %d\n", i, getpid(), getppid());
            flag = 1;
        }else{
            printf("PADRE %d, pid %d\n", i, getpid());
            pid = fork();
        }
    }

    do{
        pid = wait(&status);
    }while(pid != -1);
    
    exit(EXIT_SUCCESS);
}



