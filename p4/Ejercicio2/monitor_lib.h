#ifndef MONITOR_LIB_H
#define MONITOR_LIB_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "caballos.h"


#define OK           1
#define ERROR        0
#define MAX_ARGS     10
#define WAIT_TIME    15



/**
 * Funcion que se encarga de la inicializacion de los parametros.
 * Imprime por pantalla en caso de error
 * @param  numCaballos    Puntero a entero donde se guardan el numero de caballos
 * @param  longCarrera    donde se guarda la longitud de la carrera
 * @param  numApostadores donde se guarda el numero de apostadores
 * @param  numVentanillas donde se guarda el numer
 * @param  argv           argumentos de entrada del main
 * @param argc            numero de argumentos de argv
 * @return                OK si no ha habido errores, ERROR si los ha habido
 */
int inicializacionArgs(int *numCaballos, int *longCarrera, int *numApostadores,
                       int *numVentanillas, char **argv, int argc);

/**
 * Funcion que va contando 15 segundos, imprimiendolos por pantalla
 */
void esperaImprime();

/**
 * Funcion que hace de wrapper para el funcionamiento durante la carrera
 * @param infoC puntero a infoCaballos
 */
void wrapperCarrera(infoCaballos *infoC);

#endif