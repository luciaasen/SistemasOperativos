#include "apostador.h"
#include "gestor.h"
#include <sys/msg.h>
#include <stdio.h>

typedef struct _Ret ret;
typedef struct _mens mens;

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
int paraApuestas(Ret *r);



