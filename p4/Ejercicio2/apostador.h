

#define N 1997
#define F "/bin/cat"
#define MTYPE 1

typedef struct _Apuesta Apuesta;

typedef struct _Mensaje{
    long id;
    Apuesta *a;
} Mensaje;

/**
 * Crea apuesta aleatoria
 * @param idApostador numero del apostador
 * @param nCaballos caballos en la carrera
 * @return NULL si error, si no la apuesta creada.
 */
Apuesta *apuesta_ini(int idApostador, int nCaballos);     

/**
 * Funcion que se encarga de generar cada 0.1 segundos apuestas aleatorias
 * y enviarlas a una cola de mensajes que lee el gestor
 * @param nApostadores numero de apostadores
 * @param nCaballos numero de caballos
 */
int generador(int nApostadores, int nCaballos, int colaApuesta, int tipo);

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

/**
 * Obtiene la apuesta dentro del mensaje
 * @param mensaje del que obtener la apuesta
 * @return apuesta del mensaje
 */
 Apuesta *getApuesta(Mensaje m);
 
 
