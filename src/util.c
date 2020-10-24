#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <util.h>
#include <vida.h>
#include <math.h>
//struct juego_de_vida juego;

//muestra el tablero
void dibujar_grilla(char **matriz, int fil, int col)
{

	printf("\e[1;1H\e[2J");
	printf(" ");
	for (int i = 0; i < fil; i++)
	{
		printf("-");
	}
	printf("\n");
	char *linea = malloc(col + 1); //Char nulo al final
	for (int i = 0; i < fil; i++)
	{
		memset(linea, ' ', col + 1);
		linea[col] = 0;
		printf("-");
		for (int j = 0; j < col; j++)
		{
			if (matriz[i][j] == 0)
			{
				linea[j] = ' ';
			}
			else if (matriz[i][j] == 1)
			{
				linea[j] = 'x';
			}
		}
		printf("%s", linea);
		printf("\n");
		fflush(stdout);
	}
	for (int i = 0; i < fil + 1; i++)
	{
		printf("-");
	}
	printf("\n");
	free(linea);
}

//mensajes de error para argumentos de entrada
void validaciones(juego_de_vida *juego)
{
	if ((juego->filas == 0) || (juego->columnas == 0))
	{
		printf("El tablero no puede ser creado con 0 filas o 0 columnas. \nSaliendo del programa\n");
		exit(0);
	}
	if ((juego->filas) < 0)
	{
		printf("El numero de filas ingresado es menor a cero, se cambia a positivo para que pueda ingresar\n");
		juego->filas = labs(juego->filas);
	}
	if ((juego->columnas) < 0)
	{
		printf("El numero de columnas ingresado es menor a cero, se cambia a positivo para que pueda ingresar\n");
		juego->columnas = labs(juego->columnas);
	}
	if ((juego->tiempo_sleep) < 0)
	{
		printf("El tiempo de retardo es menor a cero, se cambiara a un valor positivo para que pueda ingresar\n");
		juego->tiempo_sleep = labs(juego->tiempo_sleep);
	}
	printf("Presione ENTER para continuar\n");
	getchar();
}

//separa memoria para el tablero
char **crearSimulacion(int filas, int columnas)
{
	char **simulacion = (char **)malloc(filas * sizeof(char *));
	if (simulacion == NULL)
	{
		exit(0);
	}
	for (int i = 0; i < filas; i++)
	{
		simulacion[i] = (char *)malloc(columnas * sizeof(char));
	}
	return simulacion;
}

//liberado de memoria dinamica malloc
void evitar_Memory(char **simulacion, int filas)
{
	for (int i = 0; i < filas; i++)
	{
		free(simulacion[i]);
	}
	free(simulacion);
}

//llena la tabla al azar
void llenar_matriz_azar(char **grilla, int fil, int col, int cantidad)
{
	for (int i = 0; i < fil; i++)
	{
		memset(grilla[i], 0, col);
	}
	srand(time(NULL));
	for (int i = 0; i < cantidad; i++)
	{
		long rnd_fil = rand() % fil;
		long rnd_col = rand() % col;

		//printf("%ld %ld\n", rnd_fil, rnd_col);
		if (grilla[rnd_fil][rnd_col] == 1)
		{
			i--;
		}
		else
		{
			grilla[rnd_fil][rnd_col] = 1;
		}
	}
}

//void reglasEnv(){

