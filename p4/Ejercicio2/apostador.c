#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include "apostador.h"


struct _Apuesta{
    char nombre[20];
    int numC;
    double cuantia;
};


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

/*Esto en teoria va a enviar apuestas*/
void envia_apuesta(int cola, int tipo, Apuesta *a){
    Mensaje mns;
    mns.a = a;
    mns.id = tipo;
    msgsnd(cola, (struct msgbuf *)&mns, sizeof(Mensaje) - sizeof(long), IPC_NOWAIT);
}

int generador(int nApostadores, int nCaballos, int colaApuesta, int tipo){
    int i, apostador;
    Apuesta *a;

    /*CdE + cambio manejador*/
    /************************/
    if(nApostadores < 1 || nCaballos < 1 || colaApuesta < 1){
        exit(-1);
    }
    
    if(signal(SIGINT, sig_handler) == SIG_ERR){
        perror("Error en el manejador");
        exit(0);
    }

    /*Genera y envia apuestas aleatoriasm parara cuando le llegue una senial*/
    /************************************************************************/
    i = 0;
    while(1){
        apostador = i%nApostadores;
        a = apuesta_ini(apostador, nCaballos);
        if(a == NULL){
            printf("Error en creacion apuesta %d\n", i);
            exit(-1);
        }
        envia_apuesta(colaApuesta, tipo, a);
        i++;
        usleep(10000);
    }
}

Apuesta *apuesta_ini(int idApostador, int nCaballos){     
    Apuesta *a;
    if(nCaballos < 1){
        return NULL;
    }
    
    a = (Apuesta *)malloc(sizeof(Apuesta));
    if(a == NULL){
        return NULL;
    }
    sprintf(a->nombre, "Apostador-%d", idApostador);
    a->numC = rand()%nCaballos;
    a->cuantia = rand();    
    return a;
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

Apuesta *getApuesta(Mensaje m){
    return m.a;
}
