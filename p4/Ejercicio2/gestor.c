    #include "apostador.h"
    #include "apuestas.h"
    #include "semaforos.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <pthread.h>
    #include <unistd.h>
    #include <sys/msg.h>
    
    #include "gestor.h"

    struct _infoApuestas{
        /*dinero[i][j] contiene el dinero que se da al apostador j si gana el caballo i*/
        double** dinero;
        /*cotizacion por caballo*/
        double* cotizacion;
        /*Dinero apostado por cada caballo*/
        double* apostado;
        /*Total dinero apostado, no necesaria pero si comoda*/
        double total;
        int numC;
        int numA;
    };

    /*Esructura que se pasa como argumento al thread*/
    typedef struct _Attr{
        int cola;
        int tipo;
        int infoMutex;
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
    infoApuestas *info_ini(int numC, int numA);

    /**
     * Reserva memoria para una estructura Attr, y la inicializa con los campos indicados
     * @param semid id del mutex 
     * @param info puntero al infoApuestas indicado
     * @return NULL si error, puntero a Attr si OK
     */
    Attr *attr_ini(int semid, infoApuestas *info, int cola, int tipo);

    
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
        infoApuestas *info;
        Attr* attr;
        int semid, key, i, j;
        pthread_t *ventanillas;
        MensajeRes mensaje;
        mens recibido;
        
        /*Crea semaforos, reserva ventanillas,crea infoApuestas y Argumento para los threads*/
        /***********************************************************************************/
        ventanillas = (pthread_t*)malloc(numV * sizeof(pthread_t));
        if(ventanillas == NULL){
            return NULL;
        }
        
        info = info_ini(numC, numA);
        if(info == NULL){
            free(ventanillas);
            return NULL;
        }
            
        srand(time(NULL) * getpid());
        key = rand();
        if(Crear_Semaforo(key, 1, &semid) == ERROR){
            info_free(info);
            free(ventanillas);
            return NULL;
        }
        
        attr= attr_ini(semid, info, colaApuesta, tipo);
        if(attr == NULL){
            free(ventanillas);
            Borrar_Semaforo(semid);
            info_free(info);
            return NULL;
        }
        
        /*Lanza hilos*/
        /*************/
        for(i = 0; i < numV; i++){
            if(pthread_create(ventanillas + i, NULL, ventanillas, (void*) attr) != 0){
                printf("Error en la creacion del hilo %d\n", i);
                attr_free(attr);            
                for(j = 0; j < i; j++){
                    pthread_cancel(ventanillas[j]);
                }
                free(ventanillas);
                Borrar_Semaforo(semid);
                info_free(info);
                return NULL;
            }
        }

        /*Espero hasta que recibo mensaje del main, cierro libero*/ 
        /*************************************************************************************/
        msgrcv(colaMain, (struct msgbuf *)&recibido, sizeof(mens) - sizeof(long), tipo, 0);
        for(i = 0; i < numV; i++){
            pthread_cancel(ventanillas[i]);
        }
        free(ventanillas);
        Borrar_Semaforo(semid);              
          
        /*Envio mensaje al main con la info de las apuestas*/
        /***************************************************/
        mensaje.info = attr->info;
        mensaje.id = tipo;
        msgsnd(colaMain, (struct msgbuf *)&mensaje, sizeof(MensajeRes)-sizeof(long), IPC_NOWAIT);
        free(attr); /*Solo libero el puntero, el infoApuestas de dentro que envia el mensaje no, vd?*/
        return attr->info;
    }


    void * ventanilla(void *atributo){
        Apuesta *a;
        Attr *attr; 
        int apostador, caballo;
        double cuantia;
        Mensaje mensaje;
        if(atributo == NULL){
            return NULL;
        }
        attr = (Attr *) atributo;
        
        /*Recibe mensaje bloqueante - toca la estructura protegiendola - repeat*/
        /***********************************************************************/
        while(1){
            msgrcv(attr->cola, (struct msgbuf *)&mensaje, sizeof(Mensaje) - sizeof(long), attr->tipo, 0);
            a = getApuesta(mensaje);            

            apostador = getApostador(a);
            caballo = getCaballo(a);
            cuantia = getCuantia(a);

            Down_Semaforo(attr->infoMutex, 0, 0);
            attr->info->dinero[caballo][apostador] += cuantia * attr->info->cotizacion[caballo];
            attr->info->total += cuantia;
            attr->info->apostado[caballo] += cuantia;
            attr->info->cotizacion[caballo] = attr->info->total/attr->info->apostado[caballo];
            Up_Semaforo(attr->infoMutex, 0, 0);
        }
        return NULL;
    }

    int imprimeApuestas(MensajeRes *r){
        int i;
        double *cotizaciones;
        if(r == NULL){
            return -1;
        }        
        /*Imprime todas las cotizaciones*/
        /********************************/
        cotizaciones = r->info->cotizacion;
        printf("Cotizaciones:\n");
        for(i = 0; i < r->info->numC; i++){
            printf("\tCaballo %d -> cotizacion %lf\n", i+1, cotizaciones[i]);
        }
        return 0;
    }
    
    int imprimeResApuestas(MensajeRes *r, int prim, int sec, int terc){
        int i;
        double ganancia;
        if(r == NULL || prim < 0 || sec < 0 || terc < 0){
            return -1;
        }
        
        /*Imprime ganancia de cada apostador*/
        /************************************/
        printf("Ganancia apostadores:\n");
        for(i = 0; i < r->info->numA; i++){
            ganancia = r->info->dinero[prim-1][i] + r->info->dinero[sec-1][i] + r->info->dinero[terc-1][i];
            if(ganancia > 0){
                printf("\tApostador %d -> ganancia %lf\n", i, ganancia);
            }
        }
        return 0;        
    }

    Attr *attr_ini(int semid, infoApuestas *info, int cola, int tipo){
        Attr *attr;

        if(semid < 0 || cola < 0 || tipo < 0 || info == NULL){
            return NULL;
        }
        attr = (Attr*)malloc(sizeof(Attr));
        if(attr == NULL){
            return NULL;
        }
        attr->infoMutex = semid;
        attr->info = info;
        attr->cola = cola;
        attr->tipo = tipo;
        return attr;
    }


    void attr_free(Attr* attr){
        if(attr != NULL){
            if(attr->info!= NULL){
                info_free(attr->info);
            }
            free(attr);
        }
    }

    /*Esta funcion ocupa muchisimo por culpa de hacer mil millones de mallocs. No hace nada complicado*/
    infoApuestas *info_ini(int numC, int numA){
        infoApuestas *info;
        int i, j;

        if(numC < 1 || numA < 1){
            return NULL;
        }
        /*Reservamos catorcemil cosas*/

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
        
        /*Inicializamos catorcmil cosas*/
        info->total = 0;
        for(i = 0; i < numC; i++){
            for(j = 0; j < numA; j++){
                info->dinero[i][j] = 0;
            }
            info->cotizacion[i] = numC;
            info->apostado[i] = 1;
        }
        info->total = numC;

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
    }
