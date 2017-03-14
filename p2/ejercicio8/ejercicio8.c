#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

/**
* Funcion que devuelve una string con la hora actual
*/
int mytime(){
    char output[50];
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    strftime(output, 50, "%H:%M:%S", tlocal);
    printf("Hola PID = %d, time = %s\n", getpid(), output);
    return 0;
}

/**
* Manejador para la senial SIGTERM
* @param int sig senial a manejar
*/
void sigtermHandler(int sig){
    printf("Muere PID=%d\n", getpid());
}

/**
* Manejador para SIGUSR1
* @param sig senial a manejar
*/
void sigusr1Handler(int sig){
    mytime();
    sleep(2);
    return;
}

/**
* Funcion main: crea N hijos secuencialmente. A partir del ultimo, se pasan entre 
* ellos V veces la senial SIGUSR1, de padres a hijos (siendo el hijo del ultimo hijo
* el proceso raiz). Despues, repiten el mismo proceso para SIGTERM, antes de hacer 
* exit(). El padre empieza enviando la senial al hijo, y este a su hijo, etc hasta
* el ultimo hijo, que envia al padre.
* @param: N>0 numero de hijos a crear
* @param: V>0 numero de vueltas a dar por SIGUSR1
*/
int main(int argc, char **argv){
    int i, j, flag, V, N;
    pid_t child, root = getpid();
        
    if(argc < 3){
        perror("Error en los argumentos de entrada: Nhijos,Vvueltas");
        exit(EXIT_FAILURE);
    }
    N = atoi(argv[1]);
    V = atoi(argv[2]);
    if(N < 1 || V < 1){
        perror("Los parametros N,V deben ser estrictamente positivos");
        return -1;
    }
    /*Cambiamos manejadores*/
    if(signal(SIGTERM, sigtermHandler) == SIG_ERR){
        perror("Error al cambiar manejador SIGTERM");
        return -1;
    }
    if(signal(SIGUSR1, sigusr1Handler) == SIG_ERR){
        perror("Error al cambiar manejador SIGUSR1");
        return -1;
    }

    /*Generamos hijos */
    flag = 0;
    for(i = 0; i < N && flag == 0; i++){
        child = fork();
        if(child < 0){
            perror("Error en un fork");
            exit(EXIT_FAILURE);
        }else if (child > 0){
            flag = 1;
        }    
    }

    /*El "hijo" del ultimo hijo es root (es modular), ejecute 1º kill*/
    if(i == N && child == 0){
        sleep(5);
        child = root;
        kill(child, SIGUSR1);
    }    
    /*Cada hijo pause+kill V veces*/
    for(j = 0; j < V; j++){
        if(pause() != -1){
            perror("Error en llamada a pause()");
            exit(EXIT_FAILURE);
        }
        kill(child, SIGUSR1);
    }
    if(pause() != -1){
        perror("Error en la ultima llamada a pause()");
        exit(EXIT_FAILURE);
    }
    sleep(1);
    kill(child, SIGTERM);
    /*El root hace un ultimo pause() para ser matado*/
    if(getpid() == root){
        pause();
    }
    exit(EXIT_SUCCESS);
}
