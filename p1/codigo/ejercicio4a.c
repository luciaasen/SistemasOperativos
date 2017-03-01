/**
* @file ejercicio4a.c
* @author Lucia Asencio y Rodrigo de Pool
* @date 28-2-2017
* @brief fichero que contiene ej4a de la practica 1 de SOPER para el estudio de la existencia de hijos huerfanos.
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_PROC 3

/**
*   Funcion main:
*   Cada proceso genera un hijo, tenga ya hijos o no.
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

