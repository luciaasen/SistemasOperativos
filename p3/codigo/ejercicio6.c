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

#define KEY 14327876
#define FILEKEY "/"

typedef struct {
    char alpha[27];
    int end;
    int temp;
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
int crear_shmem(char *filekey, int key, int size, AlphaStack **att);

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
 * Tras crear los semaforos necesarios, esta funcion produce
 * una cola de letras mientras no este disponible todo el abecedario
 * En caso de error, hara detach de la memoria pero no la borrara
 * @param alpha: puntero a la estructura compartida
 * @param mutex, lleno, vacio: punteros a los semids
 * @return -1 en caso de error, else 0.
 */
int productor(AlphaStack *alpha, int *mutex, int *lleno, int *vacio, int prodSleep);

/**
 * Tras crear los semaforos necesarios, el proceso que ejecuta esta funcion
 * consume, mientras haya productos disponibles, las letras del abecedario
 * En caso de error, hara detach de la memoria pero no la borrara
 * @param alpha: puntero a la estructura compartida
 * @param mutex, lleno, vacio: punteros a los semids
 * @return -1 en caso de error, else 0
 */
int consumidor(AlphaStack *alpha, int *mutex, int *lleno, int *vacio, int consSleep);



/*************************/
/*Definicion de funciones*/
/*************************/

int consumidor(AlphaStack *alpha, int *mutex, int *lleno, int *vacio, int consSleep){ 
    if(alpha == NULL || mutex == NULL || lleno == NULL || vacio == NULL){
        return -1;
    }
       
    /*bucle*/
    while(alpha->temp == 1){
        
        /*Down lleno, down mutex*/
        if(Down_Semaforo(*lleno, 0, 0) == -1){
            shmdt(alpha);
            return -1;            
        }
        if(alpha->temp == 0){
            shmdt(alpha);
            return 0;            
        }
        if(Down_Semaforo(*mutex, 0, 0) == -1){
            shmdt(alpha);
            return -1;            
        }

        /*Consume*/
        printf("El consumidor consume %c se encuentra el end a %d y lo decrementa\n", alpha->alpha[alpha->end], alpha->end);
        fflush(stdout);
        (alpha->end) -= 1;

        /*Up vaico, up mutex*/
        if(Up_Semaforo(*vacio, 0, 0) == -1){
            shmdt(alpha);
            return -1;            
        }
        if(Up_Semaforo(*mutex, 0, 0) == -1){
            shmdt(alpha);
            return -1;            
        }
        
        usleep(consSleep);
    }
    shmdt(alpha);
    return 0;
}

int productor(AlphaStack *alpha, int *mutex, int *lleno, int *vacio, int prodSleep){
    if(alpha == NULL || mutex == NULL || lleno == NULL || vacio == NULL){
        return -1;
    }
    
    /*bucle*/
    while(alpha->temp == 1){

        /*Down vacio, down mutex*/
        if(Down_Semaforo(*vacio, 0, 0) == -1){
            shmdt(alpha);
            return -1;            
        }
        if(alpha->temp == 0){
            shmdt(alpha);
            return 0;            
        }
        if(Down_Semaforo(*mutex, 0, 0) == -1){
            shmdt(alpha);
            return -1;            
        }

        /*Produce*/
        (alpha->end) += 1;
        printf("El productor ha producido %c y deja el end a %d\n", alpha->alpha[alpha->end], alpha->end);
        fflush(stdout);
       
        /*Up lleno, up mutex*/
        if(Up_Semaforo(*lleno, 0, 0) == -1){
            shmdt(alpha);
            return -1;            
        }
        if(Up_Semaforo(*mutex, 0, 0) == -1){
            shmdt(alpha);
            return -1;            
        }
        usleep(prodSleep);
    }
    shmdt(alpha);
    return 0;
}

int crear_shmem(char *filekey, int key, int size, AlphaStack **att){
    int k, id;
    if(filekey == NULL || size < 1 ){
        perror("Error en los parametros para crear_shmem\n");
        return -1;
    }   

    /*Generamos una clave*/
    if((k = ftok(filekey, key)) == -1){
        perror("Error en fok desde crear_shmem\n");
        return -1;
    }

    /*Getteamos la shared mem*/
    id = shmget(k, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W); 
    if( id == -1){
        if(errno == EEXIST){
            id = shmget(k, size, SHM_R | SHM_W);
            if(id == -1){
                printf("Error en shmget, argumentos k %d size %d\n", k, size);
                return -1;
            }            
        }else{
            perror("Error2 en el shmget\n");
            return -1;
        }
    }
   
    /*Hacemos attach*/
    *att = (AlphaStack *)shmat(id, (void *)NULL, 0);  
    if( *att == ((void*)-1)){
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

    k1 = ftok(filekey, 256743);
    k2 = ftok(filekey, 721572);
    k3 = ftok(filekey, 17625);
    if( ( k1 == -1) || ( k2 == -1) ||  k3 == -1) {
        perror("Error en el ftok desde crear_sems\n");
        return -1;
    }

    /*Debemos inicializar los semaforos, ademas de crearlos*/
    /*Mutex se inicializa a 1, vacio a 0, lleno a 26*/
    ret = Crear_Semaforo(k1, 1, mutex); 
    if((ret) == -1){
        perror("Error en la creacion de semaforo mutex \n");
        return -1;
    } else {
        array[0] = 1;
        Inicializar_Semaforo(*mutex, array);
    }
   
    ret = Crear_Semaforo(k2, 1, vacio);
    if((ret) == -1){
        perror("Error en la creacion de semaforo vacio \n");
        Borrar_Semaforo(*mutex);
        return -1;
    } else {
        array[0] = 26;
        Inicializar_Semaforo(*vacio, array);
    }

    ret = Crear_Semaforo(k3, 1, lleno);
    if((ret) == -1){
        perror("Error en la creacion de semaforo lleno\n");
        Borrar_Semaforo(*mutex);
        Borrar_Semaforo(*vacio);
        return -1;
    } else {
        array[0] = 0;
        Inicializar_Semaforo(*lleno, array);
    }
    
    return 0;
}

int main(int argc, char**argv){
    /* En orden, variables para: shmem/sems; forks; return+args del main; estructura del almacen */
    int id, mutex, lleno, vacio;
    int pid[2], status[2];
    int ret, prodSleep, consSleep, tempo;
    AlphaStack q, *pq;
    char myalpha[27] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '\0'};

    /*Captamos argumentos entrada*/
    if(argc != 4){
        perror("Error. El programa espera 3 naturales:\n \tn1: sleep productor\n\tn2: sleep consumidor\n\tn3: temporizador\n");
        return -1;
    }
    prodSleep = atoi(argv[1]);
    consSleep = atoi(argv[2]);
    tempo = atoi(argv[3]);
    if(prodSleep < 0 || consSleep < 0 || tempo < 1){
        perror("Error: Necesito sleeps de entrada >= 0, temp > 0 \n");
        return -1;
    }
    /*creacion + atach + inicialicacion shmem*/
    pq = &q;
    if( (id = crear_shmem(FILEKEY, KEY, sizeof(AlphaStack), (&pq)) )   == -1){
        return -1;
    }
    strcpy(pq->alpha, myalpha);
    pq->end = -1;    
    pq->temp = 1;
    /*semaforos necesarios*/
    if( (crear_sems(FILEKEY, &mutex, &lleno, &vacio)) == -1){
        shmdt(pq);
        shmctl(id, IPC_RMID, (struct shmid_ds *)NULL);
        return -1;
    }    
    /*Ahora creamos otros 2 procesos: padre produce, hijo1 consume, hijo2 temporiza*/
    if((pid[0] = fork()) == -1){
        perror("Error en el fork\n");
        shmdt(pq);
        ret = -1;
    }else if( pid[0] == 0){
        ret = consumidor(pq, &mutex, &lleno, &vacio, consSleep);
        if(ret == -1){
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }else{
        if( (pid[1] = fork()) == -1){
            perror("Error en el fork al temporizador\n");
            shmdt(pq);
            ret = -1;
        }else if (pid[1] == 0){
            usleep(tempo);
            pq->temp = 0;
            if(Up_Semaforo(lleno, 0, 0) == -1){
                shmdt(pq);
                return -1;            
            }
            if(Up_Semaforo(vacio, 0, 0) == -1){
                shmdt(pq);
                return -1;            
            }
            exit(EXIT_SUCCESS);
        }else{
            ret = productor(pq, &mutex, &lleno, &vacio, prodSleep);
            wait(&(status[1]));
            wait(&(status[0]));
        }
    }
    /*Eliminamos shmem y los semaforos*/
    shmctl(id, IPC_RMID,(struct shmid_ds *)NULL);
    Borrar_Semaforo(mutex);
    Borrar_Semaforo(lleno);
    Borrar_Semaforo(vacio);
    fflush(stdout);
    return ret;
}
