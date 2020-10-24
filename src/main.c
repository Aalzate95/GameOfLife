#include <stdio.h>
#include "util.h"
#include "vida.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

//Estructura
juego_de_vida juego;
diccionario dict;
int *figuras_detectadas;
int gen;


void options(juego_de_vida *juego, long *celula, int *n_hilos, int *opcion, int *mostrar, void *argc, char ***argv) {
		while ((*opcion = getopt(*((int *) argc), *argv, "f:c:g:s:i:n:m")) != -1)
	{
		switch (*opcion)
		{

		case 'f':
			juego->filas = atol(optarg);
			break;
		case 'c':
			juego->columnas = atol(optarg);
			break;
		case 'g':
			juego->generaciones = atol(optarg);
			break;
		case 's':
			juego->tiempo_sleep = atol(optarg);
			break;
		case 'i':
			*celula = atol(optarg);

			break;
		case 'n':
			*n_hilos = atoi(optarg);
			break;
		case 'm':
			*mostrar = 1;
			break;
		default:
			printf("Argumento %c invalido\n", *opcion);
			break;
		}
	}
}



int main(int argc, char **argv)
{
	//Esto se invoca cuando el programa se termina presionando ctrl+c
	signal(SIGINT, handle_sigint);
	int opcion = 0, n_hilos;
	int mostrar = 0;
	
	figuras_detectadas = (int *)malloc(sizeof(int) * 6);
	
	
	for (int i = 0; i < 6; i++)
		figuras_detectadas[i] = 0;

	//juego_de_vida copia;

	long celula;
	//char **simulacion;

	
	options(&juego, &celula, &n_hilos, &opcion, &mostrar, &argc, &argv);
	

	if (n_hilos <= 0)
	{
		printf("Faltan parametros o numero de hilos menor a cero!\nUso: ./vida -f num_fil -c num_col -g num_gen -s t_slepp -n hilos -m mostrar_tablero(si o no)\n\n");
		exit(-1);
	}

	if ((juego.filas * juego.columnas) <= celula)
	{
		printf("No puede agregar una cantidad de celulas iniciales que superen al espacio del tablero!\n\n");
		exit(-1);
	}

	/*Validacion*/
	if (juego.filas <= 0 || juego.columnas <= 0 || juego.tiempo_sleep <= 0)
	{
		//exit(0);
		validaciones(&juego);
	}
	if (n_hilos > juego.filas)
	{
		printf("Solo se usaran %ld hilos\n", juego.filas);
		n_hilos = juego.filas;
		printf("ENTER para continuar\n");
		getchar();
	}
	if (celula < 0)
	{
		printf("El numero de celulas vivas no puede ser menor a cero, se procedera a cambiar a su correspondiente valor 			positivo\n");
		celula = labs(celula);
		printf("ENTER para continuar\n");
		getchar();
	}
	
	
	//lleno diccionario - inicio
	dict.inicio = celula;
	dict.nacieron = celula;
	dict.murieron = 0;
	dict.total_n = celula;
	dict.total_m = 0;
	
	
	if (mostrar == 1)
	{
		printf("se va a mostrar la grilla\n");
		printf("ENTER para continuar\n");
		getchar();
	}
	
	juego.tablero = crearSimulacion(juego.filas, juego.columnas);
	llenar_matriz_azar(juego.tablero, juego.filas, juego.columnas, celula);
	//generacion 0
	printf("Inicio\n");
	if (mostrar == 1)
	{
		dibujar_grilla(juego.tablero, juego.filas, juego.columnas);
	}
	gen = 0;
	estadisticas(&dict, gen);
	usleep(juego.tiempo_sleep);
	/*simulacion=reglas(juego.tablero, juego.filas, juego.columnas);
	copia.tablero = simulacion;*/

	hiloT *lHilos[n_hilos];
	hiloT *hilo;
	pthread_t tid[n_hilos];
	hiloFigura *hilosFiguras[6];
	hiloFigura *hFigura;
	pthread_t tid_hfiguras[6];
	int error;
	if (juego.generaciones <= 0)
	{
		//printf("===!GENERACION CONTINUA!=====");
		while (1)
		{
			gen++;
			for (int j = 0; j < n_hilos; j++)
			{
				//************************ seccion de hilos ************************
				hilo = (hiloT *)malloc(sizeof(hiloT));
				hilo->id = j + 1;
				hilo->hilos = n_hilos;
				hilo->juego = &juego;
				hilo->dict = &dict;
				lHilos[j] = hilo;
				error = pthread_create(&tid[j], NULL, reglas, (void *)hilo);
				if (error)
				{
					printf("ERROR\n");
					exit(-1);
				}
				//******************************************************************
				//reglas(&juego, &dict);
			}
			for (int i = 0; i < n_hilos; i++)
			{
				pthread_join(tid[i], NULL);
				free(lHilos[i]);
			}
			if (mostrar == 1)
			{
				dibujar_grilla(juego.tablero, juego.filas, juego.columnas);
			}
			estadisticas(&dict, gen);

			//******SECCION DETECCION DE FIGURAS ****/
			for (int m = 0; m < 6; m++)
			{
				hFigura = (hiloFigura *)malloc(sizeof(hiloFigura));
				hFigura->pos = m;
				hFigura->juego = &juego;
				hFigura->arregloFiguras = figuras_detectadas;
				switch (m)
				{
				case 0:
					hFigura->figura = "bloque";
					break;
				case 1:
					hFigura->figura = "colmena_vertical";
					break;
				case 2:
					hFigura->figura = "colmena_horizontal";
					break;
				case 3:
					hFigura->figura = "hogaza";
					break;
				case 4:
					hFigura->figura = "bote";
					break;
				case 5:
					hFigura->figura = "tina";
					break;
				default:
					break;
				}

				hilosFiguras[m] = hFigura;
				error = pthread_create(&tid_hfiguras[m], NULL, detectar_formas, (void *)hFigura);
				if (error)
				{
					printf("ERROR\n");
					exit(-1);
				}
			}

			for (int n = 0; n < 6; n++)
			{
				pthread_join(tid_hfiguras[n], NULL);
				free(hilosFiguras[n]);
			}
			//estadisticas_figuras(figuras_detectadas, gen);
			usleep(juego.tiempo_sleep);
		}
	}
	else
	{
		for (int i = 0; i < juego.generaciones; i++)
		{
			gen++;
			for (int j = 0; j < n_hilos; j++)
			{
				//************************ seccion de hilos ************************
				hilo = (hiloT *)malloc(sizeof(hiloT));
				hilo->id = j + 1;
				hilo->hilos = n_hilos;
				hilo->juego = &juego;
				hilo->dict = &dict;
				lHilos[j] = hilo;
				error = pthread_create(&tid[j], NULL, reglas, (void *)hilo);
				if (error)
				{
					printf("ERROR\n");
					exit(-1);
				}
				//******************************************************************
				//reglas(&juego, &dict);
			}
			for (int k = 0; k < n_hilos; k++)
			{
				pthread_join(tid[k], NULL);
				free(lHilos[k]);
			}
			if (mostrar == 1)
			{
				dibujar_grilla(juego.tablero, juego.filas, juego.columnas);
			}
			estadisticas(&dict, i + 1);

			//******SECCION DETECCION DE FIGURAS ****/
			for (int m = 0; m < 6; m++)
			{
				hFigura = (hiloFigura *)malloc(sizeof(hiloFigura));
				hFigura->pos = m;
				hFigura->juego = &juego;
				hFigura->arregloFiguras = figuras_detectadas;
				switch (m)
				{
				case 0:
					hFigura->figura = "bloque";
					break;
				case 1:
					hFigura->figura = "colmena_vertical";
					break;
				case 2:
					hFigura->figura = "colmena_horizontal";
					break;
				case 3:
					hFigura->figura = "hogaza";
					break;
				case 4:
					hFigura->figura = "bote";
					break;
				case 5:
					hFigura->figura = "tina";
					break;
				default:
					break;
				}

				hilosFiguras[m] = hFigura;
				error = pthread_create(&tid_hfiguras[m], NULL, detectar_formas, (void *)hFigura);
				if (error)
				{
					printf("ERROR\n");
					exit(-1);
				}
			}

			for (int n = 0; n < 6; n++)
			{
				pthread_join(tid_hfiguras[n], NULL);
				free(hilosFiguras[n]);
			}
			//estadisticas_figuras(figuras_detectadas, gen);
			usleep(juego.tiempo_sleep);
		}
	}

	estadisticas_figuras(figuras_detectadas, gen);
	guadar_resultados(juego.tablero, juego.filas, juego.columnas, &dict, gen, figuras_detectadas);

	//evitar_Memory(copia.tablero, juego.filas);
	/*
	juego.tablero = crearSimulacion(juego.filas, juego.columnas);
	llenar_matriz_azar(juego.tablero, juego.filas, juego.columnas, celula);
	dibujar_grilla(juego.tablero, juego.filas, juego.columnas);*/
	/* --------------------OJO
	--------------------OJO */
	evitar_Memory(juego.tablero, juego.filas);

	printf("Final\n");

	return 0;
}

void handle_sigint(int sig)
{
	estadisticas_figuras(figuras_detectadas, gen);
	guadar_resultados(juego.tablero, juego.filas, juego.columnas, &dict, gen, figuras_detectadas);
	exit(0);
}

