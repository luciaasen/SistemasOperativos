#include "cadena_montaje.h"

void toLowerCase(char *string, char *copy);
int recibirEnviar(int id);

void procesoB(int id){
    int flag = 0;

    do {
        if (recibirEnviar(id) == 1) flag = 1;
    } while (flag == 0);
}


int recibirEnviar(int id){
    int     aux = 0;
    mensaje msg27, msg42;

    msgrcv(id, &msg27, sizeof(mensaje), 27, 0);
    msg42.end = 0;
    msg42.id  = 42;
    if (msg27.end == 1) {
        aux       = 1;
        msg42.end = 1;
    }
    toLowerCase(msg27.info, msg42.info);
    msgsnd(id, &msg42, sizeof(mensaje), IPC_NOWAIT);
    return aux;
}

void toLowerCase(char *string, char *copy){
    int i;

    for (i = 0; i < strlen(string) + 1; i++) {
        copy[i] = tolower(string[i]);
    }
    return;
}
