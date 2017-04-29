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

//Funcion que ejecutan todos los procesos de caballos
void procesoCaballo(int id, int *pipe, int buzon);

//Cabeceras de funciones auxiliares y privadas
void cierraComunicaciones(infoCaballos *info);
int inicializacionInfoArrays(infoCaballos * info, int numC, int longCarrera);
int generaComunicaciones(infoCaballos *info);
int generaProcesos(infoCaballos *info);
void setAllNULL(infoCaballos *info);


// ZONA DE FUNCION DE INICIALIZACION Y SUS AUXILIARES

infoCaballos *inicializaCaballos(int numC, int longCarrera){
    infoCaballos *info;

    info = (infoCaballos *) malloc(sizeof(infoCaballos));
    setAllNULL(info); //Para evitar errores en caso de fallo de memoria
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
    //Inicializamos valores totales a 0 y estados a NONE
    for (i = 0; i < numC; i++) {
        info->valoresTotales[i] = 0;
        info->estadoActual[i]   = NONE;
    }

    return TRUE;
}

int generaComunicaciones(infoCaballos *info){
    int i, clave;
    //Tuberias
    for (i = 0; i < info->numCaballos; i++) {
        if (pipe(info->pipes + 2 * i) == -1) return FALSE;
    }
    //Buzon
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


// FIN DE ZONA DE FUNCION DE INICIALIZACION

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
        //IGUALLL NO LOSE HAY QUE PONER - SIZEOF(LONG)
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

    if (info->valoresTotales[idC - 1] > info->longCarrera)
        return TRUE;
    return FALSE;
}

int posicionCaballo(infoCaballos *info, int id){
    int i;
    int pos = info->numCaballos;

    if (id > info->numCaballos + 1) return -1;

    for (i = 0; i < info->numCaballos; i++) {
        if (i != id - 1) {
            if (info->valoresTotales[id - 1] >= info->valoresTotales[i]) {
                pos--;
            }
        }
    }
    return pos;
}

int idPrimerLugar(infoCaballos *info){
    int i;
    for (i = 0; i < info->numCaballos; i++) {
        if (posicionCaballo(info, i) == 1)
            return i + 1;
    }
    return -1;
}


int idSegundoLugar(infoCaballos *info){
    int i;
    for (i = 0; i < info->numCaballos; i++) {
        if (posicionCaballo(info, i) == 2)
            return i + 1;
    }
    return -1;
}


int idTercerLugar(infoCaballos *info){
    int i;
    for (i = 0; i < info->numCaballos; i++) {
        if (posicionCaballo(info, i) == 3)
            return i + 1;
    }
    return -1;
}


int idUltimoLugar(infoCaballos *info){
    int i, max, id, aux;
    max = 0;
    for (i = 0; i < info->numCaballos; i++) {
        aux = posicionCaballo(info, i);
        if (aux >= max) {
            max = aux;
            id  = i + 1;
        }
    }
    return id;
}

void imprimeResultados(infoCaballos *info){
    printf("Carrera finalizada.\n");
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
        kill(info->idProcCaballos[i], SIGINT);
    }
    cierraComunicaciones(info);
    free(info->valoresTotales);
    free(info->idProcCaballos);
    free(info->estadoActual);
    free(info->pipes);
    free(info);
    return;
}

//
//
/**
 * Proceso que ejecuta cada caballo de forma independiente.
 * @param pipe Para que el monitor se comunique con el.
 * //FALTA COSAS PARA PODER COMUNICAR EL CABALLO CON EL MONITOR.
 * //MANEJADOR PARA CERRAR CUANDO ACABE LA CARRERA
 */
void procesoCaballo(int id, int *pipe, int buzon){
    //cierra pipe que no se necesita
    //bucle do while con:
    //recibe info por el pipe, espera ocupada
    //genera su numero en base a su estados
    //envia mensaje por buzon
}
