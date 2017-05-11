#include "cadena_montaje.h"

#define MSJ_TYPE1    27
#define MSJ_TYPE2    42

/*PROCESO A*/

int enviarMensaje(int id, FILE *read);

void procesoA(int id, FILE *read){
    int flag = 0;

    do {
        if (enviarMensaje(id, read) == 1) flag = 1;
    } while (flag == 0);
}

/**
 * Funcion que envia el mensaje del proceso A al proceso B con
 * el texto correspondiente
 * @param  id   id del buzon
 * @param  read fichero desdde el que se lee
 * @return      devuelve 1 si se ha enviado el ultimo mensaje
 */
int enviarMensaje(int id, FILE *read){
    int     aux, flag;
    mensaje msg;

    flag = 0;

    msg.id  = MSJ_TYPE1;
    aux     = fread(msg.info, 1, sizeof(char) * TAM, read);
    msg.end = 0;
    if (aux < sizeof(char) * TAM) {
        flag    = 1;
        msg.end = 1;
        /*fread no pone fin de cadena cuando se encuentra en EOF*/
        msg.info[aux / sizeof(char)] = '\0';
    }
    msgsnd(id, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long), IPC_NOWAIT);
    return flag;
}

/*FIN PROCESO A*/


/*PROCESO B*/
void toLowerCase(char *string, char *copy);
int recibirEnviar(int id);

void procesoB(int id){
    int flag = 0;

    do {
        if (recibirEnviar(id) == 1) flag = 1;
    } while (flag == 0);
}

/**
 * Funcion que se encarga de recibir el mensaje de A, cambiarlo a lower case
 * y enviar el mensaje pertinente a C
 * @param  id id del buzon
 * @return    flag de si se ha enviado o no el ultimo mensaje, 1 si es ultimo
 */
int recibirEnviar(int id){
    int     aux = 0;
    mensaje msg27, msg42;

    msgrcv(id, (struct msgbuf *) &msg27, sizeof(mensaje) - sizeof(long), MSJ_TYPE1, 0);
    msg42.end = 0;
    msg42.id  = MSJ_TYPE2;
    if (msg27.end == 1) {
        aux       = 1;
        msg42.end = 1;
    }
    toLowerCase(msg27.info, msg42.info);
    msgsnd(id, (struct msgbuf *) &msg42, sizeof(mensaje) - sizeof(long), IPC_NOWAIT);
    return aux;
}


/**
 * Pasa la cadena string a copy en lower case
 * @param string cadena a pasar a lower case
 * @param copy   cadena en la que se copia en lower case
 */
void toLowerCase(char *string, char *copy){
    int i;
    /*+1 para que tambien se copie el fin de cadena*/
    for (i = 0; i < strlen(string) + 1; i++) {
        copy[i] = tolower(string[i]);
    }
    return;
}


/*FIN PROCESO B*/




/*PROCESO C*/


void procesoC(int id, FILE *write){
    mensaje msg;
    int     flag = 0;

    do {
        msgrcv(id, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long), MSJ_TYPE2, 0);
        if (msg.end == 1) { /*end reached*/
            flag = 1;
        }
        fwrite(msg.info, 1, strlen(msg.info) * sizeof(char), write);
    } while (flag == 0);
}



/*FIN PROCESO C*/
