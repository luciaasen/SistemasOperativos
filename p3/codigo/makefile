EXEC = ejercicio2 main_test

all: $(EXEC)
	rm *.o

ejercicio2: ejercicio2.c
	gcc -o $@ $^

main_test: semaforos.o semaforostest.o main_test.o
	gcc -o $@ $^

semaforos.o: semaforos.c
	gcc -c  $^

semaforostest.o: semaforostest.c
	gcc -c  $^

main_test.o: main_test.c
	gcc -c  $^

clean:
	 rm $(EXEC) 