//}
//funcion donde nacen y mueren
void *reglas(void *tab)
{
	//double **m1, **m2, **res;
	long filas, columnas, filas_h;
	int id, hilos;
	hiloT *arg = (hiloT *)tab;

	filas = arg->juego->filas;
	columnas = arg->juego->columnas;
	id = arg->id;
	hilos = arg->hilos;
	filas_h = filas / hilos;
	int dif = filas - (filas_h * hilos);
	if ((dif > 0) && (dif <= id))
	{
		filas_h++;
	}
	int cont_nac = 0, cont_mur = 0, vecinos;
	char **nextGen = (char **)malloc(filas_h * sizeof(char *));
	for (int r = 0; r < filas_h; r++)
	{
		nextGen[r] = (char *)malloc(columnas * sizeof(char));
	}
	if (nextGen == NULL)
	{
		printf("NULL\n");
		exit(0);
	}
	//condiciones de posicion
	int ini_x, ini_y, fin_x, fin_y;
	for (int i = 0; i < filas_h; i++)
	{
		for (int j = 0; j < columnas; j++)
		{
			//validacion de esquinas
			if ((i * hilos) + id - 1 == 0)
			{
				ini_x = (i * hilos) + id - 1;
			}
			else
			{
				ini_x = (i * hilos) + id - 2;
			}
			if ((i * hilos) + id - 1 == filas - 1)
			{
				fin_x = (i * hilos) + id - 1;
			}
			else
			{
				fin_x = (i * hilos) + id;
			}
			if (j == 0)
			{
				ini_y = j;
			}
			else
			{
				ini_y = j - 1;
			}
			if (j == columnas - 1)
			{
				fin_y = j;
			}
			else
			{
				fin_y = j + 1;
			}
			vecinos = 0;
			for (int x = ini_x; x <= fin_x; x++)
			{
				for (int y = ini_y; y <= fin_y; y++)
				{

					if (arg->juego->tablero[x][y] == 1)
					{
						vecinos++;
					}
				} //printf("\n");
			}	 //printf("\n");
			//Aplicacion de reglas
			if (arg->juego->tablero[(i * hilos) + id - 1][j] == 1)
			{
				vecinos--;
				cont_nac++; //nace
				if (vecinos <= 1)
				{
					//Muerte por soledad
					//printf("soledad %d\n", vecinos);
					nextGen[i][j] = 0;
				}
				else if (vecinos >= 4)
				{
					//Muerte por sobrepoblacion
					//printf("sobrepoblacion %d\n", vecinos);
					nextGen[i][j] = 0;
				}
				else if ((vecinos == 2) || (vecinos == 3))
				{
					//Sobrevive la celula
					//printf("sobrevive %d\n", vecinos);
					nextGen[i][j] = 1;
				}
			}
			else if ((arg->juego->tablero[(i * hilos) + id - 1][j] == 0) && (vecinos == 3))
			{
				//Nace ya que tengo 3 vecinos
				//printf("vecinos de muerto %d\n", vecinos);
				nextGen[i][j] = 1;
				cont_mur++; //muerte
			}
			else
			{
				nextGen[i][j] = arg->juego->tablero[(i * hilos) + id - 1][j];
			}
		}
	}
	arg->dict->nacieron = cont_nac;
	arg->dict->murieron = cont_mur;
	arg->dict->total_n = arg->dict->total_n + cont_nac;
	arg->dict->total_m = arg->dict->total_m + cont_mur;
	for (int i = 0; i < filas_h; i++)
	{
		for (int j = 0; j < columnas; j++)
		{
			arg->juego->tablero[(i * hilos) + id - 1][j] = nextGen[i][j];
		}
	}
	evitar_Memory(nextGen, filas_h);
	return NULL;
}

///print de los resultados
void estadisticas(diccionario *dict, long generacion)
{
	long double promedio_cn, promedio_cm;
	if (generacion != 0)
	{
		promedio_cn = (dict->total_n - dict->inicio) / generacion;
		promedio_cm = (dict->total_m) / generacion;
	}
	else
	{
		promedio_cn = 0;
		promedio_cm = 0;
	}
	printf("Generacion: %li\n", generacion);
	printf("\tNúmero de células que nacieron en esta generacion: %i\n", dict->nacieron);
	printf("\tNúmero de células que murieron en esta generacion: %i\n", dict->murieron);
	printf("\tNúmero de células que han nacido desde generacion 0: %li\n", dict->total_n);
	printf("\tNúmero de células que han muerto desde generacion 0: %li\n", dict->total_m);
	printf("\tPromedio de celulas que han nacido por generacion: %.2Lf\n", promedio_cn);
	printf("\tPromedio de celulas que han muerto por generacion: %.2Lf\n", promedio_cm);
}

