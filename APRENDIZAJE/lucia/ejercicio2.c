#include  <stdio.h>
#include  <string.h>
#include  <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    int i, num, f, f2;
    int a, p;
    f = fork();
    f2 = fork();
    if(f == 0){
        if(f2 == 0){
            num = 0;
        }else{
            num = 100;
        }
        
    }else{
        if(f2 == 0){
            num = 1000;
        }
        else{
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
