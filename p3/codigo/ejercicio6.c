#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#define KEY 1
#define FILEKEY "./"

typedef struct _AlphaQueue{
    char[26] alpha;
    int ini;
    int end;
} AlphaQueue;


consumidor(){
    /*creacion shmem*/
    
    /*semaforos necesarios*/

    /*bucle*/

}

/**
 * Tras crear las zonas de memoria compartida/ semaforos necesarios, este  proceso produce
 * una cola de letras mientras no este disponible todo el abecedario
 * @return -1 en caso de error
 */
int productor(){
    /*creacion + atach shmem*/
    int id;
    AlphaQueue q;
    if(id = crear_shmem(FILEKEY, KEY, sizeof(AlphaQueue), (void **)(&&q)) == -1){
        return -1;
    }
    /*semaforos necesarios*/
    

    /*bucle*/
}

int producir(){
    /*creacion + atach shmem*/
    int id;
    AlphaQueue q;
    if(id = crear_shmem(FILEKEY, KEY, sizeof(AlphaQueue), (void **)(&&q)) == -1){
        return -1;
    }
    /*semaforos necesarios*/
    
    
}

/**
 * Solicita zona de memoria compartida, primero con IPC_CREAT | IPC_EXCL y, si ya existe,
 * de nuevo sin estas banderas. Also attaches the result to a struct AlphaQueue
 * @param filekey, key : parametros identicos a la funcion ftok
 * @param size: int > 0, tamanio de la memoria a compartir
 * @return identifier for the shared mmemory, -1 in case of error
*/
int crear_shmem(char *filekey, int key, int size, void **att){
    int k, id;
    if(filekey == NULL || size < 1 ){
        perror("Error en los parametros para crear_shmem\n");
        return -1;
    }   
    if(k = ftok(filekey, key) == -1){
        perror("Error en fok desde crear_shmem\n");
        return -1;
    }
    if((id = shmget(key, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W) == -1) && errno == EEXIST){
        if( id = shmget(k, size, SHM_R | SHM_W) == 1){
            perror("Error en shmget");
            return -1
        }
    }
    if(*att = shmat(id, (void *)0, 0) == -1){
        perror("Error en el attach de crear_shmem\n");
        shmdt(*att);
        shmctl(id, IPC_RMID, (struct shmid_ds *)NULL);
        return -1;
    }
    return id;
}


int crear_sems(){
    
}
