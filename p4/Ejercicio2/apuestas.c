#include "apuestas.h"

struct _Ret{
    int cola;
    int tipo;
    int pid1;
    int pid2;
}

Ret *apuestas(int numC, int numV, int numA){
    int colaMain, colaApuesta, key;
    int pidGestor, pidApostador;
    Ret *ret = (Ret *)malloc(sizeof(Ret));
    
    if(ret == NULL){
        return NULL;
    }
    /*Crea cola main*/
    srand(time(NULL) * getpid());
    key = rand();    
    colaMain = msgget(key, 0660);
    if(colaMain == -1){
        perror("Error en la creacion de colaMain\n");
        free(ret);
        return NULL;
    }
    ret->cola = colaMain;
    ret->tipo = MTYPE;

    /*Crea cola para las apuestas*/
    srand(time(NULL) * getpid());
    key = rand();i
    colaApuesta = msgget(clave, 0660);
    if(colaApuesta == -1){
        perror("Error en la creacion de colaApuesta\n");
        free(ret);
        return NULL;
    }
    
    /*Lanza hijos*/
    pidApostador = fork();
    if(Apostador == -1){

        perror("Error en el fork para apostador\n");
        free(ret);
        return NULL;

    }else if(pidApostador == 0){

        if(generador(numA, numC, colaApuesta, MTYPE) == -1){
            exit(-1)
        }
    }else{
        
        ret->pid1 = pidApostador;
        pidGestor = fork();
        if(pidGestor == -1){
           
           perror("Error en el fork para el gestor\n");
           return NULL;

        }else if (pidGestor == 0){

            if(gestorApuestas(colaApuesta, colaMain, MTYPE, numC, numA, numV) == NULL){
                exit(-1); 
            }
        }else{
            ret->pid2 = pidGestor
            return ret;
        }
    }
}
