/****************************************************************************************
 * Fecha: 03-Nov-2025
 * Autor: David Calderon y Alejandro Lopez
 * Materia: Sistemas Operativos
 * Tema: Taller de Evaluación de Rendimiento
 * Programa: Multiplicación de Matrices algoritmo matriz Transpuesta (Filas x Filas) 
 * Versión: Paralelismo con OpenMP
 ****************************************************************************************/

#include "mmFilasOpenMP.h"

int main(int argc, char *argv[]){
	// Validación de Argumentos
	if(argc < 3){
		printf("\n Use: $./clasicaOpenMP SIZE Hilos \n\n");
		exit(0);
	}

    // Lectura de Argumentos
	int N = atoi(argv[1]);
	int TH = atoi(argv[2]);
	
    // Reserva de Memoria
	double *matrixA  = (double *)calloc(N*N, sizeof(double));
	double *matrixB  = (double *)calloc(N*N, sizeof(double));
	double *matrixC  = (double *)calloc(N*N, sizeof(double));
	srand(time(NULL));

    // Configuración OpenMP
	omp_set_num_threads(TH);

    // Inicialización de las matrices
	iniMatrix(matrixA, matrixB, N);

    // Imprime las matrices iniciales (con formatos 0 y 1)
	impMatrix(matrixA, N, 0);
	impMatrix(matrixB, N, 1);

    // 6Ejecución y Medición
	InicioMuestra();
    // Llama a la función de multiplicación transpuesta
	multiMatrixTrans(matrixA, matrixB, matrixC, N);
	FinMuestra();

    // Se muestran los resultados
	impMatrix(matrixC, N, 0);

	// Liberación de Memoria
	free(matrixA);
	free(matrixB);
	free(matrixC);
	
	return 0;
}
