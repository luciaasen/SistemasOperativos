#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "apuestas.h"

#define PHI       "/bin/ls"
#define LAMBDA    11

struct _Ret {
    int cola;
    int tipo;
    int pidGestor;
    int pidApostador;
};



struct msgbuf {
    long mtype;
    char mtext[1];
};

Ret *apuestas(int numC, int numV, int numA){
    int  colaMain, colaApuesta;
    long key;
    int  pidGestor, pidApostador;
    Ret  *ret = (Ret *) malloc(sizeof(Ret));

    /**Creo colaApuesta y colaMain, reservo memoria*/
    /***********************************************/
    if (ret == NULL) {
        return NULL;
    }
    /*Crea cola main*/
    srand(time(NULL) * getpid());
    key = rand();
    //key = ftok(PHI, LAMBDA);
    colaMain = msgget(key, 0660);
    if (colaMain == -1) {
        perror("Error en la creacion de colaMain\n");
        free(ret);
        return NULL;
    }
    ret->cola = colaMain;
    ret->tipo = MTYPE;

    /*Crea cola para las apuestas*/
    srand(time(NULL) * getpid());
    key         = rand();
    colaApuesta = msgget(key, 0660);
    if (colaApuesta == -1) {
        perror("Error en la creacion de colaApuesta\n");
        free(ret);
        return NULL;
    }

    /*Lanza hijos: un gestor y un apostador, guardo sus pids para poder cargarmelos*/
    /*******************************************************************************/
    pidApostador = fork();
    if (pidApostador == -1) {
        perror("Error en el fork para apostador\n");
        free(ret);
        return NULL;
    }else if (pidApostador == 0) {
        if (generador(numA, numC, colaApuesta, ret->tipo) == -1) {
            exit(-1);
        }
    }else{
        ret->pidApostador = pidApostador;
        pidGestor         = fork();
        if (pidGestor == -1) {
            perror("Error en el fork para el gestor\n");
            return NULL;
        }else if (pidGestor == 0) {
            if (gestorApuestas(colaApuesta, colaMain, ret->tipo, numC, numA, numV) == NULL) {
                exit(-1);
            }
        }else{
            ret->pidGestor = pidGestor;
            return ret;
        }
    }
}

infoApuestas *paraApuestas(Ret *r){
    mens         m;
    infoApuestas *resultados;

    if (r == NULL) {
        return NULL;
    }else  {
        /*Manda mensaje al gestor, mata al apostador, recibe info del gestor*/
        /********************************************************************/
        resultados = (infoApuestas *) malloc(sizeof(infoApuestas));
        if (resultados == NULL) {
            return NULL;
        }

        m.type = r->tipo;
        printf("La funcion para apuestas envia a cola %d tipo %d\n", r->cola, r->tipo);

        msgsnd(r->cola, (struct msgbuf *) &m, sizeof(mens) - sizeof(long), IPC_NOWAIT);
        kill(r->pidApostador, SIGINT);
        printf("pid al q espera paraApuesta %d, pid al que mata %d\n", r->pidGestor, r->pidApostador);
        waitpid(r->pidGestor, NULL, WUNTRACED);
        printf("Apuestas piensa que el gestor ha muerto\n");
        printf("La funcion para apuestas intenta recibir de cola %d tipo %d\n", r->cola, r->tipo);
        msgrcv(r->cola, (struct msgbuf *) resultados, sizeof(infoApuestas) - sizeof(long), r->tipo, 0);
        printf("recibe el mismo\n");
        printf("Hola after receive de paraApeustas\n");
        return resultados;
    }
}
