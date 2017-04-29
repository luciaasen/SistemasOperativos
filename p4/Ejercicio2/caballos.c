#include "caballos.h"
#include <time.h>

#define FIRST    1
#define LAST     9
#define NONE     5

/*Cada caballo viene representado por sus valores en los arrays*/
/*En el caso de los pipes cada caballo tiene dos valores en el array*/
typedef struct _infoCaballos {
    int numCaballos;
    int longCarrera;
    int *idCaballos;     //id de los procesos de caballos
    int *valoresTotales; //Valores en carrera
    int *estadoActual;   //estados FIRST, LAST, NONE
    int *pipes;          // Comunicacion monitor con caballos
    int endFlag;         // flag de finalizcion de carrera, si es forzada (SIGINT)
    int buzon;
}  infoCaballos;

//Funcion que ejecutan todos los procesos de caballos
void procesoCaballo(int id, int *pipe, int buzon);

// ZONA DE FUNCION DE INICIALIZACION Y SUS AUXILIARES

int inicializacionInfoArrays(infoCaballos * info, int numC, int longCarrera);
int generaComunicaciones(infoCaballos *info);
int generaProcesos(infoCaballos *info);
void setAllNULL(infoCaballos *info);

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
        free(info->idCaballos);
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
    info->idCaballos     = NULL;
    info->valoresTotales = NULL;
    info->estadoActual   = NULL;
    info->pipes          = NULL;
}

int inicializacionInfoArrays(infoCaballos * info, int numC, int longCarrera){
    int i;
    info->numCaballos    = numC;
    info->longCarrera    = longCarrera;
    info->idCaballos     = (int *) malloc(sizeof(int) * numC);
    info->valoresTotales = (int *) malloc(sizeof(int) * numC);
    info->estadoActual   = (int *) malloc(sizeof(int) * numC);
    info->pipes          = (int *) malloc(sizeof(int) * 2 * numC);
    if (info->idCaballos == NULL || info->valoresTotales == NULL
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
        return FALSE
    }

    return TRUE;
}



int generaProcesos(infoCaballos *info){
    int i, aux, j;
    for (i = 0; i < info->numCaballos; i++) {
        aux = fork();
        if (aux == -1) {
            for (j = 0; j < i; j++)
                kill(info->idCaballos[j], SIGINT);
            return FALSE;
        }else if (aux == 0) {
            procesoCaballo(i + 1, info->pipes + 2 * i, info->buzon);
            return -10; //Aqui nunca se llega, aunque por sia acaso
        }else{
            info->idCaballos[i] = aux;
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

//FALTAN TODAS LAS FUNCIONES DE CABALLOS_H

//
//
/**
 * Proceso que ejecuta cada caballo de forma independiente.
 * @param pipe Para que el monitor se comunique con el.
 * //FALTA COSAS PARA PODER COMUNICAR EL CABALLO CON EL MONITOR.
 * //MANEJADOR PARA CERRAR CUANDO ACABE LA CARRERA
 */
void procesoCaballo(int id, int *pipe, int buzon){
}
