#include "monitor_lib.h"
#include "apuestas.h"

infoCaballos *infoC; /*Variable global para que pueda acceder el manejador*/

/**
 * Manejador de fin de carrera
 */
void manejador(int sig){
    forzarFin(infoC);
}

/**
 * Proceso monitor.
 * Argumentos:
 * - Numero de caballos < 11
 * - longitud de carrera (entero)
 * - Numero de apostadores < 11
 * - Numero de ventanillas
 */
int main(int argc, char **argv){
    int        numCaballos, longCarrera, numApostadores, numVentanillas;
    Ret        *ret;
    MensajeRes *resApuestas;

    if (inicializacionArgs(&numCaballos, &longCarrera, &numApostadores,
                           &numVentanillas, argv, argc) == ERROR) {
        return 0;
    }
    infoC = inicializaCaballos(numCaballos, longCarrera);
    if (infoC == NULL) {
        printf("Error en la inicializacion de caballos.\n");
        return 0;
    }
    /*INICIALIZACION DE APOSTADORES Y GESTOR DE APUESTAS*/
    ret = apuestas(numCaballos, numVentanillas, numApostadores);
    if (ret == NULL) {
        finalizaLibera(infoC);
        return 0;
    }
    esperaImprime();
    /*NOTIFICA A LAS APUESTAS DE QUE SE ACABO EL TIEMPO, RECIBE RESULTADOS*/
    resApuestas = paraApuestas(ret);
   
    printf("Inicia la carrera.\n");
    signal(SIGINT, manejador);
    wrapperCarrera(infoC, resApuestas);


    finalizaLibera(infoC);
    return 1;
}
