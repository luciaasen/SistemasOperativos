#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

void execute(char *func, char *flag);
/**
 * Funcion principal que se encarga de crear procesos hijos para ejecutar
 * los comandos pasados como argumentos.
 * Nota importante: Si se quiere ejecutar un proceso dado solo su nombre
 *                  debido a que esta en PATH, se tiene que ejecutar con
 *                  el comando correcto (execvp, execlp).
 * @param  argv lista de comandos a ejecutar y de ultimo un indicador de la
 *              funcion que se quiere utilizar para ejecutar los comandos.
 */
int main(int argc, char *argv[]){
    int procs;
    int i;
    if (argc < 3) {
        perror("Argumentos insuficientes\n");
        exit(EXIT_FAILURE);
    }
    procs = argc - 2;
    /*Ejecutamos todos los programas en procesos hijos*/
    for (i = 0; i < procs; i++) {
        if (fork() == 0) {
            execute(argv[i + 1], argv[argc - 1]);
            perror("Error en la funcion execute");
            exit(EXIT_FAILURE);
        }
    }
    /*Esperamos a que todos los procesos hijos cierren*/
    for (i = 0; i < procs; i++) {
        wait(NULL);
    }
    exit(EXIT_SUCCESS);
}

/**
 * Esta funcion se encarga de ejecutar un comando con la funcion indicada
 * a traves del flag.
 * @param func indica el nombre del comando a ejecutar o su path.
 * @param flag indica que funcion se utilizara para ejecutar func.
 */
void execute(char *func, char *flag){
    int  l;
    char *prog[2];
    prog[0] = func;
    prog[1] = (char *) NULL;
    l       = strlen(flag);
    if (l == 2) {
        switch (flag[1]) {
        case 'l':
            execl(func, func, (char *) NULL);
            break;
        case 'v':
            execv(func, prog);
            break;
        default:
            return;
        }
    }else if (l == 3) {
        switch (flag[1]) {
        case 'l':
            execlp(func, func, (char *) NULL);
            break;
        case 'v':
            execvp(func, prog);
            break;
        default:
            return;
        }
    }

    return;
}
