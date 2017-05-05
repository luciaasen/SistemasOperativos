#include "cadena_montaje.h"

int enviarMensaje(int id, FILE *read);

void procesoA(int id, FILE *read){
    int flag = 0;

    do {
        if (enviarMensaje(id, read) == 1) flag = 1;
    } while (flag == 0);
}


int enviarMensaje(int id, FILE *read){
    int     aux, flag;
    mensaje msg;

    flag = 0;

    msg.id  = 27;
    aux     = fread(msg.info, 1, sizeof(char) * TAM, read);
    msg.end = 0;
    if (aux < sizeof(char) * TAM) {
        flag    = 1;
        msg.end = 1;
        /*fread no pone fin de cadena cuando se encuentra en EOF*/
        msg.info[aux / sizeof(char)] = '\0';
    }
    msgsnd(id, &msg, sizeof(mensaje)- sizeof(long), IPC_NOWAIT);
    return flag;
}
