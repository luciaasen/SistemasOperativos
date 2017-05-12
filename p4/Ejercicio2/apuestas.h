#ifndef APUESTAS_H
#define APUESTAS_H
#include "gestor.h"
#include "apostador.h"

/*Estructura con valores de colas y pids para que el monitor pueda mandar a callar
   a las apuestas*/
typedef struct _Ret   Ret;

/*Mensaje para decirle al gestor que se calle*/
typedef struct _mens {
    long type;
    char c[1];
}mens;

/**
 * Crea 2 colas de mensajes: una por la que el gestor se comunicara con el main,
 * y otra por la que el gestor se comunicara con los apostadores.
 * Lanza dos procesos, uno que crea apuestas y otro que las gestiona.
 * @param numC num caballos
 * @param numV num ventanillas
 * @param numA num apostadores
 * @return estructura con la informacion de: cola y tipo de mensajes para parar la gestion de apuestas, y los pid a los que esperar
 */
Ret *apuestas(int numC, int numV, int numA);

/**
 * Envia el mensaje de parada con la cola y el tipo de r
 * @param r Estructura obtenida de la funcion apuestas
 * @return 0 si bien, 1 si error
 */
infoApuestas * paraApuestas(Ret *r);



#endif
