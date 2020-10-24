
#ifndef UTIL_H
#define UTIL_H
#include "vida.h"
typedef struct dicc
{
	int inicio;
	int nacieron;
	int murieron;
	long total_n;
	long total_m;
} diccionario;

/* typedef struct dicc_figura{
	int bloque;
	int colmena_vertical;
	int colmena_horizontal;
	int hogaza;
	int bote;
	int tina;
} diccionario_figura; */

typedef struct threadTablero
{
	int id;
	int hilos;
	juego_de_vida *juego;
	diccionario *dict;
} hiloT;

typedef struct threadFigura
{
	char *figura;
	int pos;
	int *arregloFiguras;
	juego_de_vida *juego;
} hiloFigura;
/*
	Mensajes con la validaciones correspondientes a los datos de entrada
*/
void validaciones(juego_de_vida *juego);
/*
Funcion que dibuja una grilla en la terminal.
Recibe un arreglo 2D de chars. Si el elemento es el 
numero 0, se muestra un espacio vacio, si es 1, se 
muestra una x.

Cada llamada de esta funcion borrara todo lo mostrado
en pantalla.

*/
void dibujar_grilla(char **matriz, int fil, int col);

/*
Llena matriz con cantidad de 1s en posiciones al azar,
el resto lo llena de 0s.
*/
void llenar_matriz_azar(char **grilla, int fil, int col, int cantidad);

char **crearSimulacion(int filas, int columnas);

void *reglas(void *tab);

char **simulacion(int filas, int columnas);

void estadisticas(diccionario *dict, long generacion);

void evitar_Memory(char **simulacion, int filas);

void *detectar_formas(void *hFiguraJuego);

void estadisticas_figuras(int *conteo_figuras, long generacion);

int guadar_resultados(char **tablero, int fil, int col, diccionario *dict, int generacion, int *conteo_figuras);

void handle_sigint(int sig);
#endif

