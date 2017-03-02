/**
* @file ejercicio4b.c
* @author Lucia Asencio y Rodrigo de Pool
* @date 28-2-2017
* @brief fichero que contiene ej4b de la practica 1 de SOPER para el estudio de la existencia de hijos huerfanos.
*/

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
*   Ademas, cada proceso hace un wait antes de terminar la ejecucion.
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

