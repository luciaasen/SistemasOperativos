#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "semaforos.h"

#define KEY 1
#define KEY2 2
#define FILEKEY "/"

typedef struct {
    char alpha[27];
    int end;
} AlphaStack;

/************************/
/*Cabeceras de funciones*/
/************************/


/**
 * Solicita zona de memoria compartida, primero con IPC_CREAT | IPC_EXCL y, si ya existe,
 * de nuevo sin estas banderas. Also attaches the result to a struct AlphaQueue
 * @param filekey, key : parametros identicos a la funcion ftok
 * @param size: int > 0, tamanio de la memoria a compartir
 * @return identifier for the shared mmemory, -1 in case of error
*/
int crear_shmem(char *filekey, int key, int size, void **att);

/**
 * Solicita los 3 sems necesarios para el problema productor/consumidor
 * Devuelve el identificador del semaforo en *mutex, *lleno, *vacio
 * Los semaforos son inicializados: mutex a 1, lleno a 0, vacio a 26
 * @param filekey: parametro para generar la clave con ftok. La key cambia para cada semaforo
 * @param mutex, lleno, vacio: punteros a los ints donde se guarda los sems id
 * @return 0 si semaforos creados, -1 en caso de error (libera los semaforos ya creados si error)
 */
int crear_sems(char *filekey, int *mutex, int *lleno, int *vacio);

/**
 * Sirve para generalizar la enttrada a la seccion critica por parte de ambos procesos, ya que los dos hacen
 * en este orden: down(sem1), down(sem2), up(sem3), y lo que varia son los semaforos sobre los que operan
 * @param sem1: puntero primer semaforo, al que se hara down
 * @param sem2: puntero segundo semaforo, al que se hara down (este deberia ser el mutex)
 * @param sem3: puntero tercer semaforo, al que se hara up
 * @return -1 si error (libera los semaforos en caso de que el error no fuera en argumentos entrada), else 0
 */
int in_critica(int *sem1, int *sem2, int *sem3);

/**
 * Tras crear los semaforos necesarios, esta funcion produce
 * una cola de letras mientras no este disponible todo el abecedario
 * En caso de error, hara detach de la memoria pero no la borrara
 * @param alpha: puntero a la estructura compartida
 * @param mutex, lleno, vacio: punteros a los semids
 * @return -1 en caso de error, else 0.
 */
int productor(AlphaStack *alpha, int *mutex, int *lleno, int *vacio);

/**
 * Tras crear los semaforos necesarios, el proceso que ejecuta esta funcion
 * consume, mientras haya productos disponibles, las letras del abecedario
 * En caso de error, hara detach de la memoria pero no la borrara
 * @param alpha: puntero a la estructura compartida
 * @param mutex, lleno, vacio: punteros a los semids
 * @return -1 en caso de error, else 0
 */
int consumidor(AlphaStack *alpha, int *mutex, int *lleno, int *vacio);



/*************************/
/*Definicion de funciones*/
/*************************/

int consumidor(AlphaStack *alpha, int *mutex, int *lleno, int *vacio){ 
    if(alpha == NULL || mutex == NULL || lleno == NULL || vacio == NULL){
        return -1;
    }
    
    /*bucle*/
    while(1){
        if(in_critica(lleno, mutex, vacio) == -1){
            shmdt(alpha);
            return -1;
        }

        /*Consume*/
        printf("El consumidor consume %c\n", alpha->alpha[alpha->end]);
        fflush(stdout);
        alpha->end--;

        if(Up_Semaforo(*mutex, 0, 0) == -1){
            shmdt(alpha);
            return -1;            
        }
    }
    return 0;
}

int productor(AlphaStack *alpha, int *mutex, int *lleno, int *vacio){
    if(alpha == NULL || mutex == NULL || lleno == NULL || vacio == NULL){
        return -1;
    }

    /*bucle*/
    while(1){
        if(in_critica(vacio, mutex, lleno) == -1){
            shmdt(alpha);
            return -1;
        }

        /*Produce*/
        alpha->end ++;
        printf("El productor ha producido %c\n", alpha->alpha[alpha->end]);
        fflush(stdout);

        if(Up_Semaforo(*mutex, 0, 0) == -1){
            shmdt(alpha);
            return -1;            
        }
    }
    return 0;
}

int crear_shmem(char *filekey, int key, int size, void **att){
    int k, id;
    if(filekey == NULL || size < 1 ){
        perror("Error en los parametros para crear_shmem\n");
        return -1;
    }   
    if(k = ftok(filekey, 136) == -1){
        perror("Error en fok desde crear_shmem\n");
        return -1;
    }
    id = shmget(352532, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W); 
    if( id == -1){
        if(errno == EEXIST){
	    printf("EEXIST\n");
            id = shmget(352532, size, SHM_R | SHM_W);
            if(id == -1){
                perror("Error en shmget");
                return -1;
            }            
        }else{
            perror("Error2 en el shmget\n");
            return -1;
        }
    }
    printf("%d\n", id);
    if(shmat(id, (void *)NULL, 0) == ((void*)-1)){
       printf("Papaya\n");
    }
    if((*((AlphaStack**)att) = (AlphaStack *)shmat(id, (void *)NULL, 0)) == ((void*)-1)){
       /* if(*att == NULL){
            perror("shmat devolvio NULL\n");
            printf("id %d\n", id);
        }*/
        if(errno == EACCES){
            perror("EACCESS\n");
        }
        if(errno == EIDRM){
            perror("EIDRM\n");
        }
        if(errno == EINVAL){
            perror("EINVAL\n");
        }
        if(errno == ENOMEM){
            perror("ENOMEM\n");
        }
        if(errno == EINVAL){
            perror("EINVAL\n");
        }
        
        perror("Error en el attach de crear_shmem\n");
        shmctl(id, IPC_RMID, (struct shmid_ds *)NULL);
        return -1;
    }
    return id;
}

