#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>


int llama_exec(char *file, char* opcion);

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

    }else if(strcmp("-lp", opcion) == 0){
        lista[0] = file;
        lista[1] = NULL;
        return execvp(file, lista);

    }else{
        perror("Ningun exec asociado con la opcion");
    }
}
