FLAGS = -c -g
all: cadena_montaje
	rm *.o

cadena_montaje : cadena_montaje.o procesos.o
	gcc -o cadena_montaje $^

cadena_montaje.o : cadena_montaje.c cadena_montaje.h
	gcc $(FLAGS) cadena_montaje.c

procesos.o : procesos.c cadena_montaje.h
	gcc $(FLAGS) procesos.c
	
clean:
	rm *.o