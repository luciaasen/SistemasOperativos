 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <sys/wait.h>
 #include <string.h>
 #include <sys/types.h>

/**
 *  Funcion main del ejercicio 9
    Pide por pantalla dos valores double que seran almacenados por el proceso padre.
    El proceso padre genera cuatro hijos, a los que envua los valores escaneados a traves de un pipe.
    Cada hijo realiza una operacion y envia el resultado al padre a traves de otro pipe.
    El padre imprime cada uno de los resultados extraidos del pipe.

*/
 int main(){
    pid_t hijos[4];
    int i, nbytes, status;
    int fd1[4][2], fd2[4][2];
    double op1, op2, res;
    char operandos[10], resultado[150], operacion[10];

    printf("Introduce tus dos operandos separados por una coma, por ejemplo '5,6'\n");
    scanf("%s", operandos);
    
    for(i = 0; i < 4; i ++){
        if(pipe(fd1[i]) < 0 || pipe(fd2[i]) < 0){
            perror("Tuberia fallida");
            exit(EXIT_FAILURE);
        }

        if((hijos[i]= fork()) == -1){
            perror("Error en fork 1");
            exit(EXIT_FAILURE);

        }else if(hijos[i] == 0){

            /*Hijo escribe en fd2, lee de fd1*/
            close(fd1[i][1]);
            close(fd2[i][0]);

            if(read(fd1[i][0], operandos, 10) < 0){
                perror("Error al leer operandos en 1"),
                exit(EXIT_FAILURE);
            }
            sscanf(operandos, "%lf,%lf", &op1, &op2);
            switch(i){
                case 0:
                    res = op1 + op2;
                    strcpy(operacion , "Suma");
                    break;
                case 1:
                    res = op1 - op2;
                    strcpy(operacion , "Resta");
                    break;
                case 2:
                    res = op1 * op2;
                    strcpy(operacion , "Producto");
                    break;
                default:
                    res =  op1 / op2;
                    strcpy(operacion , "Division");
            }
            sprintf(resultado, "Datos enviados a través de la tubería por el proces %d.\n Operando 1: %.3lf. Operando 2: %.3lf. %s:%.3lf",  getpid(), op1, op2, operacion, res);
            nbytes = strlen(resultado) + 1;
            if(write(fd2[i][1], resultado, nbytes) < nbytes){
                perror("Escritura incorrecta del resultado");
                exit(EXIT_FAILURE);
            }else{
                exit(EXIT_SUCCESS);
            }
                    
        }else{

            /*padre escribe en fd1, lee de fd2*/
            close(fd1[i][0]);
            close(fd2[i][1]);

            nbytes = 1 + strlen(operandos);
            if(write(fd1[i][1], operandos, nbytes) < 0){
                perror("No se han escrito los operandos en 1");
                exit(EXIT_FAILURE);
            }
            wait(&status);
            if(read(fd2[i][0], resultado, 150) < 0){
                perror("Error al leer resultado");
                exit(EXIT_FAILURE);
            }
            printf("%s\n", resultado);
                       
        }
    }

    return 0;
    
 }
