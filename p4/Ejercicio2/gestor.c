    #include "apostador.h"
    #include "apuestas.h"
    #include "semaforos.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <pthread.h>
    #include <unistd.h>
    #include <sys/msg.h>

    #include "gestor.h"

    #define SEM_TIPO    37


/*Esructura que se pasa como argumento al thread*/
typedef struct _Attr {
    int          cola;
    infoApuestas *info;
} Attr;

struct msgbuf {
    long mtype;
    char mtext[1];
};


/**
 * Reserva memoria para una estructura y la inicializa.
 * Apostado a cada caballo: 1
 * Cotizacion: numC
 * Dinero [i][j] = 0
 * @param numC numero de caballos de la carrera
 * @param numA numero de apostadores de la carrera
 * @return puntero a la estuctura o NULL en caso de error
 */
infoApuestas *info_ini(long id, int numC, int numA);

/**
 * Reserva memoria para una estructura Attr, y la inicializa con los campos indicados
 * @param semid id del mutex
 * @param info puntero al infoApuestas indicado
 * @return NULL si error, puntero a Attr si OK
 */
Attr *attr_ini(infoApuestas *info, int cola);


/**
 * Libera la estructura attr
 * @param attr la estructura a liberar
 */
void attr_free(Attr* attr);

/**
 * Ejecuta la accion de cada hilo ventanilla:
 * Reciben y procesan secuencialmente (y sincronizadamente) diferentes apuestas.
 * El procesamiento de una encuesta consiste en leerla y actualizar toda la informacion
 * de las apuestas (estructura infoApuestas)
 * @param atributo puntero a Attr, con el mutex y la informacion compartida entre todas las
 * ventanillas acerca de las apuestas
 * @return -1 si error, 0 else
 */
void* ventanilla(void *atributo);

/**
 * Libera los recursos d una estructura infoApuestas
 * @param info estructura a liberar
 */
void info_free(infoApuestas *info);


infoApuestas *gestorApuestas(int colaApuesta, int colaMain, int tipo, int numC, int numA, int numV){
    infoApuestas info;
    Attr         * attr;
    int          i, j;
    pthread_t    *ventanillas;
    mens         recibido, semaforo;

    /*Crea semaforos, reserva ventanillas,crea infoApuestas y Argumento para los threads*/
    /***********************************************************************************/
    ventanillas = (pthread_t *) malloc(numV * sizeof(pthread_t));
    if (ventanillas == NULL) {
        return NULL;
    }

    /*Inicializacion del info*/
    info.numC  = numC;
    info.numA  = numA;
    info.id    = tipo;
    info.total = numC;
    for (i = 0; i < numC; i++) {
        for (j = 0; j < numA; j++) {
            info.dinero[i][j] = 0;
        }
        info.cotizacion[i] = numC;
        info.apostado[i]   = 1;
    }
    /*Fin de inicializacion*/


    attr = attr_ini(&info, colaApuesta);
    if (attr == NULL) {
        free(ventanillas);
        return NULL;
    }

    /*crear + enviar mensaje semtpo */
    semaforo.type = SEM_TIPO;
    semaforo.c[0] = 'a';
    msgsnd(colaApuesta, &semaforo, sizeof(mens) - sizeof(long), IPC_NOWAIT);

    /*Lanza hilos*/
    /*************/
    for (i = 0; i < numV; i++) {
        if (pthread_create(ventanillas + i, NULL, ventanilla, (void *) attr) != 0) {
            printf("Error en la creacion del hilo %d\n", i);
            attr_free(attr);
            for (j = 0; j < i; j++) {
                pthread_cancel(ventanillas[j]);
            }
            free(ventanillas);
            return NULL;
        }
    }

    /*Espero hasta que recibo mensaje del main, cierro libero*/
    /*********************************************************/
    printf("La funcion gestor apuestas intenta recibir de cola %d tipo %d, pid %d\n", colaMain, tipo, getpid());

    if (msgrcv(colaMain, (struct msgbuf *) &recibido, sizeof(mens) - sizeof(long), STOP_TIPO, 0) == -1) {
        perror("Error en la recepcion de mensaje de parada\n");
        return NULL;
    }
    printf("recibeeeeeeeeeeee\n");
    for (i = 0; i < numV; i++) {
        pthread_cancel(ventanillas[i]);
    }
    free(ventanillas);

    /*Envio mensaje al main con la info de las apuestas*/
    /***************************************************/
    info.id = RESULTADO_TIPO;
    msgsnd(colaMain, (struct msgbuf *)&info, sizeof(infoApuestas) - sizeof(long), IPC_NOWAIT);
    free(attr);     /*Solo libero el puntero, el infoApuestas de dentro que envia el mensaje no, vd?*/
    exit(0);
    return attr->info;
}


