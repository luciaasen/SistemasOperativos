#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_PROC 3

int main(){
    int pid, actual, padre;
    int i;
    
    for(i=0; i<NUM_PROC;i++){
        if((pid=fork())<0){
            printf("Error al emplear fork \n");
            exit(EXIT_FAILURE);
        }else if(pid==0){
            actual=getpid();
            padre=getppid();
            printf("Este proceso es el %d con padre %d\n",actual,padre);
        }
    }
    /*El proceso con mas hijos tiene 3, con esto nos aseguramos que todos*/
    /*los procesos finalizan antes de terminar la ejecucion de cualquiera*/
    wait(NULL);
    wait(NULL);
    wait(NULL);
    exit(EXIT_FAILURE);
}
