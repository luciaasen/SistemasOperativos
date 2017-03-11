#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

/**
 * Esta funcion realizara el algoritmo propuesto en el ejercicio 5
 * @return 1
 */
int main(){
    int phijo;
    phijo = fork();

    if (phijo == 0) {
        printf("Soy el proceso hijo con PID: %d\n", getpid());
        sleep(5);
        exit(EXIT_SUCCESS);
    }
    sleep(30);
    if (kill(phijo, SIGINT) != -1) {
        printf("Senal enviada CON exito\n");
    }else{
        printf("Senal enviada SIN exito\n");
    }
    wait(NULL);
    return 1;
}
