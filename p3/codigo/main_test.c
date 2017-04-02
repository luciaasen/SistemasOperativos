/**
 * @file main_test.c
 * @author Lucia Asencio y Rodrigo de Pool
 * @date 31-3-2017
 * @brief fichero que contiene el main que prueba la libreria de semaforos
 */
#include <stdio.h>
#include <stdlib.h>
#include "semaforostest.h"

/**
 * Este programa prueba todos los tests de la libreriía semaforostest.h
 * y se asegura de que el retorno sea positivo
 */
void main(){
    char nombre[40];

    if (Crear_Semaforo_Test() == NOT_PASSED)
        return;
    printf("Exito en el test de Crear_Semaforo.\n");

    if (Inicializar_Semaforo_Test() == NOT_PASSED)
        return;
    printf("Exito en el test de Inicializar_Semaforo.\n");

    if (Down_Semaforo_Test() == NOT_PASSED)
        return;
    printf("Exito en el test de Down_Semaforo.\n");

    if (Up_Semaforo_Test() == NOT_PASSED)
        return;
    printf("Exito en el test de Up_Semaforo.\n");


    if (DownMultiple_Semaforo_Test() == NOT_PASSED)
        return;
    printf("Exito en el test de DownMultiple_Semaforo.\n");

    if (UpMultiple_Semaforo_Test() == NOT_PASSED)
        return;
    printf("Exito en el test de UpMultiple_Semaforo.\n");


    return;
}