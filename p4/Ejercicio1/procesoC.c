#include "cadena_montaje.h"

void procesoC(int id, int num, FILE *write){
    mensaje msg;
    int     flag = 0;

    do {
        msgrcv(id, (struct msgbuf *) &msg, sizeof(mensaje), 42, 0);
        if (msg.type == 1) { /*end reached*/
            flag = 1;
        }
        fwrite(msg.info, 1, num * sizeof(char), write);
    } while (flag == 0);
}