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
    do{
        f[i] = fork();

        if(f[i] < 0){
            free(f);
            return -1;
        }else if(f[i] == 0){
            pflag = 0;
            ret = llama_exec(argv[i + 1], argv[argc - 1]
        }
        i++;
    }while( (i < argc-2) && (pflag==1));

    /*waits que hace el padre*/
    do{
        pid = wait(&status);
    }while(pid != -1);
}

int llama_exec(char *file, char *opcion){
    char lista[2];
    if(file == NULL || opcion == NULL){
        return -1;
    }
    switch(opcion){
        case "-l":
            return  execl(file, NULL);
        case "-lp":
            return execlp(file, NULL);
        case "-v":
            lista[0] = file;
            lista[1] = NULL;
            return execv(file, lista);
        case "-vp":
            lsta[0] = file;
            lista[1] = NULL;
            return execv(file, lista);
    }

