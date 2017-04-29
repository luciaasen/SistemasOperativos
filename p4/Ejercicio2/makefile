OBJ = monitor_lib.o monitor.o caballos.o

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
	
clean:
	rm monitor