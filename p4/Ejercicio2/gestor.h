#ifndef GESTOR_H
#define GESTOR_H

typedef struct _infoApuestas{
    /*Tipo del mensaje que recogera el main*/
    long id;
    /*dinero[i][j] contiene el dinero que se da al apostador j si gana el caballo i*/
    double dinero[10][10];
    /*cotizacion por caballo*/
    double cotizacion[10];
    /*Dinero apostado por cada caballo*/
    double apostado[10];
    /*Total dinero apostado, no necesaria pero si comoda*/
    double total;
    int numC;
    int numA;
}infoApuestas;

/**
 * Espero que en algun momento antes del viernes, esto gestione apuestas
 * Inicialize la estructura con la informacion de todas las apuesats,
 * y se la pase, junto con una cola de la que leer mensajes y un semaforo con el que proteger a la estructura,
 * a un monton de threads ventanillas que gestionen las diferentes apuestas que lean de la cola de mensajes
 * @param cola de mensajes
 * @param numC caballos en la carrera
 * @param numA apostadores en la carrera
 * @param numV ventanillas en la carrera
 * @param colaApuesta cola para envia laas apuestas al gestor desde los apostadores
 * @param colaMain cola por la que el monitor manda a callar al gestor y gestor dara resultados
 * @param tipo tipo de mensaje a usar
 * @return  estructura con la info de las apuestas
 */
infoApuestas * gestorApuestas(int colaApuesta, int colaMain, int tipo, int numC, int numA, int numV);

/**
 * Imprime cotizaciones de todos los caballos
 * @param r Mensaje enviado por el gestor al monitor al finalizar las apuestas
 * @return 0 si fue bien, -1 si error
 */
int imprimeApuestas(infoApuestas *r);

/**
 * Imprime ganancias de los apostadores dados los caballos primero, segundo y tercero
 * @param r Mensaje enviado al main por el gestor
 * @param prim caballo ganador - ASUMO QUE EMPIEZAN EN CERO
 * @param sec caballo segundo - ASUMO QUE EMPIEZAN EN CERO
 * @param terc caballo tercero - ASUMO QUE EMPIEZAN EN CERO
 * @return 0 si fue bien, -1 si error
 */
int imprimeResApuestas(infoApuestas *r, int prim, int sec, int terc);
#endif
