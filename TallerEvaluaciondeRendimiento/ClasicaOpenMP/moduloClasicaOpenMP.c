/****************************************************************************************
* Fecha: 03-Nov-2025
* Autor: David Calderon y Alejandro Lopez
* Materia: Sistemas Operativos
* Tema: Taller de Evaluación de Rendimiento
* Fichero: Módulo de implementación para mmClasicaOpenMP
***************************************************************************************/

#include "mmClasicaOpenMP.h"

// Definición de variables de tiempo
struct timeval inicio, fin; 

// Inicia cronómetro
void InicioMuestra(){
	gettimeofday(&inicio, (void *)0);
}

// Detiene cronómetro e imprime
void FinMuestra(){
	gettimeofday(&fin, (void *)0);
	fin.tv_usec -= inicio.tv_usec;
	fin.tv_sec  -= inicio.tv_sec;
	double tiempo = (double) (fin.tv_sec*1000000 + fin.tv_usec); 
	printf("%9.0f \n", tiempo);
}

// Imprime matriz
void impMatrix(double *matrix, int D){
	if(D < 9){
		printf("\n");
		for(int i=0; i<D*D; i++){
			if(i%D==0) printf("\n");
			printf("%.2f ", matrix[i]);
		}
		printf("\n**-----------------------------**\n");
	}
}

// Inicializa matrices
void iniMatrix(double *m1, double *m2, int D){
	for(int i=0; i<D*D; i++, m1++, m2++){
		*m1 = (double)rand()/RAND_MAX*(5.0-1.0);	
		*m2 = (double)rand()/RAND_MAX*(9.0-2.0);	
	}
}

//Multiplicación con OpenMP
void multiMatrix(double *mA, double *mB, double *mC, int D){
	double Suma, *pA, *pB;
    // Inicia una región paralela
	#pragma omp parallel
	{
    // Directiva OpenMP: divide las iteraciones del bucle 'for' (i) entre los hilos
	#pragma omp for
	for(int i=0; i<D; i++){ // Cada hilo toma un conjunto de 'i's
		for(int j=0; j<D; j++){
			pA = mA+i*D;	
			pB = mB+j; 
			Suma = 0.0;
			for(int k=0; k<D; k++, pA++, pB+=D){
				Suma += *pA * *pB;
			}
			mC[i*D+j] = Suma;
		}
	}
	} // Fin de la región paralela, los hilos se sincronizan
}
