all: cadena_montaje

cadena_montaje : cadena_montaje.o procesoA.o procesoB.o procesoC.o
	gcc -o cadena_montaje $^

cadena_montaje.o : cadena_montaje.c cadena_montaje.h
	gcc -c cadena_montaje.c

procesoA.o : procesoA.c cadena_montaje.h
	gcc -c procesoA.c
	
procesoB.o : procesoB.c cadena_montaje.h
	gcc -c procesoB.c

procesoC.o : procesoC.c cadena_montaje.h
	gcc -c procesoC.c

clean:
	rm *.o