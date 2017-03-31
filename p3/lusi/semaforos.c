#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#define ERROR -1
#define OK 0
int Crear_Semaforo(key_t key, int size, int *semid){
    int i; 

    if(semid == NULL || size < 1){
        perror("Invalid arguments.\n");
        return ERROR;
    }

    /*Inicializacion estructura*/
    union semin(){
        int val;
        struct semid_ds *semstat;
        unsigned short *array;
    } arg;
    arg.array = (short *)malloc(size * sizeof(short));
    if(array == NULL){
        perror("Error inicializacion de arg\n");
        return ERROR;
    }
    for(i = 0; i < size; i++){
        arg.array[i] = 0;
    }
    
    /*Creacion*/
    *semid = semget(key, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
    if(*semid == -1 && errno == EEXIST){
        *semid = semget(key, size, SHM_R | SHM_W);
        if(*semid == -1){
            perror("Error creating sem.\n");
            free(arg.array);
            return ERROR;
        }
    }

    /*Inicializacion semaforo*/
    if(semctl(*semid, 0, SETALL, arg) == -1){
        perror("Error inicializacion semaforo.\n");
        free(arg.array);
        return ERROR;
    }    
    free(arg.array);
    if(errno == EEXIST){
        return 1;
    }
    return 0;
}

int Inicializar_Semaforo(int semid, unsigned short *array){
    int semnum;
    if(semid < 0 || array == 0){
        perror("Invalid arguments.\n");
    }

    union semun(){
        int val;
        struct semid_ds *semstat;
        unsigned short *array;
    }arg;
    
    arg.array = array;
    if(semctl(semid, 0, SETALL, arg) == -1){
        perror("Error inicializando semaforo\n");
        return ERROR;
    }
    return OK;
}


