/**
 * @file semaforostest.c
 * @author Lucia Asencio y Rodrigo de Pool
 * @date 31-3-2017
 * @brief fichero que contiene la implementacion de la libreria de test de semaforos
 */
#include "semaforostest.h"
#include "semaforos.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>

/**
 * Funcion que prueba Crear_Semaforo_Test:
 * 1) Se asegura que se crear un semaforo en condiciones normales
 * 2) Se asegura que se puede solicitar la informacion de un semaforo ya creado
 * 3) Se asegura que NO se puede crear otro semaforo con la misma clave y
 * un tamanio distinto ( si fueran el mismo estariamos en el caso 2)
 * 4) Se asegura que el valor de cada semaforo creado es 0
 * @return PASSED si el test termina con exito, NOT_PASSED en caso contrario
 */
int Crear_Semaforo_Test(){
    int clave = 12341;
    int semid1, semid2;
    int tam = 3;
    int i;
    union semun {
        int             val;
        struct semid_ds *semstat;
        unsigned short  *array;
    } arg;
    arg.array = (unsigned short *) malloc(sizeof(unsigned short) * tam);

    //Circumstancias normales prueba1
    i = Crear_Semaforo(clave, tam, &semid1);
    if (i == 0 || i == ERROR) {
        //No deberia de haber ninguno antes creado
        perror("Crear_Semaforo_Test prueba1 fallo.\n");
        Borrar_Semaforo(semid1);
        return NOT_PASSED;
    }
    //Circumstancias normales prueba2
    //Deberia devolver semaforo ya creado pero sin error
    i = Crear_Semaforo(clave, tam, &semid2);
    if (i == 1 || i == ERROR) {
        perror("Crear_Semaforo_Test prueba2 fallo.\n");
        Borrar_Semaforo(semid1);
        return NOT_PASSED;
    }
    if (semid1 != semid2) {
        perror("Crear_Semaforo_Test prueba2 fallo.\n");
        Borrar_Semaforo(semid1);
        return NOT_PASSED;
    }

    //Circumstancias anormales prueba3
    //se prueba que devuelve error si se le pasa la
    //misma clave pero distinto tamanio
    if (Crear_Semaforo(clave, tam + 1, &semid2) == 1 ||
        semid2 != ERROR) {
        perror("Crear_Semaforo_Test prueba3 fallo.\n");
        Borrar_Semaforo(semid1);
        return NOT_PASSED;
    }
    //Probamos que todos los semaforos estan inicializados
    //a 0
    semctl(semid1, tam, GETALL, arg);
    for (i = 0; i < tam; i++) {
        if (arg.array[i] != 0) {
            perror("Crear_Semaforo_Test fallo en los ceros.\n");
            Borrar_Semaforo(semid1);
            return NOT_PASSED;
        }
    }
    //Liberamos todo
    Borrar_Semaforo(semid1);

    return PASSED;
}

/**
 * Funcion que se asegura del correcto funcionamiento de Inicializar_Semaforo:
 * 1) Nos aseguramos de que podemos inicializar los valores correctamente
 * 2) Nos aseguramos que devuelve error en caso de dar argumentos erroneos
 * @return PASSED si el test terminal con exito, en cualquier otro caso NOT_PASSED
 */
int Inicializar_Semaforo_Test(){
    int            semid;
    int            clave = 531445;
    int            tam   = 4;
    int            i;
    unsigned short array1[4] = { 0, 1, 2, 3 };
    union semun {
        int             val;
        struct semid_ds *semstat;
        unsigned short  *array;
    } arg;
    arg.array = (unsigned short *) malloc(sizeof(unsigned short) * tam);


    if (Crear_Semaforo(clave, tam, &semid) == ERROR)
        return NOT_PASSED;

    if (Inicializar_Semaforo(semid, array1) == ERROR) {
        Borrar_Semaforo(semid);
        return NOT_PASSED;
    }

    //Probamos que se inicializa correctamente
    semctl(semid, tam, GETALL, arg);
    for (i = 0; i < tam; i++) {
        if (arg.array[i] != i) {
            Borrar_Semaforo(semid);
            return NOT_PASSED;
        }
    }

    //Probamos que da error correctamente
    if (Inicializar_Semaforo(semid, NULL) != ERROR) {
        Borrar_Semaforo(semid);
        return NOT_PASSED;
    }

    Borrar_Semaforo(semid);
    return PASSED;
}

/**
 * Funcion privada que generaliza las prueba Down_Semaforo_Test y
 * Up_Semaforo_Test.
 * @param  num si es -1 se realiza Down_Semaforo_Test, cualquier otro numero
 * realiza Up_Semaforo_Test
 * @return     PASSED o NOT_PASSED dependiendo de si se paso o no la prueba
 */
