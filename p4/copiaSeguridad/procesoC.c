#include "cadena_montaje.h"


void procesoC(int id, int num, FILE *write){
    msgbuf msg;
    int    flag = 0;
    msg.info = (char *) malloc(sizeof(char) * (num));
    if (msg.info == NULL) {
        perror("Error de memoria, cerrar el programa.\n");
        return;
    }

    do { //Estp???
        msg.info = (char *) malloc(sizeof(char) * (num));
        //??

        msgrcv(id, &msg, sizeof(msgbuf), 42, 0);
        if (msg.type == 1) { /*end reached*/
            flag = 1;
        }
        fwrite(msg.info, 1, num * sizeof(char), write);
    } while (flag == 0);
    //free(msg.info);
}