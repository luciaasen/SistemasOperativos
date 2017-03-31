#include "semaforostest.h"
#include "semaforos.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>

//COMENTAR RODRI
int Crear_Semaforo_Test(){
	int clave = 12341;
	int semid1, semid2;
	int tam = 3;
	int i;
	union semun {
		int val;
		struct semid_ds *semstat;
		unsigned short *array;
	} arg;
	arg.array = (unsigned short *)malloc(sizeof(unsigned short)*tam);

	//Circumstancias normales prueba1
	i = Crear_Semaforo(clave,tam,&semid1);
	if(i == 0 || i  == ERROR){
		//No deberia de haber ninguno antes creado
		perror("Crear_Semaforo_Test prueba1 fallo.\n");
		semctl(semid1, tam, IPC_RMID, arg);
		return NOT_PASSED;
	}
	//Circumstancias normales prueba2
	//Deberia devolver semaforo ya creado pero sin error
	i = Crear_Semaforo(clave, tam, &semid2); 
	if( i == 1 ){
		perror("Crear_Semaforo_Test prueba2 fallo.\n");
		semctl(semid1, tam, IPC_RMID, arg);
		return NOT_PASSED;
	}
	if( semid1 != semid2){
		perror("Crear_Semaforo_Test prueba2 fallo.\n");
		semctl(semid1, tam, IPC_RMID, arg);
		return NOT_PASSED;	
	}

	//Circumstancias anormales prueba3
	//se prueba que devuelve error si se le pasa la
	//misma clave pero distinto tamanio
	if( Crear_Semaforo(clave,tam+1,&semid2) == 1 ||
		semid2 != ERROR) {
		perror("Crear_Semaforo_Test prueba3 fallo.\n");
		semctl(semid1, tam, IPC_RMID, arg);
		return NOT_PASSED;
	}
	//Probamos que todos los semaforos estan inicializados
	//a 0
	semctl(semid1, tam, GETALL, arg);
	for( i = 0; i < tam; i++){
		if(arg.array[i] != 0){
			perror("Crear_Semaforo_Test fallo en los ceros.\n");
			semctl(semid1, tam, IPC_RMID, arg);
			return NOT_PASSED;
		}
	}
	//Liberamos todo
	semctl(semid1, tam, IPC_RMID, arg);

	return PASSED;
}

int Inicializar_Semaforo_Test(){
	int semid;
	int clave = 531445;
	int tam = 4;
	int i;
	unsigned short array1[4] = {0 , 1, 2, 3};
	union semun {
		int val;
		struct semid_ds *semstat;
		unsigned short *array;
	} arg;
	arg.array = (unsigned short *)malloc(sizeof(unsigned short)*tam);


	if(Crear_Semaforo(clave, tam, &semid) == ERROR)
		return NOT_PASSED;

	if(Inicializar_Semaforo(semid, array1) == ERROR){
		semctl(semid, tam, IPC_RMID, arg);
		return NOT_PASSED;
	}

	//Probamos que se inicializa correctamente
	semctl(semid, tam, GETALL, arg);
	for( i = 0; i < tam; i++){
		if(arg.array[i] != i){
			semctl(semid, tam, IPC_RMID, arg);
			return NOT_PASSED;
		}
	}

	//Probamos que da error correctamente
	if(Inicializar_Semaforo(semid, NULL) != ERROR){
		semctl(semid, tam, IPC_RMID, arg);
		return NOT_PASSED;
	}

	semctl(semid, tam, IPC_RMID, arg);
	return PASSED;
}