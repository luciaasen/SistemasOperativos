#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/msg.h>

#include "caballos.h"

#define FIRST      1
#define LAST       9
#define NONE       5
#define MSJ_CAB    99

/*Cada caballo viene representado por sus valores en los arrays*/
/*En el caso de los pipes cada caballo tiene dos valores en el array*/
typedef struct _infoCaballos {
    int numCaballos;
    int longCarrera;
    int *idProcCaballos; //id de los procesos de caballos
    int *valoresTotales; //Valores en carrera
    int *estadoActual;   //estados FIRST, LAST, NONE
    int *pipes;          // Comunicacion monitor con caballos
    int endFlag;         // flag de finalizcion de carrera, si es forzada (SIGINT)
    int buzon;           // buzon de mensajes comunicacion de cabllos con monitor
}  infoCaballos;

/**
 * Estructura de mensajes que envian los caballos al monitor
 * Seran de tipo MSJ_CAB
 */
typedef struct _mensajeCaballo {
    long id;
    int  tirada;
    int  idCaballo; /*valor del caballo que envia el mensaje 1,2,3...*/
}mensajeCaballo;

/*Funcion que ejecutan todos los procesos de caballos*/
void procesoCaballo(int id, int *pipe, int buzon);

/*Cabeceras de funciones auxiliares y privadas*/
void cierraComunicaciones(infoCaballos *info);
int inicializacionInfoArrays(infoCaballos * info, int numC, int longCarrera);
int generaComunicaciones(infoCaballos *info);
int generaProcesos(infoCaballos *info);
void setAllNULL(infoCaballos *info);


/* ZONA DE FUNCION DE INICIALIZACION Y SUS AUXILIARES*/

infoCaballos *inicializaCaballos(int numC, int longCarrera){
    infoCaballos *info;

    info = (infoCaballos *) malloc(sizeof(infoCaballos));
    setAllNULL(info); /*Para evitar errores en caso de fallo de memoria*/
    if (info == NULL) {
        printf("Error de memoria.\n");
        return NULL;
    }else if (inicializacionInfoArrays(info, numC, longCarrera) == FALSE) {
        printf("Error de memoria.\n");
        free(info);
        return NULL;
    }else if (generaComunicaciones(info) == FALSE) {
        printf("Error de comunicaciones.\n");
        free(info->valoresTotales);
        free(info->estadoActual);
        free(info->idProcCaballos);
        cierraComunicaciones(info);
        free(info->pipes);
        free(info);
        return NULL;
    }else if (generaProcesos(info) == FALSE) {
        printf("Error al generar procesos.\n");
        free(info->valoresTotales);
        free(info->estadoActual);
        cierraComunicaciones(info);
        free(info->pipes);
        free(info);
        return NULL;
    }
    return info;
}


void  setAllNULL(infoCaballos *info) {
    info->idProcCaballos = NULL;
    info->valoresTotales = NULL;
    info->estadoActual   = NULL;
    info->pipes          = NULL;
    info->endFlag        = FALSE;
}

int inicializacionInfoArrays(infoCaballos * info, int numC, int longCarrera){
    int i;
    info->numCaballos    = numC;
    info->longCarrera    = longCarrera;
    info->idProcCaballos = (int *) malloc(sizeof(int) * numC);
    info->valoresTotales = (int *) malloc(sizeof(int) * numC);
    info->estadoActual   = (int *) malloc(sizeof(int) * numC);
    info->pipes          = (int *) malloc(sizeof(int) * 2 * numC);
    if (info->idProcCaballos == NULL || info->valoresTotales == NULL
        || info->estadoActual == NULL || info->pipes == NULL) return FALSE;
    /*Inicializamos valores totales a 0 y estados a NONE*/
    for (i = 0; i < numC; i++) {
        info->valoresTotales[i] = 0;
        info->estadoActual[i]   = NONE;
    }

    return TRUE;
}