void *detectar_formas(void *hFiguraJuego)
{
	hiloFigura *struct_hf = (hiloFigura *)hFiguraJuego;
	//printf("====> struct figura: %s %d %d \n", struct_hf->figura, struct_hf->pos, struct_hf->arregloFiguras[struct_hf->pos]);

	int filas = struct_hf->juego->filas;
	int columnas = struct_hf->juego->columnas;
	int detecciones = 0;

	/* Deteccion de bloque */
	if (strcmp("bloque", struct_hf->figura) == 0)
	{
		int limite_fila = filas - 1;
		int limite_columna = columnas - 1;

		for (int i = 0; i < limite_fila; i++)
		{
			for (int j = 0; j < limite_columna; j++)
			{
				if (struct_hf->juego->tablero[i][j] == 1 && struct_hf->juego->tablero[i][j + 1] == 1 && struct_hf->juego->tablero[i + 1][j] == 1 && struct_hf->juego->tablero[i + 1][j + 1] == 1)
				{
					//printf("-----> HILO %d detecto forma de BLOQUE\n", struct_hf->pos);
					detecciones++;
				}
			}
		}
	}
	/* Deteccion de colmena vertical */
	else if (strcmp("colmena_vertical", struct_hf->figura) == 0)
	{
		int limite_fila = filas - 3;
		int limite_columnas = columnas - 1;

		for (int i = 0; i < limite_fila; i++)
		{
			for (int j = 1; j < limite_columnas; j++)
			{
				if (struct_hf->juego->tablero[i][j] == 1 &&
					struct_hf->juego->tablero[i + 1][j - 1] == 1 &&
					struct_hf->juego->tablero[i + 1][j + 1] == 1 &&
					struct_hf->juego->tablero[i + 2][j - 1] == 1 &&
					struct_hf->juego->tablero[i + 2][j + 1] == 1 &&
					struct_hf->juego->tablero[i + 3][j] == 1 &&
					struct_hf->juego->tablero[i][j - 1] == 0 &&
					struct_hf->juego->tablero[i][j + 1] == 0 &&
					struct_hf->juego->tablero[i + 3][j - 1] == 0 &&
					struct_hf->juego->tablero[i + 3][j + 1] == 0 &&
					struct_hf->juego->tablero[i + 1][j] == 0 &&
					struct_hf->juego->tablero[i + 2][j] == 0)
				{
					//printf("-----> HILO %d detecto forma de COLMENA VERTICAL\n", struct_hf->pos);
					detecciones++;
				}
			}
		}
	}
	/* Deteccion de colmena horizontal */
	else if (strcmp("colmena_horizontal", struct_hf->figura) == 0)
	{
		int limite_fila = filas - 1;
		int limite_columnas = columnas - 3;

		for (int i = 1; i < limite_fila; i++)
		{
			for (int j = 0; j < limite_columnas; j++)
			{
				if (struct_hf->juego->tablero[i][j] == 1 &&
					struct_hf->juego->tablero[i - 1][j + 1] == 1 &&
					struct_hf->juego->tablero[i + 1][j + 1] == 1 &&
					struct_hf->juego->tablero[i - 1][j + 2] == 1 &&
					struct_hf->juego->tablero[i + 1][j + 2] == 1 &&
					struct_hf->juego->tablero[i][j + 3] == 1 &&
					struct_hf->juego->tablero[i - 1][j] == 0 &&
					struct_hf->juego->tablero[i + 1][j] == 0 &&
					struct_hf->juego->tablero[i - 1][j + 3] == 0 &&
					struct_hf->juego->tablero[i + 1][j + 3] == 0 &&
					struct_hf->juego->tablero[i][j + 1] == 0 &&
					struct_hf->juego->tablero[i][j + 2] == 0)
				{
					//printf("-----> HILO %d detecto forma de COLMENA HORIZONTAL\n", struct_hf->pos);
					detecciones++;
				}
			}
		}
	}
	/* Deteccion de hogaza */
	else if (strcmp("hogaza", struct_hf->figura) == 0)
	{
		int limite_fila = filas - 2;
		int limite_columnas = columnas - 3;

		for (int i = 1; i < limite_fila; i++)
		{
			for (int j = 0; j < limite_columnas; j++)
			{
				if (struct_hf->juego->tablero[i][j] == 1 &&
					struct_hf->juego->tablero[i - 1][j + 1] == 1 &&
					struct_hf->juego->tablero[i + 1][j + 1] == 1 &&
					struct_hf->juego->tablero[i - 1][j + 2] == 1 &&
					struct_hf->juego->tablero[i + 1][j + 2] == 0 &&
					struct_hf->juego->tablero[i][j + 3] == 1 &&
					struct_hf->juego->tablero[i - 1][j] == 0 &&
					struct_hf->juego->tablero[i + 1][j] == 0 &&
					struct_hf->juego->tablero[i - 1][j + 3] == 0 &&
					struct_hf->juego->tablero[i + 1][j + 3] == 1 &&
					struct_hf->juego->tablero[i][j + 1] == 0 &&
					struct_hf->juego->tablero[i][j + 2] == 0 &&
					struct_hf->juego->tablero[i + 2][j] == 0 &&
					struct_hf->juego->tablero[i + 2][j + 1] == 0 &&
					struct_hf->juego->tablero[i + 2][j + 2] == 1 &&
					struct_hf->juego->tablero[i + 2][j + 3] == 0)
				{

					//printf("-----> HILO %d detecto forma de HOGAZA\n", struct_hf->pos);
					detecciones++;
				}
			}
		}
	}
	/* Deteccion de bote */
	else if (strcmp("bote", struct_hf->figura) == 0)
	{
		int limite_fila = filas - 2;
		int limite_columna = columnas - 2;

		for (int i = 0; i < limite_fila; i++)
		{
			for (int j = 0; j < limite_columna; j++)
			{
				if (struct_hf->juego->tablero[i][j] == 1 &&
					struct_hf->juego->tablero[i][j + 1] == 1 &&
					struct_hf->juego->tablero[i + 1][j] == 1 &&
					struct_hf->juego->tablero[i + 1][j + 2] == 1 &&
					struct_hf->juego->tablero[i + 2][j + 1] == 1 &&
					struct_hf->juego->tablero[i + 1][j + 1] == 0 &&
					struct_hf->juego->tablero[i][j + 2] == 0 &&
					struct_hf->juego->tablero[i + 2][j] == 0 &&
					struct_hf->juego->tablero[i + 2][j + 2] == 0)
				{
					//printf("-----> HILO %d detecto forma de BOTE\n", struct_hf->pos);
					detecciones++;
				}
			}
		}
	}
	/* Deteccion de tina */
	else if (strcmp("tina", struct_hf->figura) == 0)
	{
		int limite_fila = filas - 2;
		int limite_columna = columnas - 2;

		for (int i = 0; i < limite_fila; i++)
		{
			for (int j = 0; j < limite_columna; j++)
			{
				if (struct_hf->juego->tablero[i][j] == 0 &&
					struct_hf->juego->tablero[i][j + 1] == 1 &&
					struct_hf->juego->tablero[i + 1][j] == 1 &&
					struct_hf->juego->tablero[i + 1][j + 2] == 1 &&
					struct_hf->juego->tablero[i + 2][j + 1] == 1 &&
					struct_hf->juego->tablero[i + 1][j + 1] == 0 &&
					struct_hf->juego->tablero[i][j + 2] == 0 &&
					struct_hf->juego->tablero[i + 2][j] == 0 &&
					struct_hf->juego->tablero[i + 2][j + 2] == 0)
				{
					//printf("-----> HILO %d detecto forma de TINA\n", struct_hf->pos);
					detecciones++;
				}
			}
		}
	}
	//printf("@@@@@ figuras de %s detectadas: %d @@@@@\n", struct_hf->figura, detecciones);
	struct_hf->arregloFiguras[struct_hf->pos] = detecciones;
	return NULL;
}

