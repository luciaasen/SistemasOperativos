#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define NUM 3

int main (int argc, char *argv[]){
    int procs[NUM];
    int pipes[2*2];
    int i;
    char argus[2];
    /*codigo de obtener valores y guardando*/



    for(i=0;i<NUMS;i++){
        pipe(pipes);
        pipe(pipes+2);

        if(fork()==0){
            close(pipes[0]);
            close(pipes[3]);

        }else{
            close(pipes[1]);
            close(pipes[2]);
            /*guardamos en argus la variable del strin*/
            /*Mandamos por pipe al hijo los valores*/
            write(pipes[3],argus,sizeof(char)*(strlen(argus)+1));
        }
    }
    exit(EXIT_SUCCESS);

}


