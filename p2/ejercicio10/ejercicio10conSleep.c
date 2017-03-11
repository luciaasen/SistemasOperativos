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
    char str[13][NUM] = { "EL ", "PROCESO ", "A ", "ESCRIBE ", "EN ", "UN ", "FICHERO ", "HASTA ", "QUE ", "LEE ", "LA ", "CADENA ", "FIN " };
    char aux[NUM];

    /*Creamos o limpiamos el fichero*/
    f = fopen(PATH, "w");
    fclose(f);

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
                    exit(EXIT_SUCCESS);
                }
            }
        }else{
            sleep(5);
            //zona critica
            leerFichero(f, aux);
            //
            printf("%s\n", aux);
            if (strcmp(aux, str[12]) == 0) {
                numhijos++;
                hpid = fork();
            }
        }
    }
    kill(SIGINT, hpid);
    esperarHijos(numhijos);
    fclose(f);
    return 1;
}