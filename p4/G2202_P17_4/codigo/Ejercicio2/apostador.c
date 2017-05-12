#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include "apostador.h"





struct msgbuf {
    long mtype;
    char mtext[1];
};



/**
 * Maneja la senial sigint para hacer un exit(0)
 * @param signo senial
 */
void sig_handler(int signo){
    exit(0);
}


int generador(int nApostadores, int nCaballos, int colaApuesta, long tipo){
    int i, apostador;
    int flag;

    /*CdE + cambio manejador*/
    /************************/
    if (nApostadores < 1 || nCaballos < 1 || colaApuesta < 1) {
        exit(-1);
    }

    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        perror("Error en el manejador");
        exit(0);
    }

    /*Genera y envia apuestas aleatoriasm parara cuando le llegue una senial*/
    /************************************************************************/
    i = 0;
    while (1) {
        apostador = i % nApostadores;
        flag      = apuestaEnvia(tipo, apostador, nCaballos, colaApuesta);
        if (flag == -1) {
            printf("Error en creacion apuesta %d\n", i);
            exit(-1);
        }
        i++;
        usleep(10000);
    }
}

int apuestaEnvia(long tipo, int idApostador, int nCaballos, int cola){
    Apuesta a = { 0 };
    if (nCaballos < 1) {
        return -1;
    }

    sprintf(a.nombre, "Apostador-%d", idApostador);
    a.numC    = rand() % nCaballos;
    a.cuantia = rand() % 100;
    a.id      = tipo;
    msgsnd(cola, (struct msgbuf *) &a, sizeof(Apuesta) - sizeof(long), IPC_NOWAIT);

    return 1;
}


/* Estos 3 getters solo son llamados desde lineas del codigo
   donde ya hemos previaente comprobado que a no es un puntero
   nulo, por eso no hacemos el control de errores
 */
int getApostador(Apuesta *a){
    int apostador;
    sscanf(a->nombre, "Apostador-%d", &apostador);
    return apostador;
}

int getCaballo(Apuesta *a){
    return a->numC;
}

double getCuantia(Apuesta *a){
    return a->cuantia;
}

