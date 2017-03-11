#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>

#define NDEBUG

int isPrime(int num, int* lista, int tamanio);
int *nPrimos(int n);

int isPrime(int num, int* lista, int tamanio){
    int i;

    assert(lista != NULL && tamanio >= 0 && num > 1);
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
     
    assert(n >1);
    
    lista = (int*) malloc (n * sizeof(int));
    if(lista == NULL){
        return NULL;
    }

    for(tamanio = 0, num = 2;  tamanio < n; num ++){
        comprueba = isPrime(num, lista, tamanio);
        //printf("Comprueba: %d %d\n", num, comprueba);
        if(comprueba == 1){
            lista[tamanio] = num;
            tamanio ++;
        }
    }
    printf("tamanio %d ", tamanio);
    return lista;
}

int main(){
    pid_t hijos[100];
    int i, status, exitStatus;
    int *lista;
    time_t begin, end;
    double time;

    begin = clock();
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
            printf(" hijo %d\n", i);
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
    end = clock();
    time = (end - begin);
    printf("El programa con forks tarda %f\n", time);
    return time;
}
