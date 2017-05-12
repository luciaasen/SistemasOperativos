#include "monitor_lib.h"



int inicializacionArgs(int *numCaballos, int *longCarrera, int *numApostadores,
                       int *numVentanillas, char **argv, int argc){
    if (argc < 5) {
        printf("Numero de argumentos insuficientes.\n");
        return ERROR;
    }

    *numCaballos    = atoi(argv[1]);
    *longCarrera    = atoi(argv[2]);
    *numApostadores = atoi(argv[3]);
    *numVentanillas = atoi(argv[4]);

    if (*numCaballos < 3 || *numCaballos > MAX_ARGS) {
        printf("Solo se aceptan numero de caballos enteros en [3,10].\n");
        return ERROR;
    }else if (*longCarrera < 0) {
        printf("La longitud de la carrera tiene que ser un entero mayor que cero.\n");
        return ERROR;
    }else if (*numApostadores < 0 || *numApostadores > MAX_ARGS) {
        printf("Solo se aceptan numero de apostadores enteros en [1,10].\n");
        return ERROR;
    }else if (*numVentanillas < 0) {
        printf("El numero de ventanillas tiene que ser mayor que cero.\n");
        return ERROR;
    }
    return OK;
}




void esperaImprime(){
    int i;
    for (i = 0; i < WAIT_TIME; i++) {
        sleep(1);
        printf("Quedan %d segundos para que inicie la carrera.\n", (WAIT_TIME - i));
    }
}





void wrapperCarrera(infoCaballos *infoC, infoApuestas *resApuestas){
    while (hayGanador(infoC) == FALSE) {
        sleep(1); /*comentar esta linea para eliminar delay*/
        siguienteRonda(infoC);
        actualizaRonda(infoC);
        imprimeRonda(infoC);
        imprimeApuestas(resApuestas);
    }
    imprimeResultados(infoC);
    /*IMPRIME LOS RESULTADOS DE LA CARRERA*/
    imprimeResApuestas(resApuestas, idPrimerLugar(infoC), idSegundoLugar(infoC), idTercerLugar(infoC));
}