void estadisticas_figuras(int *conteo_figuras, long generacion)
{
	printf("==============================\n");
	printf("Formas detectadas: \n\n");
	printf("\tBloque: %d\n", conteo_figuras[0]);
	printf("\tColmena vertical: %d\n", conteo_figuras[1]);
	printf("\tColmena horizontal: %d\n", conteo_figuras[2]);
	printf("\tHogaza: %d\n", conteo_figuras[3]);
	printf("\tBote: %d\n", conteo_figuras[4]);
	printf("\tTina: %d\n", conteo_figuras[5]);
	printf("==============================\n");
}

int guadar_resultados(char **tablero, int fil, int col, diccionario *dict, int generacion, int *conteo_figuras)
{
	FILE *fp;
	fp = fopen("resultados.txt", "w");
	if (fp == NULL)
	{
		perror("No se puede escribir en el archivo! ");
		return -1;
	}

	/*EScritura del tablero */
	/*fprintf(fp, "\e[1;1H\e[2J");
	fprintf(fp, " ");
	for (int i = 0; i < fil; i++)
	{
		fprintf(fp, "-");
	}
	fprintf(fp, "\n");
	char *linea = malloc(col + 1); //Char nulo al final
	for (int i = 0; i < fil; i++)
	{
		memset(linea, ' ', col + 1);
		linea[col] = 0;
		fprintf(fp, "-");
		for (int j = 0; j < col; j++)
		{
			if (tablero[i][j] == 0)
			{
				linea[j] = ' ';
			}
			else if (tablero[i][j] == 1)
			{
				linea[j] = 'x';
			}
		}
		fprintf(fp, "%s", linea);
		fprintf(fp, "\n");
		//fflush(stdout);
	}
	for (int i = 0; i < fil + 1; i++)
	{
		fprintf(fp, "-");
	}
	fprintf(fp, "\n");
	free(linea);
	*/

	/*Escritura de los resultados */
	long double promedio_cn, promedio_cm;
	if (generacion != 0)
	{
		promedio_cn = (dict->total_n - dict->inicio) / generacion;
		promedio_cm = (dict->total_m) / generacion;
	}
	else
	{
		promedio_cn = 0;
		promedio_cm = 0;
	}
	fprintf(fp, "Generacion: %i\n", generacion);
	fprintf(fp, "\tNúmero de células que nacieron en esta generacion: %i\n", dict->nacieron);
	fprintf(fp, "\tNúmero de células que murieron en esta generacion: %i\n", dict->murieron);
	fprintf(fp, "\tNúmero de células que han nacido desde generacion 0: %li\n", dict->total_n);
	fprintf(fp, "\tNúmero de células que han muerto desde generacion 0: %li\n", dict->total_m);
	fprintf(fp, "\tPromedio de celulas que han nacido por generacion: %.2Lf\n", promedio_cn);
	fprintf(fp, "\tPromedio de celulas que han muerto por generacion: %.2Lf\n", promedio_cm);

	/*Escritura de resultados de figuras */
	fprintf(fp, "==============================\n");
	fprintf(fp, "Formas detectadas: \n\n");
	fprintf(fp, "\tBloque: %d\n", conteo_figuras[0]);
	fprintf(fp, "\tColmena vertical: %d\n", conteo_figuras[1]);
	fprintf(fp, "\tColmena horizontal: %d\n", conteo_figuras[2]);
	fprintf(fp, "\tHogaza: %d\n", conteo_figuras[3]);
	fprintf(fp, "\tBote: %d\n", conteo_figuras[4]);
	fprintf(fp, "\tTina: %d\n", conteo_figuras[5]);
	fprintf(fp, "==============================\n");

	fclose(fp);
	return 0;
}
