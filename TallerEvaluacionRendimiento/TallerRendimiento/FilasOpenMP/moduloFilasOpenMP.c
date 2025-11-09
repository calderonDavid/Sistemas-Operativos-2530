/****************************************************************************************
* Fecha: 03-Nov-2025
* Autor: David Calderon y Alejandro Lopez
* Materia: Sistemas Operativos
* Tema: Taller de Evaluación de Rendimiento
* Fichero: Módulo de implementación para mmFilasOpenMP
***************************************************************************************/

#include "mmFilasOpenMP.h"

struct timeval inicio, fin; 

// Inicia cronómetro
void InicioMuestra(){
	gettimeofday(&inicio, (void *)0);
}

// Detiene cronómetro
void FinMuestra(){
	gettimeofday(&fin, (void *)0);
	fin.tv_usec -= inicio.tv_usec;
	fin.tv_sec  -= inicio.tv_sec;
	double tiempo = (double) (fin.tv_sec*1000000 + fin.tv_usec); 
	printf("%9.0f \n", tiempo);
}

// Imprime matriz con dos formatos posibles
void impMatrix(double *matrix, int D, int t){
	int aux = 0;
	if(D < 6)
		switch(t){
			case 0: // Formato normal
				for(int i=0; i<D*D; i++){
					if(i%D==0) printf("\n");
						printf("%.2f ", matrix[i]);
				}
				printf("\n  - \n");
				break;
			case 1: // Formato especial
				while(aux<D){
					for(int i=aux; i<D*D; i+=D)
						printf("%.2f ", matrix[i]);
					aux++;
					printf("\n");
				}	
				printf("\n  - \n");
				break;
			default:
				printf("Sin tipo de impresión\n");
		}
}

// Inicializa matrices
void iniMatrix(double *m1, double *m2, int D){
	for(int i=0; i<D*D; i++, m1++, m2++){
		*m1 = (double)rand()/RAND_MAX*(5.0-1.0);	
		*m2 = (double)rand()/RAND_MAX*(9.0-5.0);		
	}
}

// Multiplicación usando acceso por filas (optimizado)
void multiMatrixTrans(double *mA, double *mB, double *mC, int D){
	double Suma, *pA, *pB;
    // Paralelización con OpenMP
	#pragma omp parallel
	{
	#pragma omp for
	for(int i=0; i<D; i++){
		for(int j=0; j<D; j++){
			pA = mA+i*D;	
			pB = mB+j*D; // Acceso por filas en B también (porque está transpuesta implícitamente o se trata como tal)
			Suma = 0.0;
			for(int k=0; k<D; k++, pA++, pB++){
				Suma += *pA * *pB;
			}
			mC[i*D+j] = Suma;
		}
	}
	}
}
