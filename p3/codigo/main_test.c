#include <stdio.h>
#include <stdlib.h>
#include "semaforostest.h"

//COMENTAR RODRI
void main(){
	char nombre[40];
	
	if(Crear_Semaforo_Test() == NOT_PASSED)
		return;
	printf("Exito en el test de Crear_Semaforo.\n");

	if(Inicializar_Semaforo_Test() == NOT_PASSED)
		return;
	printf("Exito en el test de Inicializar_Semaforo.\n");


	return;
}