#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    int pid, status;
    char *cad = NULL;
    cad = (char *)malloc(80 * sizeof(char));
    if(cad == NULL){
        printf("Error en la reserva de memoria\n");
        return -1;
    }
    pid = fork();
    if(pid == 0){
        printf("La cadena del hijo esta en %p\n", cad);
        printf("Introduce un nombre: ");
        scanf("%s", cad);
        printf("Has introducido %s\n", cad);
    }else{
        printf("La cadena del padre esta en %p\n", cad);
    }
    free(cad);
    wait(&status);
    exit(EXIT_SUCCESS);
}

/**************************************************************/
/*
    Hay que liberar los recursos reservados en ambos procesos,
    ya que el proceso padre y el hijo no comparten los bloques
    de datos.
*/
