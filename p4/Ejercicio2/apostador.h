#ifndef APOSTADOR_H
#define APOSTADOR_H
#define N        1997
#define F        "/bin/cat"
#define MTYPE    199

typedef struct _Apuesta {
    long   id;
    char   nombre[20];
    int    numC;
    double cuantia;
} Apuesta;

/**
 * Genera una apuesta aleatoria con los parametros dados y
 * manda el mensaje a una de las ventanillas
 * @param  tipo        [description]
 * @param  idApostador [description]
 * @param  nCaballos   [description]
 * @param  cola        [description]
 * @return             [description]
 */
int apuestaEnvia(long tipo, int idApostador, int nCaballos, int cola);

/**
 * Funcion que se encarga de generar cada 0.1 segundos apuestas aleatorias
 * y enviarlas a una cola de mensajes que lee el gestor
 * @param nApostadores numero de apostadores
 * @param nCaballos numero de caballos
 */
int generador(int nApostadores, int nCaballos, int colaApuesta, long tipo);

/**
 * @param a Apuesta
 * @return id del caballo
 */
int getCaballo(Apuesta *a);

/**
 * @param a Apuesta
 * @return id del apostador
 */
int getApostador(Apuesta *a);

/**
 * @param a Apuesta
 * @return cuantia de la apuesta
 */
double getCuantia(Apuesta *a);



#endif
