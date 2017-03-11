#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
    pid_t val, p1, p2;
    
    val = fork();
    if(val==0){
        printf("Hola soy el proceso hijo\n");
        exit(0);

    }
    waitpid(val,NULL,0);
    printf("Hola soy el proceso padre\n");
    return 2;
}
