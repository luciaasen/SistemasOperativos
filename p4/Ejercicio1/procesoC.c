#include "cadena_montaje.h"


void procesoC(int id, FILE *write){
    mensaje msg;
    int     flag = 0;

    do {
        msgrcv(id, &msg, sizeof(mensaje) - sizeof(long), 42, 0);
        if (msg.end == 1) { /*end reached*/
            flag = 1;
        }
        fwrite(msg.info, 1, strlen(msg.info) * sizeof(char), write);
    } while (flag == 0);
}