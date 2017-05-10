#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define N 1997
#define F "/bin/cat"
#define MTYPE 1

typedef struct _Apuesta Apuesta;

/**
 * Crea apuesta aleatoria
 * @param idApostador numero del apostador
 * @param nCaballos caballos en la carrera
 * @return NULL si error, si no la apuesta creada.
 */
Apuesta apuesta_ini(int idApostador, int nCaballos);     

/**
 * Funcion que se encarga de generar cada 0.1 segundos apuestas aleatorias
 * y enviarlas a una cola de mensajes que lee el gestor
 * @param nApostadores numero de apostadores
 * @param nCaballos numero de caballos
 */
int generador(int nApostadores, int nCaballos);

/**
 * @param a Apuesta
 * @return id del caballo
 */
int getCaballo(Apuesta *a);
 
/**
 * @param a Apuesta
 * @return id del apostador
 */
int getApostador(Apuesta *a);

/**
 * @param a Apuesta
 * @return cuantia de la apuesta
 */
int getCuantia(Apuesta *a);