void * ventanilla(void *atributo){
    Apuesta a;
    Attr    *attr;
    int     apostador, caballo;
    double  cuantia;
    mens    semaforo;
    if (atributo == NULL) {
        return NULL;
    }
    attr = (Attr *) atributo;

    /*Recibe mensaje bloqueante - toca la estructura protegiendola - repeat*/
    /***********************************************************************/
    while (1) {
        msgrcv(attr->cola, (struct msgbuf *) &a, sizeof(Apuesta) - sizeof(long), attr->info->id, 0);
        apostador = getApostador(&a);
        caballo   = getCaballo(&a);
        cuantia   = getCuantia(&a);

        msgrcv(attr->cola, &semaforo, sizeof(semaforo) - sizeof(long), SEM_TIPO, 0);
        attr->info->dinero[caballo][apostador] += cuantia * attr->info->cotizacion[caballo];
        attr->info->total                      += cuantia;
        attr->info->apostado[caballo]          += cuantia;
        attr->info->cotizacion[caballo]         = attr->info->total / attr->info->apostado[caballo];
        msgsnd(attr->cola, &semaforo, sizeof(mens) - sizeof(long), IPC_NOWAIT);
    }
    return NULL;
}

int imprimeApuestas(infoApuestas *r){
    int    i;
    double *cotizaciones;
    if (r == NULL) {
        return -1;
    }
    /*Imprime todas las cotizaciones*/
    /********************************/
    cotizaciones = r->cotizacion;
    printf("Cotizaciones:\n");
    if (r != NULL) {
        printf("numcaballos %d\n", r->numC);
    }
    for (i = 0; i < r->numC; i++) {
        printf("\tCaballo %d -> cotizacion %lf\n", i + 1, cotizaciones[i]);
    }
    return 0;
}

int imprimeResApuestas(infoApuestas *r, int prim, int sec, int terc){
    int    i;
    double ganancia;
    if (r == NULL || prim < 0 || sec < 0 || terc < 0) {
        return -1;
    }

    /*Imprime ganancia de cada apostador*/
    /************************************/
    printf("Ganancia apostadores:\n");
    for (i = 0; i < r->numA; i++) {
        ganancia = r->dinero[prim - 1][i] + r->dinero[sec - 1][i] + r->dinero[terc - 1][i];
        if (ganancia > 0) {
            printf("\tApostador %d -> ganancia %lf\n", i, ganancia);
        }
    }
    return 0;
}

Attr *attr_ini(infoApuestas *info, int cola){
    Attr *attr;

    if (cola < 0 || info == NULL) {
        return NULL;
    }
    attr = (Attr *) malloc(sizeof(Attr));
    if (attr == NULL) {
        return NULL;
    }
    attr->info = info;
    attr->cola = cola;
    return attr;
}


void attr_free(Attr* attr){
    if (attr != NULL) {
        if (attr->info != NULL) {
            info_free(attr->info);
        }
        free(attr);
    }
}



void info_free(infoApuestas *info){
    if (info != NULL) {
        free(info);
    }
}









/*Esta funcion ocupa muchisimo por culpa de hacer mil millones de mallocs. No hace nada complicado*/
/* infoApuestas *info_ini(int numC, int numA){
     infoApuestas *info;
     int i, j;

     if(numC < 1 || numA < 1){
         return NULL;
     }

     info = (infoApuestas *)malloc(sizeof(infoApuestas));
     if(info == NULL){
         return NULL;
     }
     info->numC = numC;
     info->numA = numA;
     info->dinero = (double **)malloc(numC * sizeof(double *));
     if(info->dinero == NULL){
         free(info);
         return NULL;
     }
     for(i = 0; i < numC; i++){
         info->dinero[i] = (double *)malloc(numA*sizeof(double));
         if(info->dinero[i] == NULL){
             for(j = 0; j < i; j++){
                 free(info->dinero[j]);
             }
             free(info->dinero);
             free(info);
             return NULL;
         }
     }

     info->apostado = (double *)malloc(numC*sizeof(double));
     if(info->apostado == NULL){
         for(j = 0; j < numC; j++){
             free(info->dinero[j]);
         }
         free(info->dinero);
         free(info);
         return NULL;
     }

     info->cotizacion = (double *)malloc(numC * sizeof(double));
     if(info->cotizacion == NULL){
         for(j = 0; j < numC; j++){
             free(info->dinero[i]);
         }
         free(info->dinero);

         free(info->apostado);
         free(info);
         return NULL;
     }

     info->total = 0;
     for(i = 0; i < numC; i++){
         for(j = 0; j < numA; j++){
             info->dinero[i][j] = 0;
         }
         info->cotizacion[i] = numC;
         info->apostado[i] = 1;
     }
     info->total = numC;
     printf("En info ini numC %d\n", numC);

     return info;
   }

   void info_free(infoApuestas *info){
     int i;
     if(info != NULL ){
         if(info->apostado != NULL){
             free(info->apostado);
         }
         if(info->cotizacion != NULL){
             free(info->cotizacion);
         }
         if(info->dinero != NULL){
             for(i = 0; i < info->numC; i++){
                 if(info->dinero[i] != NULL){
                     free(info->dinero[i]);
                 }
                 free(info->dinero);
             }
         }
         free(info);
     }
   }*/