int generaComunicaciones(infoCaballos *info){
    int i, clave;
    /*Tuberias*/
    for (i = 0; i < info->numCaballos; i++) {
        if (pipe(info->pipes + 2 * i) == -1) return FALSE;
    }
    /*Buzon*/
    srand(time(NULL) * getpid());
    clave       = rand();
    info->buzon = msgget(clave, 0600 | IPC_CREAT);
    if (info->buzon == -1) {
        return FALSE;
    }

    return TRUE;
}



int generaProcesos(infoCaballos *info){
    int i, aux, j;
    for (i = 0; i < info->numCaballos; i++) {
        aux = fork();
        if (aux == -1) {
            for (j = 0; j < i; j++)
                kill(info->idProcCaballos[j], SIGINT);
            return FALSE;
        }else if (aux == 0) {
            procesoCaballo(i + 1, info->pipes + 2 * i, info->buzon);
            return -10; /*Aqui nunca se llega, aunque por sia acaso*/
        }else{
            info->idProcCaballos[i] = aux;
            close(info->pipes[0 + i * 2]); /*cierre descriptor de entrada*/
        }
    }
    return TRUE;
}


/* FIN DE ZONA DE FUNCION DE INICIALIZACION*/

void cierraComunicaciones(infoCaballos *info){
    int i;
    for (i = 0; i < info->numCaballos * 2; i++) {
        close(info->pipes[i]);
    }
    msgctl(info->buzon, IPC_RMID, NULL);
}


void siguienteRonda(infoCaballos *info){
    int i;
    for (i = 0; i < info->numCaballos; i++) {
        write(info->pipes[1 + 2 * i], info->estadoActual + i, sizeof(int));
    }
}

void actualizaRonda(infoCaballos *info){
    int            i, idC, primero, ultimo;
    mensajeCaballo mensaje;

    for (i = 0; i < info->numCaballos; i++) {
        msgrcv(info->buzon, (struct msgbuf *) &mensaje,
               sizeof(mensajeCaballo), MSJ_CAB, 0);
        idC                           = mensaje.idCaballo;
        info->valoresTotales[idC - 1] = info->valoresTotales[idC - 1] + mensaje.tirada;
    }
    primero = idPrimerLugar(info);
    ultimo  = idUltimoLugar(info);
    for (i = 0; i < info->numCaballos; i++) {
        if (primero - 1 == i) {
            info->estadoActual[i] = FIRST;
        }else if (ultimo - 1 == i) {
            info->estadoActual[i] = LAST;
        }else{
            info->estadoActual[i] = NONE;
        }
    }
}

void imprimeRonda(infoCaballos *info){
    int i;
    printf("\nRESUMEN DE LA CARRERA :\n");
    for (i = 0; i < info->numCaballos; i++) {
        printf("Caballo %d: Ha recorrido %d de %d, posicion %d.\n",
               i + 1, info->valoresTotales[i], info->longCarrera,
               posicionCaballo(info, i + 1));
    }
}

int hayGanador(infoCaballos *info){
    int idC;
    idC = idPrimerLugar(info);

    /*final forzado*/
    if (info->endFlag == TRUE) return TRUE;

    if (info->valoresTotales[idC - 1] >= info->longCarrera)
        return TRUE;
    return FALSE;
}

int posicionCaballo(infoCaballos *info, int id){
    int i;
    int pos = info->numCaballos;

    if (id > info->numCaballos) return -1;

    for (i = 0; i < info->numCaballos; i++) {
        if (i != id - 1) {
            if (info->valoresTotales[id - 1] >= info->valoresTotales[i]) {
                pos--;
            }
        }
    }
    return pos;
}


/*auxiliar para el caso de empates en el primer lugar*/
int idPrimerLugarFlag(infoCaballos *info, int flag){
    int i, num;
    num = flag;
    for (i = 0; i < info->numCaballos; i++) {
        if (posicionCaballo(info, i + 1) == 1) {
            num--;
            if (num < 0)
                return i + 1;
        }
    }
    return -1;
}

