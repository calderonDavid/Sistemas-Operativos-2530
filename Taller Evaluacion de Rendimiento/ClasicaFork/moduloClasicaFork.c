/****************************************************************************************
** Fecha: 03-Nov-2025
** Autor: David Calderon y Alejandro Lopez
** Materia: Sistemas Operativos
** Tema: Taller de Evaluación de Rendimiento
** Fichero: Módulo de implementación para mmClasicaFork
***************************************************************************************/

#include "mmClasicaFork.h"

struct timeval inicio, fin;

// Captura el tiempo inicial
void InicioMuestra(){
	gettimeofday(&inicio, (void *)0);
}

// Captura el tiempo final, calcula la diferencia e imprime en microsegundos
void FinMuestra(){
	gettimeofday(&fin, (void *)0);
	fin.tv_usec -= inicio.tv_usec;
	fin.tv_sec  -= inicio.tv_sec;
	double tiempo = (double) (fin.tv_sec*1000000 + fin.tv_usec);
	printf("%9.0f \n", tiempo);
}

// Multiplica las filas desde filaI hasta filaF de la matriz A por la matriz B
void multiMatrix(double *mA, double *mB, double *mC, int D, int filaI, int filaF) {
	double Suma, *pA, *pB;
    for (int i = filaI; i < filaF; i++) {
        for (int j = 0; j < D; j++) {
			Suma = 0.0;
			pA = mA+i*D; // Puntero a la fila i de A
			pB = mB+j;   // Puntero a la columna j de B
            for (int k = 0; k < D; k++, pA++, pB+=D) {
				Suma += *pA * *pB; // Producto punto	
            }
			mC[i*D+j] = Suma; // Guarda el resultado en C
        }
    }
}

// Imprime la matriz si su tamaño es menor a 9x9
void impMatrix(double *matrix, int D) {
	if (D < 9) {
    	printf("\nImpresión	...\n");
    	for (int i = 0; i < D*D; i++, matrix++) {
			if(i%D==0) printf("\n");
            	printf(" %.2f ", *matrix);
        	}
        printf("\n ");
    }
}

// Llena las matrices A y B con números aleatorios
void iniMatrix(double *mA, double *mB, int D){
	for (int i = 0; i < D*D; i++, mA++, mB++){
            *mA = (double)rand()/RAND_MAX*(5.0-1.0); ; 
            *mB = (double)rand()/RAND_MAX*(9.0-5.0); 
        }
}
