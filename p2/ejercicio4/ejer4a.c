#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

/**
 * Definimos la estructura info que contendra toda la informacion
 * que necesita cada una de las funciones de nuestros hilos.
 */
typedef struct _Info {
    int dim;
    int **matriz;
    int multiplicador;
    int numHilo;
}Info;

/**
 * Se encarga de liberar una matriz
 * @param matriz matriz  a liberar
 * @param dim la dimension en filas de la matriz
 */
void liberarMatriz(int **matriz, int dim){
    int i;
    for (i = 0; i < dim; i++) {
        free(matriz[i]);
    }
    free(matriz);
    return;
}

/**
 * Funcion que se encarga de reservar memoria para una matriz
 * @param  dim    dimesnion de la matriz a la que se le requiere reservar mem
 * @return        int ** con la matriz creada o NULL en caso de error
 */
int **crearMatriz(int dim){
    int **matriz;
    int i;

    matriz = (int * *) malloc(sizeof(int *) * dim);
    if (matriz == NULL) {
        return NULL;
    }

    for (i = 0; i < dim; i++) {
        matriz[i] = (int *) malloc(sizeof(int) * dim);
        if (matriz[i] == NULL) {
            liberarMatriz(matriz, i - 1);
            return NULL;
        }
    }
    return matriz;
}

/**
 * Funcion que se encarga de leer los datos del usuario por la linea de
 * comandos en introducirlos en las estructuras Info pasadas como argumento
 * @param i1 Estructura Info del primer hilo
 * @param i2 Estructura Info del segundo hilo
 * @return 1 si no ha habido error, 0 si ha habido un error
 */
int lecturaDeDatos(Info *i1, Info *i2){
    int  i, j;
    char matriz1[100], matriz2[100];
    char del[2] = " ";
    char *token1;

    printf("Introduzca el multiplicador 1 :\n");
    fgets(matriz1, 100, stdin);
    i1->multiplicador = atoi(matriz1);

    printf("Introduzca el multiplicador 2 :\n");
    fgets(matriz1, 100, stdin);
    i2->multiplicador = atoi(matriz1);

    printf("Introduzca la matriz 1 : \n");
    fgets(matriz1, 100, stdin);
    printf("Introduzca la matriz 2 : \n");
    fgets(matriz2, 100, stdin);

    if (i1->dim == 1) {
        i1->matriz[0][0] = atoi(matriz1);
        i2->matriz[0][0] = atoi(matriz2);
        return 1;
    }

    token1           = strtok(matriz1, del);
    i1->matriz[0][0] = atoi(token1);
    for (i = 0; i < i1->dim; i++) {
        for (j = 0; j < i1->dim; j++) {
            /*La primera asignacion se hace fuera del bucle*/
            if (i == 0 && j == 0) j++;
            token1 = strtok(NULL, del);
            if (token1 == NULL) return 0;
            /*si no, se asignan los valores*/
            i1->matriz[i][j] = atoi(token1);
        }
    }

    token1           = strtok(matriz2, del);
    i2->matriz[0][0] = atoi(token1);
    for (i = 0; i < i2->dim; i++) {
        for (j = 0; j < i2->dim; j++) {
            /*La primera asignacion se hace fuera del bucle*/
            if (i == 0 && j == 0) j++;
            token1 = strtok(NULL, del);
            if (token1 == NULL) return 0;
            /*si no, se asignan los valores*/
            i2->matriz[i][j] = atoi(token1);
        }
    }

    return 1;
}

/**
 * Funcion que se encarga de incilizar la estructura info dada la dimension,
 * tambien inicializa la matriz de dentro
 * @param  dim dimension de la matriz que contiene info
 * @return     Puntero al info creado o null en caso de error
 */
Info *crearInfo(int dim){
    Info *i1;
    i1 = (Info *) malloc(sizeof(Info));
    if (i1 == NULL) {
        return NULL;
    }
    i1->dim    = dim;
    i1->matriz = crearMatriz(dim);
    if (i1->matriz == NULL) {
        free(i1);
        return NULL;
    }
    return i1;
}

/**
 * Funcion que se encarga de liberar un info
 */
void liberarInfo(Info *i){
    if (i == NULL)
        return;
    liberarMatriz(i->matriz, i->dim);
    free(i);
    return;
}

/**
 * Funcion que ejecutara cada hilo
 */
void *operacionPrint(void *i1){
    Info *i = (Info *) i1;
    int  k, j;

    for (k = 0; k < i->dim; k++) {
        printf("Hilo %d multiplicando fila %d resultado ", i->numHilo, k);
        for (j = 0; j < i->dim; j++) {
            printf("%d ", i->matriz[k][j] * i->multiplicador);
        }
        printf("\n");
        usleep(1000000);
    }
    exit(EXIT_SUCCESS);
}


/**
 * Funcion principal que ejecutará a través de hilos operaciones
 * matriciales.
 * Nota: Esta funcion puede ser sensible si no se introducen los datos
 * con el formato indicado
 */
int main(){
    /*Identificadores de los dos hilos a utilizar*/
    pthread_t h1, h2;
    Info      *i1, *i2;
    int       dim, i, j;
    char      str[4];

    printf("Introduzca la dimension de la matriz cuadrada :\n");
    fgets(str, 4, stdin);
    dim = atoi(str);
    if (dim == 0 || dim > 4) {
        printf("La matriz solo puede tener dimension 4 o menor\n");
        exit(EXIT_FAILURE);
    }

    i1 = crearInfo(dim);
    if (i1 == NULL) {
        printf("Error de memoria\n");
        exit(EXIT_FAILURE);
    }
    i2 = crearInfo(dim);
    if (i2 == NULL) {
        liberarInfo(i1);
        printf("Error de memoria\n");
        exit(EXIT_FAILURE);
    }
    i1->numHilo = 1;
    i2->numHilo = 2;

    i = lecturaDeDatos(i1, i2);
    if (i != 1) {
        liberarInfo(i1);
        liberarInfo(i2);
        printf("Error en la lectura de datos\n");
        exit(EXIT_FAILURE);
    }
    pthread_create(&h1, NULL, operacionPrint, (void *) i1);
    pthread_create(&h2, NULL, operacionPrint, (void *) i2);
    pthread_join(h1, NULL);
    pthread_join(h2, NULL);
    liberarInfo(i1);
    liberarInfo(i2);
    return 1;
}
