OBJ = monitor_lib.o monitor.o caballos.o apuestas.o gestor.o apostador.o semaforos.o

all: monitor
	rm *.o

monitor: $(OBJ)
	gcc -o $@ $^ 

monitor_lib.o : monitor_lib.c monitor_lib.h
	gcc -c monitor_lib.c

monitor.o: monitor.c
	gcc -c  monitor.c
	
caballos.o : caballos.c caballos.h
	gcc -c caballos.c
	
gestor.o : gestor.c gestor.h 
	gcc -c gestor.c 
	
apostador.o : apostador.c apostador.h
	gcc -c apostador.c

apuestas.o : apuestas.c apuestas.h apostador.h gestor.h
	gcc -c apuestas.c	
	
semaforos.o : semaforos.c semaforos.h
	gcc -c semaforos.c
    
clean:
	rm monitor
