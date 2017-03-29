#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#define FILEKEY    "/"
#define KEY        10

typedef struct _Info {
    char nombre[80];
    int  id;
}Info;

/**
 * Imprime la informacion contenida en la estructura Info.
 * Si el parametro pasado es NULL no hace nada
 * @param  i estructura Info que se queire imprimir
 * @return   1 si se imprime 0 en caso contrario
 */
int printInfo(Info *i){
    if (i == NULL) return 0;
    printf("Nombre del cliente: %s\nId: %d\n", i->nombre, i->id);
    return 1;
}

/**
 * Manejador de la senial SIG_USR1 que no hace nada (ignora la senial)
 * @param sig Numero de la senial recibida
 */
void manejador(int sig){
    return;
}

/**
 * Ejercicio 2 implementación especificada en el pdf de la entrega
 * @param  argc [description]
 * @param  argv Se espera un argumento 'n' que es el numero de procesos hijos
 * a crear
 */
int main(int argc, char const *argv[]) {
    int  nhijos;
    int  key, id_zone;
    Info *info;

    if (argc < 2) {
        perror("Argumentos insuficientes.\n");
        return 0;
    }
    nhijos = atoi(argv[1]);

    if (nhijos < 1) {
        perror("Numero de hijos menor que 1.\n");
        return 0;
    }

    //Key para la memoria
    key = ftok(FILEKEY, KEY);
    //Asignamos memoria compartida para la estructura
    id_zone = shmget(key, sizeof(Info), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
    if (id_zone == -1) {
        perror("Error en la peticion de memoria compartida.\n");
        return 0;
    }
    //El attach servira asi para todos? o habra que hacer attach dentro de cada hijo?
    info = (Info *) shmat(id_zone, (void *) NULL, 0);
    if (info == NULL) {
        perror("Error en el attachment de memoria.\n");
        return 0;
    }
    //Redefinimos como actuar en caso de que se reciba la senial SIG_USR1
    //??el manejador no haria nada?? ya que si hace algo tendriamos que suar variables globales

    //Hacemos detach de la memoria
    if (shmdt((void *) info) == -1) {
        perror("Error en el detach de la memoria.\n");
    }
    shmctl(id_zone, IPC_RMID, NULL);
    return 0;
}