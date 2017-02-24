#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define NUM 4

void operation(int x, int y, int op,int handle);


/**
*   Esta es la funcion principal del ejercicio 9.
*   Pedira por pantalla dos valores que se les seran
*   pasados a sus hijos para realizar ciertas operaciones. Una vez realizadas
*   imprimiran el resultado de la operacion.
*
*   Los numeros han de ser enteros n tal que : 0<n<10
*/
int main (int argc, char *argv[]){
    /*Two pipes for each child*/
    int pipes[2*2*NUM];
    int i,j,x,y;
    char argus[4];
    char desc[140];
    /*We initialize the pipes*/
    for(i=0;i<NUM;i++){
        /*Pipe for child to read*/
        pipe(pipes+i*4);
        /*Pipe for child to write*/
        pipe(pipes+i*4+2);
    }

    for(i=0;i<NUM;i++){
        if(fork()==0){
            close(pipes[1+i*4]);
            close(pipes[2+i*4]);
            /*We close pipes not corresponding to the child*/
            for(j=0;j<NUM;j++){
                if(j!=i){
                    close(pipes[0+j*4]);
                    close(pipes[1+j*4]);
                    close(pipes[2+j*4]);
                    close(pipes[3+j*4]);
                }
            }
            /*Read from parent the arguments*/
            read(pipes[0+i*4],argus,sizeof(char)*4);
            /*parse the arguments*/
            x=argus[0]-'0';
            y=argus[2]-'0';
            /*operation its in charge of doing the sending*/
            operation(x,y,i,pipes[3+i*4]);
            exit(EXIT_SUCCESS);
        }else{
            /*We ensure to close the handles for each child*/
            /*so the mutual comunication is established*/
            close(pipes[0+i*4]);
            close(pipes[3+i*4]);
        }
    }
    printf("Introduzca los numeros con el formato indicado: ");
    scanf("%s",argus);
    /*Enviamos a cada hijo los argumentos*/
    /*esperamos el resultado del hijo y lo imprimimos*/
    for(i=0;i<NUM;i++){
        write(pipes[1+i*4], argus, sizeof(char)*4);
        read(pipes[2+i*4], desc, sizeof(char)*140);
        printf("%s\n",desc);
    }
    /*cerramos todos los hijos */
    for(i=0; i<NUM ;i++)
        wait(NULL);
    exit(EXIT_SUCCESS);

}

/**
*   Devuelve el resultado (string) de la operacion indicada a traves del handle
*   dado.
*   @param x es uno de los operandos.
*   @param y es el segundo operando.
*   @param op indica la operacion.
*   @param handle es el descriptor del fichero al que se enviara el string.
*
*/
void operation(int x, int y, int op,int handle){
    char s[140];
    switch(op){
        case 0:
            sprintf(s,"Datos enviados a traves de la tuberia con PID=%d. Operando 1: %d. Operando 2: %d. Suma: %d\n", getpid(),x,y,x+y);
            break;
        case 1:
            sprintf(s,"Datos enviados a traves de la tuberia con PID=%d. Operando 1: %d. Operando 2: %d. Resta: %d\n", getpid(),x,y,x-y);
            break;
        case 2:
            sprintf(s,"Datos enviados a traves de la tuberia con PID=%d. Operando 1: %d. Operando 2: %d. Multiplicacion: %d\n", getpid(),x,y,x*y);
            break;
        case 3:
            sprintf(s,"Datos enviados a traves de la tuberia con PID=%d. Operando 1: %d. Operando 2: %d. Division: %f\n", getpid(),x,y,x/(float)y);
            break;
        default:
            return;
    }
    /*now we send it */
    write(handle,s,sizeof(char)*(strlen(s)+1));
    return;
}

