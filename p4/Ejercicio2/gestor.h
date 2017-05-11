    #include "apostador.h"
    #include "semaforos.h"
    #include "gestor.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <pthread.h>
    #include <unistd.h>
    #include <sys/msg.h>
    
    typedef struct _infoApuestas infoApuestas;
    typedef struct _MensajeRes MensajeRes;

    /**
     * Espero que en algun momento antes del viernes, esto gestione apuestas
     * Inicialize la estructura con la informacion de todas las apuesats,
     * y se la pase, junto con una cola de la que leer mensajes y un semaforo con el que proteger a la estructura,
     * a un monton de threads ventanillas que gestionen las diferentes apuestas que lean de la cola de mensajes
     * @param cola de mensajes
     * @param numC caballos en la carrera
     * @param numA apostadores en la carrera
     * @param numV ventanillas en la carrera
     * @return -1 si error, 0 si ok
     */
    int gestorApuestas(int colaApuesta, int colaMain, int numC, int numA, int numV);

    /**
     * Imprime cotizaciones
     */
    int imprimeApuestas(MensajeRes *r); 
