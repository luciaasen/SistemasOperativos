/**
 * @file ejercicio5a.c
 * @author Lucia Asencio y Rodrigo de Pool
 * @date 28-2-2017
 * @brief fichero que contiene ej5a de la practica 1 de SOPER.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_PROC    3

/**
 * Un proceso padre genera un hijo que a su vez genera otro y asi NUM_PROC
 * veces, cada proceso hijo imprimira su informacion.
 */
int main(void){
    int pid = 0;
    int i;
    /*Agregamos la condicion de que un proceso no tenga hijo para mantenerse*/
    for (i = 0; i < NUM_PROC && pid == 0; i++) {
        if ((pid = fork()) < 0) {
            printf("Error al emplear fork\n");
            exit(EXIT_FAILURE);
        }else if (pid == 0) {
            printf("Este es el proceso %d con padre %d\n", getpid(), getppid());
        }
    }
    /*Cada proceso espera a su hijo para cerrarse*/
    wait(NULL);
    exit(EXIT_SUCCESS);
}
