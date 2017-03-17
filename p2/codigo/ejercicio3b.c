/**
* @file ejercicio3b.c
* @author Lucia Asencio y Rodrigo de Pool
* @date 16-3-2017
* @brief fichero que contiene ej3b de la practica 2 de SOPER para el estudio forks vs threads
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#define NDEBUG
#include <assert.h>


int isPrime(int num, int* lista, int tamanio);
int *nPrimos(int n);

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
* nPrimos: reserva memoria para una lista de tamanio n, que devuelve con los n primeros numeros
* primos.
* @param n entero > 0, numero de primos a calcular
* @return lista de primos o NULL en caso de error.
*/

int *nPrimos(int n){
    int *lista = NULL;
    int tamanio, comprueba, num;
     
    assert(n >=1);
    
    lista = (int*) malloc (n * sizeof(int));
    if(lista == NULL){
        return NULL;
    }

    for(tamanio = 0, num = 2;  tamanio < n; num ++){
        comprueba = isPrime(num, lista, tamanio);
        if(comprueba == 1){
            lista[tamanio] = num;
            tamanio ++;
        }
    }
    return lista;
}

/**
* Funcion main
* Genera de manera secuencial 100 hijos, cada uno de los cuales calcula los 10000 numeros primos.
* Cada hijo se lanzado cuando el anterior completa su ejecucion.
* Imprime el tiempo total de ejecucion
* @return -1 en caso de error, 0 en caso de exito.
*/
int main(){
    pid_t hijos[100];
    int i, status, exitStatus;
    int *lista;
    struct timeval tvini, tvend, tvresult;
    
    gettimeofday(&tvini, NULL);
    for(i = 0; i < 100; i++){
        hijos[i] = fork();
        if(hijos[i] < 0){
            printf("Error en el fork %d\n", i);
            return -1;
        }else if(hijos[i] == 0){
            lista = nPrimos(10000);
            if(lista == NULL){
                printf("Error en la ejecucion %d de nPrimos\n", i);
                exit(EXIT_FAILURE);
            }
            free(lista);
            exit(EXIT_SUCCESS);
        }else{
            waitpid(hijos[i], &status, 0);
            if(WIFEXITED(status)){
                exitStatus = WEXITSTATUS(status);
                if( exitStatus == EXIT_FAILURE){
                    return -1;
                }
            }

        }
    }
    gettimeofday(&tvend, NULL);
    timersub(&tvend, &tvini, &tvresult);
    printf("El programa con forks tarda %ld.%ld\n", (long int)tvresult.tv_sec, (long int)tvresult.tv_usec);
    return 0;
}