int crear_sems(char *filekey, int *mutex, int *lleno, int *vacio){
    int ret, k1, k2, k3;
    unsigned short array[1];
    if(filekey == NULL || lleno == NULL || vacio == NULL || filekey == NULL){
        perror("Error en los argumentos de entrada a crear_sems\n");
        return -1;
    }
    if( (k1 = ftok(filekey, 2) == -1) || (k2 = ftok(filekey, 3) == -1) || (k3 = ftok(filekey, 4) == -1) ){
        perror("Error en el ftok desde crear_sems\n");
        return -1;
    }

    /*Debemos inicializar los semaforos, ademas de crearlos*/
    ret = Crear_Semaforo(k1, 1, mutex); 
    if((ret) == -1){
        perror("Error en la creacion de semaforo mutex\n");
        return -1;
    } else if(ret == 0){
        perror("El semaforo mutex ya existia\n");
        return -1;
    } else{
        array[0] = 1;
        Inicializar_Semaforo(*mutex, array);
    }
   
    ret = Crear_Semaforo(k2, 1, vacio);
    if((ret) == -1){
        perror("Error en la creacion de semaforo vacio\n");
        Borrar_Semaforo(*mutex);
        return -1;
    } else if(ret == 0){
        perror("El semaforo vacio ya existia\n");
        Borrar_Semaforo(*mutex);
        return -1;
    } else{
        array[0] = 26;
        Inicializar_Semaforo(*vacio, array);
    }

    ret = Crear_Semaforo(k3, 1, lleno);
    if((ret) == -1){
        perror("Error en la creacion de semaforo lleno\n");
        Borrar_Semaforo(*mutex);
        Borrar_Semaforo(*vacio);
        return -1;
    } else if(ret == 0){
        perror("El semaforo lleno ya existia");
        Borrar_Semaforo(*mutex);
        Borrar_Semaforo(*vacio);
        return -1;
    }
    return 0;
}

int in_critica(int *sem1, int *sem2, int *sem3){
    if(sem1 == NULL || sem2 == NULL || sem3 == NULL){
       perror("Error en los argumentos de entrada a in_critica\n");
       return -1;
    }
    if( Down_Semaforo(*sem1, 0, 0) == -1){
        printf("Error en las operaciones 1 de in_critica %d\n", getpid());
        Borrar_Semaforo(*sem1);
        Borrar_Semaforo(*sem2);
        Borrar_Semaforo(*sem3);
        return -1;
    }
    if( Down_Semaforo(*sem2, 0, 0) == -1){
        printf("Error en las operaciones 2 de in_critica %d\n", getpid());
        Borrar_Semaforo(*sem1);
        Borrar_Semaforo(*sem2);
        Borrar_Semaforo(*sem3);
        return -1;
    }
    if( Up_Semaforo(*sem3, 0, 0) == -1){
        printf("Error en las operaciones 3 de in_critica %d\n", getpid());
        Borrar_Semaforo(*sem1);
        Borrar_Semaforo(*sem2);
        Borrar_Semaforo(*sem3);
        return -1;
    }
    return 0;
}

/*int out_critica(int sem1, int sem2){
    if(sem1 < 0 || sem2 < 0){
       perror("Error en los argumentos de entrada a out_critica\n");
       return -1;
    }
    if( Up_Semaforo(sem1, 0, SEM_UNDO) == -1){
        perror("Error en las operaciones de in_critica\n");
        Borrar_Semaforo(sem1);
        Borrar_Semaforo(sem2);
        return -1;
    }
    if( Up_Semaforo(sem2, 0, SEM_UNDO) == -1){
        perror("Error en las operaciones de in_critica\n");
        Borrar_Semaforo(sem1);
        Borrar_Semaforo(sem2;)
        return -1;
    }
    return 0;
}*/

int main(){
    int id, mutex, lleno, vacio, pid, ret, status;
    AlphaStack q, *pq;
    char myalpha[27] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '\0'};

    /*creacion + atach shmem*/
    pq = &q;
    if( (id = crear_shmem(FILEKEY, KEY, sizeof(AlphaStack), (void **)(&pq)) )   == -1){
        return -1;
    }
    strcpy(q.alpha, myalpha);
    q.end = -1;
   
    /*semaforos necesarios*/
    if( (crear_sems(FILEKEY, &mutex, &lleno, &vacio)) == -1){
        shmdt(&q);
        shmctl(id, IPC_RMID, (struct shmid_ds *)NULL);
        return -1;
    }    

    /*Ahora creamos otro proceso, para que uno se encargue de consumir, y el otro de producir*/
    if((pid = fork()) == -1){
        perror("Error en el fork\n");
        shmdt(&q);
        shmctl(id, IPC_RMID,(struct shmid_ds *)NULL);
        Borrar_Semaforo(mutex);
        Borrar_Semaforo(lleno);
        Borrar_Semaforo(vacio);
        return -1;
    }else if( pid == 0){
        ret = consumidor(&q, &mutex, &lleno, &vacio);
        Borrar_Semaforo(mutex);
        Borrar_Semaforo(lleno);
        Borrar_Semaforo(vacio);
        if(ret == -1){
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }else{
        ret = productor(&q, &mutex, &lleno, &vacio);
    }

    /*Esperamos por el consumidor*/
    wait(&status);
    /*Elimminamos shmem y los semaforos*/
    Borrar_Semaforo(mutex);
    Borrar_Semaforo(lleno);
    Borrar_Semaforo(vacio);
    shmctl(id, IPC_RMID, (struct shmid_ds *)NULL);
    return ret;
}