int idPrimerLugar(infoCaballos *info){
    return idPrimerLugarFlag(info, 0);
}


/*auxiliar para casos de empate en el segundo lugar*/
int idSegundoLugarFlag(infoCaballos *info, int flag){
    int i, num;
    num = flag;
    for (i = 0; i < info->numCaballos; i++) {
        if (posicionCaballo(info, i + 1) == 2) {
            num--;
            if (num < 0)
                return i + 1;
        }
    }
    return idPrimerLugarFlag(info, flag + 1);
}

int idSegundoLugar(infoCaballos *info){
    return idSegundoLugarFlag(info, 0);
}


int idTercerLugar(infoCaballos *info){
    int i;
    for (i = 0; i < info->numCaballos; i++) {
        if (posicionCaballo(info, i + 1) == 3)
            return i + 1;
    }
    return idSegundoLugarFlag(info, 1);
}


int idUltimoLugar(infoCaballos *info){
    int i, max, id, aux;
    max = 0;
    for (i = 0; i < info->numCaballos; i++) {
        aux = posicionCaballo(info, i + 1);
        if (aux >= max) {
            max = aux;
            id  = i + 1;
        }
    }
    return id;
}

void imprimeResultados(infoCaballos *info){
    printf("\nCarrera finalizada.\n");
    printf("\nRESULTADOS DE LA CARRERA:\n");
    printf("Primer lugar: Caballo %d\n", idPrimerLugar(info));
    printf("Segundo lugar: Caballo %d\n", idSegundoLugar(info));
    printf("Tercer lugar: Caballo %d\n", idTercerLugar(info));
}


void forzarFin(infoCaballos *info){
    info->endFlag = TRUE;
}

void finalizaLibera(infoCaballos *info){
    int i;
    for (i = 0; i < info->numCaballos; i++) {
        /*Finalizamos con sigusr1 porque los caballos ignoran la senyal sigint*/
        kill(info->idProcCaballos[i], SIGUSR1);
    }
    cierraComunicaciones(info);
    free(info->valoresTotales);
    free(info->idProcCaballos);
    free(info->estadoActual);
    free(info->pipes);
    free(info);
    return;
}

/*INICIO DE PROCESO EJECUTADO POR LOS CABALLOS*/

/*Funcion auxiliares*/
int tirada(int estado);
void enviaMensajeCaballo(int buzon, int avance, int idCaballo);
void manejadorCaballo(int sing){
    return;
}

/**
 * proceso que ejecuta las acciones de los caballos de carrera
 * @param id    id de este caballo
 * @param pipe  pipe para recibir informacion del monitor
 * @param buzon buzon en el cual se dejan los mensajes al monitor
 */
void procesoCaballo(int id, int *pipe, int buzon){
    int estado, avance;

    signal(SIGINT, manejadorCaballo); /*Ignora la senial cuando le llega sigInt al proceso padre*/

    srand(time(NULL) * getpid());
    close(pipe[1]);
    do {
        read(pipe[0], &estado, sizeof(int));
        avance = tirada(estado);
        enviaMensajeCaballo(buzon, avance, id);
    } while (1);
}

int tirada(int estado){
    int avance;

    if (estado == FIRST) {
        avance = 1 + (rand() % 7);
    }else if (estado == LAST) {
        avance = 1 + (rand() % 6);
        avance = avance + (1 + rand() % 6);
    }else{
        avance = 1 + (rand() % 6);
    }
    return avance;
}

void enviaMensajeCaballo(int buzon, int avance, int idCaballo){
    mensajeCaballo msj;
    msj.id        = MSJ_CAB;
    msj.tirada    = avance;
    msj.idCaballo = idCaballo;
    msgsnd(buzon, (struct msgbuf *) &msj, sizeof(mensajeCaballo), IPC_NOWAIT);
}

/*FIN DE PROCESOS EJECUTADO POR LOS CABALLOS*/