#include <stdio.h>
#include <string.h>

int main(){
    char string[] = "Hola\nJa";
    int bytes; 
    printf("This is my string:\n");
    bytes = printf("%.5lf,%.2lf", 3.14169,3.14);
    printf("\nThis is size of string: %d\nThis is sizeof chars: 7 + 1 + 3 = 11\n", 
        bytes);
    printf("This is sizeof double: %d\n", sizeof(double));
    printf(string);
    return 0;
}
