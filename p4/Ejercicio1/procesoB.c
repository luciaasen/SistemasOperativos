#include "cadena_montaje.h"

char *copyLowerCase(char *string);

void procesoB(int id, int num){
    mensaje msg27, msg42;
    int     flag = 0;
    char    *str;

    msg42.type = 42;
    do {
        msgrcv(id, (struct msgbuf *) &msg27, sizeof(mensaje), 27, 0);
        msg42.end = 0;
        if (msg27.end == 1) {
            flag      = 1;
            msg42.end = 1;
        }
        str        = copyLowerCase(msg27.info);
        msg42.info = str;
        msgsnd(id, (struct msgbuf *) &msg42, sizeof(mensaje), IPC_NOWAIT);
        free(str);
    } while (flag == 0);
}


char *copyLowerCase(char *string){
    int  i;
    char *copy;
    copy = (char *) malloc(sizeof(char) * (strlen(string) + 1));
    if (copy == NULL) {
        perror("Error de memoria, cerrar programa y vuelve a intentarlo\n");
    }

    for (i = 0; i < strlen(string); i++) {
        copy[i] = tolower(string[i]);
    }
    return copy;
}
