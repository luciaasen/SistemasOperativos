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

//Notifica que se pueden generar los valores de la siguiente ronda a todos
//los caballos usando pipes, SUJETO A CAMBIOS
void siguienteRonda(infoCaballos *info);

//Recibe todas las tiradas de los caballos a traves de MENSAJES
//y actualiza los valores necesarios (valores totales, estados)
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
 * Devuelve el id del caballo ganador.
 * Es independiente de si ha terminado o no la carrera.
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
 * Imprime resultados de la carrera
 * @param info puntero con informacion de la carrera
 */
void imprimeResultados(infoCaballos *info);

/**
 *
 */
void forzarFin(infoCaballos *info);

/**
 * Se encarga de cerrar todos los procesos caballos abiertos y liberar la info
 * dentro de infoCaballos
 * @param info infoCaballos
 */
void finalizaLibera(infoCaballos *info);

#endif