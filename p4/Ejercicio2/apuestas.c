#include "apuestas.h"

struct _Ret{
    int cola;
    int tipo;
    int pidGestor;
    int pidApostador;
}

struct _mens{
    long type;
    char[1] c;
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

        if(generador(numA, numC, colaApuesta, ret->tipo) == -1){
            exit(-1)
        }
    }else{
        
        ret->pidApostador = pidApostador;
        pidGestor = fork();
        if(pidGestor == -1){
           
           perror("Error en el fork para el gestor\n");
           return NULL;

        }else if (pidGestor == 0){

            if(gestorApuestas(colaApuesta, colaMain, ret->tipo, numC, numA, numV) == NULL){
                exit(-1); 
            }
        }else{
            ret->pidGestor = pidGestor;
            return ret;
        }
    }
}

MensajeRes *paraApuestas(Ret *r){
    mens m;
    MensajeRes *resultados;
    
    if(r == NULL){
        return -1;
    }
    else{

        resultados = (Mensaje *)malloc(sizeof(Mensaje));
        if(resultados == NULL){
            return -1;
        }
        mens.type = r->tipo;
        msgsnd(r->cola, (msgbuf*)&m, sizeof(mens) - sizeof(long), IPC_NOWAIT);
        kill(r->pidApostador, SIGINT);
        waitpid(r->pidGestor, NULL, 0);
        msgrcv(r->cola, (msgbuf *)resultados, sizeof(MensajeRes) - sizeof(long), r->tipo, 0);
        return resultados;
    }
}
