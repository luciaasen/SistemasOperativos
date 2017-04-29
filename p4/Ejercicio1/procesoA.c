#include "cadena_montaje.h"

void procesoA(int id, int num, FILE *read){
    mensaje msg;
    int     flag = 0, aux;
    char    *str = (char *) malloc(sizeof(char) * num);
    if (str == NULL) {
        perror("Error de memoria, cerrar programa y vuelve a intentarlo.\n");
        return;
    }

    msg.type = 27;
    do {
        aux     = fread(str, 1, sizeof(char) * num, read);
        msg.end = 0;
        if (aux < sizeof(char) * num) { /*end reached*/
            flag    = 1;
            msg.end = 1;
        }
        msg.info = str;
        msgsnd(id, (struct msgbuf *) &msg, sizeof(mensaje), IPC_NOWAIT);
    } while (flag == 0);
    free(str);
}
