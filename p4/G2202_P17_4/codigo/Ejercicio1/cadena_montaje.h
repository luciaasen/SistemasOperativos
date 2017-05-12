#ifndef CADENA_MONTAJE_H
#define CADENA_MONTAJE_H
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define TAM    10

typedef struct _mensaje {
    long id;
    int  end;
    char info[TAM];
}mensaje;


/**
 * Comportamiento explicado en el enunciado. Acabara cuando se acabe el fichero.
 * En el ultimo mensaje indicara que es el ultimo marcando con un 1 el atributo
 * end del mensaje. Los tipos enviados seran marcados con el long 27
 * @param id   id del buzon
 * @param num  tamanio de caracteres dentro de la estructura
 * @param read fichero en el que se lee
 */
void procesoA(int id, FILE *read);
/**
 * Funcionamiento explicado en el enunciado. Cuando envie el ultimo mensaje a
 * C lo dira marcando con un 1 el atributo end del mensaje. Recibira mensaje
 * de tipo 27 y enviara de tipo 42
 * @param id  id del buzon
 * @param num numero de caracteres a leer dentro de la estructura
 */
void procesoB(int id);
/**
 * Funcionamiento explicado en el enunciado. Cuando envie reciba el ultimom
 * mensaje lo escribira y se cerrara. Recibira mensaje de tipo 42
 * @param id    id del buzon
 * @param num   numero de caracteres dentro de la estructura de mensaje
 * @param write fichero en el que se escribe
 */
void procesoC(int id, FILE *write);


#endif