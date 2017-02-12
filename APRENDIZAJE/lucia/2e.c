#include  <stdio.h>
#include  <string.h>
#include  <sys/types.h>
#include  <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <libexplain/waitpid.h>

int main(){
    int i, num, status;
    pid_t f, f2, cpid, a, p;
    printf("Hola soy hang %d trace %d cont %d\n",  WNOHANG, WUNTRACED, WCONTINUED);
    f = fork();
    if(f == -1){
        printf("error en el primer fork\n");
        exit(EXIT_FAILURE);
    }
    f2 = fork();
    if(f2 == -1){
        printf("error en el segundo fork\n");
        exit(EXIT_FAILURE);
    }
    if(f == 0){
        if(f2 == 0){
            printf("Soy el proceso que no espera  a nadie,\nHijo de %d\n",getppid());
            
            num = 0;
        }else{
            cpid = waitpid(f2, &status, 0);
            if (cpid == -1){
                exit(EXIT_FAILURE);
            }
            printf("El proceso %d ha waiteado a su hijo %d, pido q espere a f2 %d\n", getpid(), cpid, f2);
            num = 100;
        }
        
    }else{
        if(f2 == 0){
            printf("Voy a pedir q %d espere a %d, que deberoa ser el de 100\n",getpid(), f);
            if (waitpid(f, &status, 0) < 0){
                fprintf(stderr, "%s\n", explain_waitpid(f, &status, 0));
                exit(EXIT_FAILURE);
            }
            printf("Yo no tengo hijos, soy el hijo de  %d\n pero quiero esperar al de 100, %d (%d)\n", getpid(), f, cpid);
            num = 1000;
        }
        else{
            cpid = waitpid(f2, &status, 0);
            if (cpid == -1){
                exit(EXIT_FAILURE);
            }
            printf("El proceso %d ha waiteado a su hijo %d\n", getpid(), cpid);
            num = 10000;
        }
    }
    
    a = getpid();
    p = getppid();
    for(i = num; i - num < 10; i++){
        printf("%d proceso %d padre %d\n", i, a, p);
    }
    
    exit(EXIT_SUCCESS);
    
}
