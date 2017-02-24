#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>


int llama_exec(char *file, char* opcion);



/**
*   Main encargado de crear tantos hijos de un mismo padre como programas a ejecutar
*   se pasen como argumento de entrada.
*   Cada hijo realiza una llamada a llama_exec con la opcion y el programa adecuado.
*   @param argv: lista de programas a ejecutar + opcion de exec a ejecutar (-l/-lp/-v/-vp)
*   OJO: Si se llama con las opciones -vp/-lp, la lista de programas debe contener
*   el path completo a cada programa.
*/

int main(int argc, char**argv){
    int *f;
    int i;
    int pflag, ret;
    int status, pid;

    if(argc < 3){
        return -1;
    }

    f = (int *)malloc(sizeof(int) * (argc - 2));
    if(f == NULL){
        return -1;
    }

    i = 0;
    pflag = 1;
    do{
        f[i] = fork();

        if(f[i] < 0){
            free(f);
            return -1;
        }else if(f[i] == 0){
            pflag = 0;
            ret = llama_exec(argv[i + 1], argv[argc - 1]);
            if(ret < 0){
                perror("Llamada a exec fallida.\n");
            }
        }
        i++;
    }while( (i < argc-2) && (pflag==1));

    /*waits que hace el padre*/
    do{
        pid = wait(&status);
    }while(pid != -1);
}

/**
*    Funcion que llama al exec apropiado segun opcion y ejecuta el contenido de file.
*    @param file: fichero con el programa a ejecutar.
*    @param opcion: '-l' para execl, '-lp' para execlp, '-v' para execv, '-vp' para execvp
*    
*/

int llama_exec(char *file, char *opcion){
    char *lista[2];
    if(file == NULL || opcion == NULL){
        return -1;
    }

    if(strcmp("-l", opcion) == 0){
        return  execl(file, file, (char*)NULL);
    
    }else if (strcmp("-lp", opcion) == 0){
        return execlp(file, file, (char*)NULL);

    }else if(strcmp("-v", opcion) == 0){
        lista[0] = file;
        lista[1] = NULL;
        return execv(file, lista);

    }else if(strcmp("-vp", opcion) == 0){
        lista[0] = file;
        lista[1] = NULL;
        return execvp(file, lista);

    }else{
        perror("Ningun exec asociado con la opcion");
    }
}
