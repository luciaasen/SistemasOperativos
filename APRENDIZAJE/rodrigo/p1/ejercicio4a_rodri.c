#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_PROC 3

int main (void){
    int pid;
    int i;
    for (i=0; i < NUM_PROC; i++){
        if ((pid=fork()) <0 ){
            printf("Error al emplear fork\n");
            exit(EXIT_FAILURE);
        }else if (pid ==0){
            printf("HIJO %d\n",getpid());
        }else{
            printf ("PADRE %d \n",getppid());
        }
    }
    exit(EXIT_SUCCESS);
}
