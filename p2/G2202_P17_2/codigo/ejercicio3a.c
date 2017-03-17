/**
* @file ejercicio3a.c
* @author Lucia Asencio y Rodrigo de Pool
* @date 16-3-2017
* @brief fichero que contiene ej3a de la practica 2 de SOPER para el estudio forks vs threads
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#define NDEBUG
#include <assert.h>


typedef struct _MyArg{
    int *lista;
    int num;
}MyArg;

int isPrime(int num, int* lista, int tamanio);
void *nPrimos(void* n);

/**
* Funcion que calcula si un numero es o no primo, cribandolo.
* @param num: numero a comprobar.
* @param lista: array de enteros con todos los primos anteriores a n
* @param tamanio: numero de elementos de lista
* @return 0 si no es primo, 1 en caso contrario
*/
int isPrime(int num, int* lista, int tamanio){
    int i;

    assert(lista != NULL && tamanio >= 0 && num >= 1);
    for (i = 0; i < tamanio && lista[i] <= sqrt(num); i++){
        if(num%lista[i] == 0){
            return 0;
        }
    }
    return 1;    
}

/**
* nPrimos: reserva memoria para una lista de tamanio n->n, que asigna a n->lista y 
* devuelve con los n primeros numeros primos.
* @param n void*, puntero a MyArg, donde n->n > 0 indica el numero de primos a calcular
*/
void *nPrimos(void* n){
    int *lista = NULL;
    int tamanio, comprueba, num;
    MyArg *args = (MyArg *)n; 

    assert(n != NULL && args->num >=1);

    lista = (int*) malloc (args->num * sizeof(int));
    if(lista == NULL){
        return NULL;
    }

    for(tamanio = 0, num = 2;  tamanio < args->num; num ++){
        comprueba = isPrime(num, lista, tamanio);
        if(comprueba == 1){
            lista[tamanio] = num;
            tamanio ++;
        }
    }
    args->lista = lista;
    pthread_exit(NULL);
}

/**
 * Funcion main
 * Genera de manera secuencial 100 hilos, cada uno de los cuales calcula los 10000 numeros primos.
 * Cada hilo se lanzado cuando el anterior completa su ejecucion.
 * Imprime el tiempo total de ejecucion
 * @return -1 en caso de error, 0 en caso de exito.
 */
int main(){
    int i;
    pthread_t threads[100];
    MyArg *args;
    struct timeval tvini, tvend, tvresult;

    gettimeofday(&tvini, NULL);
    args = (MyArg *)malloc(sizeof(MyArg));
    if(args == NULL){
        printf("args reserva fallida\n");
        return -1;
    }
    args->num = 10000;
    for(i = 0; i < 100; i++){
        if( pthread_create(threads + i, NULL, nPrimos, (void*)(args)) != 0){
            printf("Error creando hilo %d\n", i);
            free(args);
            return -1;
        }
        pthread_join(threads[i], NULL);
        if(args->lista == NULL){
            printf("Error ejecutando hilo %d\n", i);
            free(args);
            return -1;
        }        
        free(args->lista);
    }
    gettimeofday(&tvend, NULL);
    timersub(&tvend, &tvini, &tvresult);
    printf("El programa con threads tarda %ld.%ld\n", (long int)tvresult.tv_sec, (long int)tvresult.tv_usec);
    return 0;
}
