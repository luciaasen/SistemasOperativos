/**
 * @file ejercicio10sincronizado.c
 * @author Lucia Asencio y Rodrigo de Pool
 * @date 17-3-2017
 * @brief fichero que contiene una variacion del ej10 de la practica 2 de SOPER
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define NUM     50
#define PATH    "fichero"

/**
 * Se encarga de escribir un string en el fichero
 * @param f     puntero al fichero
 * @param str   str string a escribir
 */
void escribirFichero(FILE *f, char * str){
    fwrite(str, NUM, sizeof(char), f);
}

/**
 * Esta función imprime que se inicia una 'tanda' de lectura
 * @param sig [description]
 */
void manejador(int sig){
    printf("Inicia la siguiente tanda de lectura:\n");
}


/**
 * Se encarga de leer una linea del fichero y guardarla en str
 * @param f   puntero al fichero
 * @param str string donde se guarda el string leido
 */
void leerFichero(FILE *f, char *str){
    fread(str, NUM, sizeof(char), f);
}

/**
 * Funcion que se encarga de esperar a un numero de terminado de hijos
 * @param num numero de hijos
 */
void esperarHijos(int num){
    int i;
    for (i = 0; i < num; i++) {
        wait(NULL);
    }
}

/**
 * Ejercicio 10 propuesto
 */
int main(){
    int  leidos;
    int  hpid;
    int  numhijos = 1;
    FILE *f;
    int  randi;
    int  flag         = 0;
    char str[13][NUM] = { "EL ", "PROCESO ", "A ", "ESCRIBE ", "EN ", "UN ", "FICHERO ", "HASTA ", "QUE ", "LEE ", "LA ", "CADENA ", "FIN " };
    char aux[NUM];

    /*Creamos o limpiamos el fichero*/
    f = fopen(PATH, "w");
    fclose(f);

    signal(SIGUSR1, manejador);

    hpid = fork();
    if ((f = fopen(PATH, "r")) == NULL) {
        esperarHijos(numhijos);
        printf("Error en el fopen\n");
        exit(EXIT_FAILURE);
    }

    for (leidos = 0; leidos < NUM; leidos++) {
        if (hpid == 0) {
            fclose(f); /*cerramos fichero de lectura del padre*/
            if ((f = fopen(PATH, "a")) == NULL) {
                printf("Error en el fopen\n");
                exit(EXIT_FAILURE);
            }
            srand(time(NULL) * getpid());
            while (1) {
                randi = rand() % 13;
                //zona critica
                escribirFichero(f, str[randi]);
                //
                if (randi == 12) {
                    fclose(f);
                    kill(getppid(), SIGUSR1);
                    exit(EXIT_SUCCESS);
                }
            }
        }else{
            if (flag == 0) {
                flag = 1;
                pause();
            }
            //zona critica
            leerFichero(f, aux);
            //
            printf("%s\n", aux);
            if (strcmp(aux, str[12]) == 0) {
                numhijos++;
                flag = 0;
                hpid = fork();
            }
        }
    }
    kill(SIGINT, hpid);
    esperarHijos(numhijos);
    fclose(f);
    return 1;
}