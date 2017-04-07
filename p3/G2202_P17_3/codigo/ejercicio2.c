/**
 * @file ejercicio2.c
 * @author Lucia Asencio y Rodrigo de Pool
 * @date 31-3-2017
 * @brief fichero que contiene la implementacion del ejercicio 2
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

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
 * Esta function espera un tiempo aleatorio entre 0 y 1 segundo
 * (0, 0.1, 0.2 , ... , 1).
 * Solicitara al usuario que introduzca un nombre de cliente
 * Incrementara en 1 el id
 * Enviara una senial SIGUSR1 al proceso padre
 * Hara un detach de la memoria compartida
 * @param info [description]
 */
void accionHijo(Info *info){
    int num = rand() % 11;
    usleep(100000 * num);
    printf("Introduzca nombre de cliente:\n");
    scanf("%s", info->nombre);
    info->id++;
    kill(getppid(), SIGUSR1);
    shmdt((void *) info);
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
    int  key, id_zone, i, aux;
    Info *info;

    if (argc < 2) {
        perror("Argumentos insuficientes.\n");
        return 0;
    }
    nhijos = atoi(argv[1]);

    if (nhijos < 1) {
        perror("Argumento introducido invalido\n");
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
    //Establecemos memoria para todos los hijos en info
    info = (Info *) shmat(id_zone, (void *) NULL, 0);
    if (info == (void *)-1) {
        perror("Error en el attachment de memoria.\n");
        return 0;
    }
    info->id = 0;
    //Redefinimos como actuar en caso de que se reciba la senial SIG_USR1 (ignorarla)
    if (signal(SIGUSR1, manejador) == SIG_ERR) {
        perror("Error en la redefinicion del manejador de SIG_USR1.\n");
        return 0;
    }
    //Generamos los hijos
    for (i = 0; i < nhijos; i++) {
        if ((aux = fork()) == 0) {
            accionHijo(info);
            exit(EXIT_SUCCESS);
        }else if (aux == -1) {
            perror("Error en la generación de un hijo");
        }
    }
    //Imprimimos hasta que todos los hijos hayan tocado la variable id
    while (info->id < nhijos) {
        pause();
        printInfo(info);
    }

    //Correspondientes waits para eliminar los bloques de control de procesos
    for (i = 0; i < nhijos; i++) {
        wait(NULL);
    }

    //Hacemos detach de la memoria
    if (shmdt((void *) info) == -1) {
        perror("Error en el detach de la memoria.\n");
    }
    shmctl(id_zone, IPC_RMID, NULL);
    return 0;
}
