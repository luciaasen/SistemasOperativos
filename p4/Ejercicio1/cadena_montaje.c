#include "cadena_montaje.h"

/**
 * Funcion principal, se encarga de determinar valores iniciales
 * y lanzar los procesos
 */
void main(int argc, char ** argv){
    int  num, id, key;
    FILE *read, *write;

    if (argc < 3) {
        perror("Numero de caracteres insuficientes\n");
        return;
    }

    read = fopen(argv[1], "r");
    if (read == NULL) {
        perror("Error al abrir el fichero de lectura\n");
        return;
    }

    write = fopen(argv[2], "r");
    if (write == NULL) {
        perror("Error al abrir el fichero de escritura\n");
        fclose(read);
        return;
    }

    srand(time(NULL) * getpid());
    key = rand();

    id = msgget(key, IPC_CREAT | 0600);
    if (id == -1) {
        perror("Error en la creacion de la cola de mensajes\n");
        fclose(read);
        fclose(write);
        return;
    }

    num = char4K();

    if (fork() == 0) {
        procesoA(id, num, read);
        return;
    }else if (fork() == 0) {
        procesoB(id, num);
        return;
    }
    procesoC(id, num, write);
    /*Esperamos al procesoA y al procesoB*/
    wait(NULL);
    wait(NULL);
    msgctl(id, IPC_RMID, NULL);
    return;
}

int char4K(){
    int num = 1;
    while (sizeof(char) * num < 4000)
        num++;
    return num;
}
