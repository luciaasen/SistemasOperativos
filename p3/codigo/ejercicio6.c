#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#define KEY 1
#define KEY2 2
#define FILEKEY "./"

typedef struct _AlphaStack{
    char[27] alpha;
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
 * @param filekey: parametro para generar la clave con ftok. La key cambia para cada semaforo
 * @param mutex, lleno, vacio: punteros a los ints donde se guarda los sems id
 * @return 0 si semaforos creados, -1 en caso de error (libera los semaforos ya creados si error)
 */
int crear_sems(char *filekey, int *mutex, int *lleno, int *vacio);

/**
 * Sirve para generalizar la enttrada a la seccion critica por parte de ambos procesos, ya que los dos hacen
 * en este orden: down(sem1), down(sem2), up(sem3), y lo que varia son los semaforos sobre los que operan
 * @param sem1: primer semaforo, al que se hara down
 * @param sem2: segundo semaforo, al que se hara down (este deberia ser el mutex)
 * @param sem3: tercer semaforo, al que se hara up
 * @return -1 si error (libera los semaforos en caso de que el error no fuera en argumentos entrada), else 0
 */
int in_critica(int sem1, int sem2, int sem3);

/**
 * Tras crear las zonas de memoria compartida/ semaforos necesarios, esta funcion produce
 * una cola de letras mientras no este disponible todo el abecedario
 * @return -1 en caso de error
 */
int productor();

/**
 * Tras crear las zonas de memoria compartida/ semaforos necesarios, el proceso que ejecuta esta funcion
 * consume, mientras haya productos disponibles, las letras del abecedario
 */
int consumidor();



/*************************/
/*Definicion de funciones*/
/*************************/

int consumidor(){
    int id, mutex, lleno, vacio;
    /*creacion + atach shmem*/
    AlphaStack q;
    if(id = crear_shmem(FILEKEY, KEY, sizeof(AlphaQueue), (void **)(&&q)) == -1){
        return -1;
    }
    
    /*Inicializacion struct*/
    
    /*semaforos necesarios*/
    if( crear_sems(filekey, &mutex, &lleno, &vacio)  == -1){
        shmdt(*att);
        shmctl(id, IPC_RMID, (struct shmid_ds *)NULL);
        return -1;
    }
    /*bucle*/

}

int productor(){
    /*creacion + atach shmem*/
    int id, mutex, lleno, vacio;
    AlphaStack q;
    if(id = crear_shmem(FILEKEY, KEY, sizeof(AlphaQueue), (void **)(&&q)) == -1){
        return -1;
    }
    /*semaforos necesarios*/
    if( crear_sems(filekey, &mutex, &lleno, &vacio)  == -1){
        shmdt(*att);
        shmctl(id, IPC_RMID, (struct shmid_ds *)NULL);
        return -1;
    }    

    /*bucle*/
    while(1){
        if(in_critica(lleno, mutex, vacio) == -1){
            shmdt(*att);
            shmctl(id, IPC_RMID (struct shmid_ds *)NULL);
            return -1;
        }

        /*Produce*/
        q.

        if(Up_Semaforo(mutex, 0, SEM_UNDO) == -1){
            Borrar_Semaforo(mutex);
            Borrar_Semaforo(lleno);
            Borrar_Semaforo(vacio);
            shmdt(*att);
            shmctl(id, IPC_RMID (struct shmid_ds *)NULL);
            return -1;            
        }
    }
}

int producir(){
    /*creacion + atach shmem*/
    int id;
    AlphaStack q;
    if(id = crear_shmem(FILEKEY, KEY, sizeof(AlphaQueue), (void **)(&&q)) == -1){
    }
    /*semaforos necesarios*/
    
    
}

int crear_shmem(char *filekey, int key, int size, void **att){
    int k, id;
    AlphaStack a;
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
        /*Confirmar que no hay problema si el rmid lo hacen todos los procesos*/
        shmctl(id, IPC_RMID, (struct shmid_ds *)NULL);
        return -1;
    }
    return id;
}

int alpha_ini(AlphaStack *a){
    int k, mutex, ret;
    unsigned short[1] array = {1};
    if (a == NULL){
        perror("Error args entrada a alpha_ini\n");
        return -1;
    }
    /*Nos aseguramos de que solo acceda un proceso a la vez*/
    if((k = ftok(FILEKEY, 5)) == -1){
        perror("Error en la creacion de clave para el semaforo de alpha_ini\n");
        return -1;
    }
    if((ret = Crear_Semaforo(k, 1, &mutex)) == -1){
        perror("Error en la creacion del semaforo de alpha_ini\n");
    }else if(ret == 1){
        Inicializar_Semaforo(mutex, array);
    }
    
    Down_Semaforo(mutex, 0, SEM_UNDO);
    /*Asumimos que si el abecedario esta empezado a escribir, otro proceso lo inicializo*/
    if(a->alpha[0] != 'A' && a->alpha[1] != 'B'){
        a->alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
        a->end = 0;
    }
    Up_Semaforo(mutex, 0, SEM_UNDO);
    Borrar_Semaforo(mutex);
    return 0;
}

int crear_sems(char *filekey, int *mutex, int *lleno, int *vacio){
    int ret;, k1, k2, k3
    if(semid == NULL || lleno == NULL || vacio == NULL || filekey == NULL){
        perror("Error en los argumentos de entrada a crear_sems\n");
        return -1;
    }
    if( (k1 = ftok(filekey, 2) == -1) || (k2 = ftok(filekey, 3) == -1) || (k3 = ftok(filekey, 4) == -1) ){
        perror("Error en el ftok desde crear_sems\n");
        return -1;
    }
    if((ret = Crear_Semaforo(k1, 1, mutex)) == -1){
        perror("Error en la creacion de semaforo mutex\n");
        return -1;
    }
    if((ret = Crear_Semaforo(k2, 1, lleno)) == -1){
        perror("Error en la creacion de semaforo lleno\n");
        Borrar_Semaforo(*mutex);
        return -1;
    }
    if((ret = Crear_Semaforo(k3, 1, vacio)) == -1){
        perror("Error en la creacion de semaforo vacio\n");
        Borrar_Semaforo(*mutex);
        Borrar_Semaforo(*lleno);
        return -1;
    }   
    return 0;
}

int in_critica(int sem1, int sem2, int sem3){
    if(sem1 < 0 || sem2 < 0 || sem3 < 0){
       perror("Error en los argumentos de entrada a in_critica\n");
       return -1;
    }
    if( Down_Semaforo(sem1, 0, SEM_UNDO) == -1){
        perror("Error en las operaciones de in_critica\n");
        Borrar_Semaforo(sem1);
        Borrar_Semaforo(sem2);
        Borrar_Semaforo(sem3);
        return -1;
    }
    if( Down_Semaforo(sem2, 0, SEM_UNDO) == -1){
        perror("Error en las operaciones de in_critica\n");
        Borrar_Semaforo(sem1);
        Borrar_Semaforo(sem2);
        Borrar_Semaforo(sem3);
        return -1;
    }
    if( Up_Semaforo(sem3, 0, SEM_UNDO) == -1){
        perror("Error en las operaciones de in_critica\n");
        Borrar_Semaforo(sem1);
        Borrar_Semaforo(sem2);
        Borrar_Semaforo(sem3);
        return -1;
    }
    return 0;
}

int out_critica(int sem1, int sem2){
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
}