int General_Single_Test(int num){
    int            semid;
    int            clave = 531445;
    int            tam   = 4;
    int            i, aux;
    unsigned short array1[4] = { 1, 2, 3, 4 };
    union semun {
        int             val;
        struct semid_ds *semstat;
        unsigned short  *array;
    } arg;
    arg.array = (unsigned short *) malloc(sizeof(unsigned short) * tam);

    Crear_Semaforo(clave, 4, &semid);
    Inicializar_Semaforo(semid, array1);
    for (i = 0; i < tam; i++) {
        if (num == -1) {
            Down_Semaforo(semid, i, SEM_UNDO);
            aux = i;
        }else{
            Up_Semaforo(semid, i, SEM_UNDO);
            aux = i + 2;
        }
        if (semctl(semid, i, GETVAL, 0) != aux) {
            Borrar_Semaforo(semid);
            return NOT_PASSED;
        }
    }
    Borrar_Semaforo(semid);
    return PASSED;
}

/**
 * Funcion que prueba el correcto funcionamiento de Down_Semaforo:
 * Crea un semaforo de 4 elementos y se inicializan. Luego se hace
 * down a cada uno de los semaforos probando que, en efecto, su
 * valor disminuye
 * @return PASSED si el test termina con exito, NOT_PASSED en caso contrario
 */
int Down_Semaforo_Test(){
    return General_Single_Test(-1);
}


/**
 * Funcion que prueba el correcto funcionamiento de Up_Semaforo:
 * Crea un semaforo de 4 elementos y se inicializan. Luego se hace
 * up a cada uno de los semaforos probando que, en efecto, su
 * valor aumenta
 * @return PASSED si el test termina con exito, NOT_PASSED en caso contrario
 */
int Up_Semaforo_Test(){
    return General_Single_Test(1);
}


/**
 * Funcion privada que generaliza las prueba DownMultiple_Semaforo_Test y
 * UpMultiple_Semaforo_Test.
 * @param  num si es -1 se realiza DownMultiple_Semaforo_Test, cualquier otro numero
 * realiza UpMultiple_Semaforo_Test
 * @return     PASSED o NOT_PASSED dependiendo de si se paso o no la prueba
 */
int General_Multiple_Test(int num){
    int            semid;
    int            clave = 531445;
    int            tam   = 4;
    int            i, aux, aux2;
    unsigned short array1[4] = { 1, 2, 3, 4 };
    int            active[3] = { 0, 2, 3 };
    union semun {
        int             val;
        struct semid_ds *semstat;
        unsigned short  *array;
    } arg;
    arg.array = (unsigned short *) malloc(sizeof(unsigned short) * tam);

    Crear_Semaforo(clave, 4, &semid);
    Inicializar_Semaforo(semid, array1);

    if (num == -1) {
        DownMultiple_Semaforo(semid, 3, SEM_UNDO, active);
        aux = -1;
    }else{
        UpMultiple_Semaforo(semid, 3, SEM_UNDO, active);
        aux = 1;
    }
    for (i = 0; i < tam; i++) {
        aux2 = 0;
        if (i != 1) {
            aux2 = aux;
        }
        if (semctl(semid, i, GETVAL, 0) != array1[i] + aux2) {
            Borrar_Semaforo(semid);
            return NOT_PASSED;
        }
    }
    Borrar_Semaforo(semid);
    return PASSED;
}

/**
 * Funcion que prueba el correcto funcioamiento de DownMultiple_Semaforo:
 * Se crea un semaforo de 4 elementos y se inicializan. Se le aplica la
 * funcion a 3 elementos indicando sus valores a traves de un array. Luego
 * se prueba que los valores fueron cambiados satisfactoriamente y que
 * el elemento no cambiado se mantuvo en su mismo estado
 * @return PASSED si el el test termina con exito, NOT_PASSED en caso contrario
 */
int DownMultiple_Semaforo_Test(){
    return General_Multiple_Test(-1);
}


/**
 * Funcion que prueba el correcto funcioamiento de UpMultiple_Semaforo:
 * Se crea un semaforo de 4 elementos y se inicializan. Se le aplica la
 * funcion a 3 elementos indicando sus valores a traves de un array. Luego
 * se prueba que los valores fueron cambiados satisfactoriamente y que
 * el elemento no cambiado se mantuvo en su mismo estado
 * @return PASSED si el el test termina con exito, NOT_PASSED en caso contrario
 */
int UpMultiple_Semaforo_Test(){
    return General_Multiple_Test(1);
}