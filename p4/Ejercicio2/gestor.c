    #include "apostador.h"
    #include "semaforos.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <pthread.h>
    #include <unistd.h>

    typedef struct _infoApuestas{
        /*dinero[i][j] contiene el dinero que se da al apostador j si gana el caballo i*/
        double[][] dinero;
        /*cotizacion por caballo*/
        double[] cotizacion;
        /*Dinero apostado por cada caballo*/
        double[] apostado;
        /*Total dinero apostado, podriamos sumarlo en cada apuesta pero me parece comodo guardarlo*/
        double total;
    } infoApuestas;

    /*Esructura que se pasa como argumento al thread*/
    typedef struct _Attr{
        int cola;
        int infoMutex;
        infoApuestas *info;
    } Attr;

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
    Attr *attr_ini(int semid, infoApuestas *info);

    /**
     * Ejecuta la accion de cada hilo ventanilla:
     * Reciben y procesan secuencialmente (y sincronizadamente, gracias al mutex) diferentes apuestas.
     * El procesamiento de una encuesta consiste en leerla y actualizar toda la informacion
     * de las apuestas (estructura infoApuestas)
     * @param atributo puntero a Attr, que contiene el mutex y la informacion compartida entre todas las 
     * ventanillas acerca de las apuestas
     * @return -1 si error, 0 else
     */
    int ventanilla(void *atributo);
    
    /**
     * Libera los recursos d una estructura infoApuestas
     * @param info estructura a liberar
     * @param numC numero de caballos en la carrera
     */
    void info_free(infoApuestas *info, int numC){


















    int gestorApuestas(int cola, int numC, int numA, int numV){
        infoApuestas *info;
        Attr* attr;
        int semid, i;
        pthread_t *ventanillas;

        ventanillas = (pthread_t*)malloc(numV * sizeof(pthread_t));
        if(ventanillas == NULL){
            return -1;
        }
        
        info = info_ini(numC, numA);
        if(info == NULL){
            free(ventanillas);
            return -1;
        }

        /*Crea semaforo con las funciones de rodri*/
        /******************************************/
        
        attr = attr_ini(semid, info, cola);
        if(attr == NULL){
            free(ventanillas);
            info_free(info, numC);
            return NULL;
        }

        
        for(i = 0; i < numV; i++){
            if(pthread_create(ventanillas + i, NULL, ventanillas, (void*) attr) != 0){
                printf("Error en la creacion del hilo %d\n", i);
                /*Este free ya libera la estructura info*/
                attr_free(attr, numC);
                /*¿Como liberar correctamente los hilos? (Parte ya se esta ejecutando) Discute rodri*/                
            }
//            pthread_join(ventanillas[i], NULL);
        }
        
        /*No se me ocurre como cerrar bien todo esto (si espero a que me llege una senial, y no tengo variables globales
         no dejaria los recursos liberados)*/       
    }


    int ventanilla(void *atributo){
        Apuesta a;
        Attr *attr; 
        int apostador, caballo, cuantia;
        if(atributo == NULL){
            return -1;
        }
        attr = (Attr *) atributo;

        while(1){
            /*Recibe el mensaje de la apuesta*/
            
            /*********************************/
            

            apostador = getApostador(a);
            caballo = getCaballo(a);
            cuantia = getCuantia(a);

            Down_Semaforo(attr->infoMutex, 0, /*UNDO*/);
            attr->info->dinero[caballo][apostador] += cuantia * attr->info->cotizacion[caballo];
            attr->info->total += cuantia;
            attr->info->apostado[caballo] += cuantia;
            attr->info->cotizacion[caballo] = attr->info->total/attr->info->apostado;
            Up_Semaforo(attr->info;utex, 0, /*UNDO*/);
        }
        /*¿Como son interrumpidas las ventanillas?*/

    }



    Attr *attr_ini(int semid, infoApuestas *info, int cola){
        Attr *attr;

        if(semid < 0 || cola < 0 || info == NULL){
            return NULL;
        }
        attr = (Attr*)malloc(sizeof(Attr));
        if(attr == NULL){
            return NULL;
        }
        attr->infoMutex = semid;
        attr->info = info;
        attr->cola = cola;
        return attr;
    }


    void attr_free(Attr* attr, int numC){
        if(attr != NULL){
            if(attr->info!= NULL){
                info_free(attr->info, numC);
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
        
        info->dinero = (double **)malloc(numC * sizeof(double *));
        if(info->dinero == NULL){
            free(info);
            return NULL;
        }
        for(i = 0; i < numC; i++){
            info->dinero[i] = (double *)malloc(numA*sizeof(double));
            if(info->dinero[i] == NULL){
                for(j = 0; j < i; k++){
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
            for(j = 0; j < numC; j++){
                free(info->apostado[j]);
            }
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

    void info_free(infoApuestas *info, int numC){
        int i;
        if(info != NULL ){
            if(info->apostado != NULL){
                free(info->apostado);
            }
            if(info->cotizacion != NULL){
                free(info->cotizacion);
            }
            if(info->dinero != NULL){
                for(i = 0; i < numC; i++){
                    if(info->dinero[i] != NULL){
                        free(info->dinero[i]);
                    }
                    free(info->dinero);
                }
            }
            free(info);
        }
    }
