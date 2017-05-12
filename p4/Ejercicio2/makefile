OBJ = monitor_lib.o monitor.o caballos.o apuestas.o gestor.o apostador.o semaforos.o

FLAG= -c -Wall -g 

all: monitor
	rm *.o

monitor: $(OBJ)
	gcc -o $@ $^  -lpthread

monitor_lib.o : monitor_lib.c monitor_lib.h
	gcc $(FLAG) monitor_lib.c

monitor.o: monitor.c
	gcc $(FLAG) monitor.c
	
caballos.o : caballos.c caballos.h
	gcc $(FLAG) caballos.c
	
gestor.o : gestor.c gestor.h 
	gcc $(FLAG) gestor.c 
	
apostador.o : apostador.c apostador.h
	gcc $(FLAG) apostador.c

apuestas.o : apuestas.c apuestas.h apostador.h gestor.h
	gcc $(FLAG) apuestas.c	
	
semaforos.o : semaforos.c semaforos.h
	gcc $(FLAG) semaforos.c
    
clean:
	rm monitor
