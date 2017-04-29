#include "cadena_montaje.h"

void toLowerCase(char *string, char *copy);

void procesoB(int id, int num){
    msgbuf msg27, msg42;
    int    flag  = 0;
    char   *copy = (char *) malloc(sizeof(char) * (num));
    msg27.info = (char *) malloc(sizeof(char) * (num));
    if (copy == NULL || msg27.info == NULL) {
        perror("Error de memoria, cerrar programa y vuelve a intentarlo\n");
        return;
    }

    msg42.type = 42;
    do {
        //ESTO??
        char *copy = (char *) malloc(sizeof(char) * (num));
        msg27.info = (char *) malloc(sizeof(char) * (num));
        //ESTO??
        msgrcv(id, &msg27, sizeof(msgbuf), 27, 0);
        msg42.end = 0;
        if (msg27.end == 1) {
            flag      = 1;
            msg42.end = 1;
        }
        toLowerCase(msg27.info, copy);
        msg42.info = copy;
        msgsnd(id, &msg42, sizeof(msgbuf), IPC_NOWAIT);
    } while (flag == 0);
    //free(copy);
    //free(msg27.info);
}


void toLowerCase(char *string, char *copy){
    int i;

    for (i = 0; i < strlen(string); i++) {
        copy[i] = tolower(string[i]);
    }
    return;
}
