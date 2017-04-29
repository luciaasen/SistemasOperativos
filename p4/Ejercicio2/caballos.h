#ifndef CABALLOS_H
#define CABALLOS_H

#define TRUE     1
#define FALSE    0
typedef struct _infoCaballos   infoCaballos;

/**
 * Inicializa la informacion sobre los caballos y genera los procesos
 * pertinentes
 * @param  num         numero de caballos
 * @param  longCarrera longitud de la carrera
 * @return             puntero donde se guarda toda la informacion sobre los caballos
 */
infoCaballos *inicializaCaballos(int num, int longCarrera);

/**
 * Notifica con una senyal a todos los caballos de que pueden esperar la
 * informacion de la siguiente ronda. Luego envia a traves de los pipes
 * la informacion de los estados
 * @param info puntero con la informacion de los caballos
 */
void siguienteRonda(infoCaballos *info);

/**
 * Recibe a traves de mensajes las tiradas de los caballos y  actualiza
 * los valores totales y los estados de todos los caballos
 * @param info puntero con la descripcion de los caballos
 */
void actualizaRonda(infoCaballos *info);

//Imprime la informacion de la ronda, sujeto a cambios
void imprimeRonda(infoCaballos *info);

/**
 * Informa de si hay un ganador
 * @param  info informacion sobre los caballos
 * @return      TRUE si hay ganador, FALSE si no
 */
int hayGanador(infoCaballos *info);

/**
 * Dado informacion sobre los caballos y el id de un caballo devuelve su posicion
 * en la carrera
 * @return      posicion del caballo o -1 en caso de error
 */
int posicionCaballo(infoCaballos *info, int id);

/**
 * Devuelve el id del caballo ganador (1 ,2, 3...).
 * Es independiente de si ha terminado o no la carrera
 * -1 en caso de error.
 */
int idPrimerLugar(infoCaballos *info);


/**
 * Devuelve el id del caballo en segundo lugar.
 * Es independiente de si ha terminado o no la carrera.
 */
int idSegundoLugar(infoCaballos *info);


/**
 * Devuelve el id del caballo en tercer lugar.
 * Es independiente de si ha terminado o no la carrera.
 */
int idTercerLugar(infoCaballos *info);

/**
 * Devuelve el id del caballo en ultimo lugar.
 * Es independiente de si ha terminado o no la carrera.
 */
int idUltimoLugar(infoCaballos *info);



/**
 * Imprime resultados de la carrera
 * @param info puntero con informacion de la carrera
 */
void imprimeResultados(infoCaballos *info);

/**
 * Funcion que se encarga de forzar el fin de la carrera.
 * Esta funcion se ejecutara cuando se mande un senal SIGINT
 * @param info informacion de los caballos
 */
void forzarFin(infoCaballos *info);

/**
 * Se encarga de cerrar todos los procesos caballos abiertos y liberar la info
 * dentro de infoCaballos
 * @param info infoCaballos
 */
void finalizaLibera(infoCaballos *info);

#endif