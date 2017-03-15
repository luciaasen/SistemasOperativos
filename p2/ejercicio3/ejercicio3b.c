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

int main(int argc, char **argv){
    pid_t hijos[100];
    int i, status, exitStatus;
    int *lista;
    struct timeval tvini, tvend, tvresult;

    if(argc < 2 || atoi(argv[1]) < 1){
        perror("Error en el argumento de entrada nprimos (debe ser > 0)");
        exit(EXIT_FAILURE);
    }

    gettimeofday(&tvini, NULL);
    for(i = 0; i < 100; i++){
        hijos[i] = fork();
        if(hijos[i] < 0){
            printf("Error en el fork %d\n", i);
            return -1;
        }else if(hijos[i] == 0){
            lista = nPrimos(atoi(argv[1]));
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
    return 1;
}
