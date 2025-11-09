/****************************************************************************************
* Fecha: 03-Nov-2025
* Autor: David Calderon y Alejandro Lopez
* Materia: Sistemas Operativos
* Tema: Taller de Evaluación de Rendimiento
* Fichero: Módulo de implementación para mmClasicaPosix
***************************************************************************************/

#include "mmClasicaPosix.h"

// --- Definición de Variables Globales ---
pthread_mutex_t MM_mutex;
double *matrixA, *matrixB, *matrixC;
struct timeval inicio, fin;

// Inicia medición de tiempo
void InicioMuestra(){
	gettimeofday(&inicio, (void *)0);
}

// Termina medición de tiempo e imprime
void FinMuestra(){
	gettimeofday(&fin, (void *)0);
	fin.tv_usec -= inicio.tv_usec;
	fin.tv_sec  -= inicio.tv_sec;
	double tiempo = (double) (fin.tv_sec*1000000 + fin.tv_usec);
	printf("%9.0f \n", tiempo);
}

// Inicializa matrices con valores aleatorios
void iniMatrix(double *m1, double *m2, int D){ 
	for(int i = 0; i < D*D; i++, m1++, m2++){
			*m1 = (double)rand()/RAND_MAX*(5.0-1.0); 
			*m2 = (double)rand()/RAND_MAX*(9.0-5.0); 
		}	
}

// Imprime matriz pequeña para verificación
void impMatrix(double *matriz, int D){
	if(D < 9){
    		for(int i = 0; i < D*D; i++){
     				if(i%D==0) printf("\n");
            		printf(" %.2f ", matriz[i]);
			}	
    	printf("\n>-------------------->\n");
	}
}

// Función principal de multiplicación de matrices (porción)
void *multiMatrix(void *variables){
	// Recibe y "castea" los argumentos
	struct parametros *data = (struct parametros *)variables;
	
    // Extrae los datos de la estructura
	int idH		= data->idH;
	int nH		= data->nH;
	int D		= data->N;
    // Calcula el rango de filas que le toca
	int filaI	= (D/nH)*idH;
	int filaF	= (D/nH)*(idH+1);
	double Suma, *pA, *pB;

    // Ejecuta la multiplicación (idéntica a la versión Fork)
    for (int i = filaI; i < filaF; i++){
        for (int j = 0; j < D; j++){
			pA = matrixA + i*D; // Hilos acceden a la matrizA global
			pB = matrixB + j; // Hilos acceden a la matrizB global
			Suma = 0.0;
            for (int k = 0; k < D; k++, pA++, pB+=D){
				Suma += *pA * *pB;
			}
			matrixC[i*D+j] = Suma; // Hilos escriben en la matrizC global
		}
	}
	
	pthread_mutex_lock (&MM_mutex);
	pthread_mutex_unlock (&MM_mutex);
	pthread_exit(NULL); // El hilo termina
}
