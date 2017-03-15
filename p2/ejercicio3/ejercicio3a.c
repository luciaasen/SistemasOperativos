#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#define NDEBUG
#include <assert.h>


typedef struct _MyArg{
    int *lista;
    int num;
}MyArg;

int isPrime(int num, int* lista, int tamanio);
void *nPrimos(void* n);

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
    /*printf("tamanio: %d ", tamanio);*/
    pthread_exit(NULL);
}

int main(int argc, char **argv){
    clock_t begin, end, thini, thend, thsum = 0;
    int i;
    double time;
    pthread_t threads[100];
    MyArg *args;
    
    if(argc < 2 || atoi(argv[1]) < 1){
        perror("Error en el argumento de entrada nprimos (debe ser > 0)");
    }

    begin = clock();
    args = (MyArg *)malloc(sizeof(MyArg));
    if(args == NULL){
        printf("args reserva fallida\n");
        return -1;
    }
    args->num = atoi(argv[1]);
    for(i = 0; i < 100; i++){
        thini = clock();
        if( pthread_create(threads + i, NULL, nPrimos, (void*)(args)) != 0){
            printf("Error creando hilo %d\n", i);
            free(args);
            return -1;
        }
        pthread_join(threads[i], NULL);
        thend = clock();
        printf("Thread %d %ld\n", i, thend-thini);
        thsum += thend - thini;
        if(args->lista == NULL){
            printf("Error ejecutando hilo %d\n", i);
            free(args);
            return -1;
        }        
        free(args->lista);
        /*printf(" hilo %d\n", i);*/
    }
    end = clock();
    time = (end - begin);
    printf("El programa con threads tarda %f, la suma de los threads %ld\n", time, thsum);
    return time;
}
