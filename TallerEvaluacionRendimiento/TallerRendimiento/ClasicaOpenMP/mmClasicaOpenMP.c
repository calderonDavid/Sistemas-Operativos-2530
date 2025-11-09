/****************************************************************************************
 * Fecha: 03-Nov-2025
 * Autor: David Calderon y Alejandro Lopez
 * Materia: Sistemas Operativos
 * Tema: Taller de Evaluación de Rendimiento
 * Programa: Multiplicación de Matrices algoritmo clásico
 * Versión: Paralelismo con OpenMP
***************************************************************************************/

#include "mmClasicaOpenMP.h"


int main(int argc, char *argv[]){
	// Validación de Argumentos
	if(argc < 3){
		printf("\n Use: $./clasicaOpenMP SIZE Hilos \n\n");
		exit(0);
	}
	int N = atoi(argv[1]);  // Tamaño Matriz
	int TH = atoi(argv[2]); // Número de Hilos
	
    // Se reserva la meemoria de las amtrices
	double *matrixA  = (double *)calloc(N*N, sizeof(double));
	double *matrixB  = (double *)calloc(N*N, sizeof(double));
	double *matrixC  = (double *)calloc(N*N, sizeof(double));

	srand(time(NULL)); // Semilla aleatoria

    // Establece el número de hilos que OpenMP debe usar
	omp_set_num_threads(TH);

    // 5. Inicialización de las matrices A y B, y se imprimen por pantalla
	iniMatrix(matrixA, matrixB, N);
	impMatrix(matrixA, N);
	impMatrix(matrixB, N);
	
	InicioMuestra(); // Inicia el cronometro
	multiMatrix(matrixA, matrixB, matrixC, N); // Llama a la función paralela
	FinMuestra();    // Detiene tiempo

    // Se muestra los resultados
	impMatrix(matrixC, N);

	// Liberación de Memoria
	free(matrixA);
	free(matrixB);
	free(matrixC);
	
	return 0;
}
