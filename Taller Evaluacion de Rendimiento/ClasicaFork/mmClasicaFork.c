/****************************************************************************************
* Fecha: 03-Nov-2025
* Autor: David Calderon y Alejandro Lopez
* Materia: Sistemas Operativos
* Tema: Taller de Evaluación de Rendimiento
* Programa: Multiplicación de Matrices algoritmo clásico
* Versión: Paralelismo con Procesos Fork 
***************************************************************************************/

#include "mmClasicaFork.h"

int main(int argc, char *argv[]) {
	// Verifica que se pasen los argumentos correctos
	if (argc < 3) {
		printf("\n \t\tUse: $./nom_ejecutable Size Hilos \n");
		exit(0);
	}
	int N      = (int) atoi(argv[1]); // Tamaño de la matriz
	int num_P  = (int) atoi(argv[2]); // Número de procesos
	
	// Reserva memoria para las tres matrices
	double *matA = (double *) calloc(N*N, sizeof(double));
	double *matB = (double *) calloc(N*N, sizeof(double));
	double *matC = (double *) calloc(N*N, sizeof(double));

    srand(time(0)); // Semilla para aleatorios
    
    iniMatrix(matA, matB, N); // Inicializa matrices
    impMatrix(matA, N);       // Imprime si es pequeña
    impMatrix(matB, N);
    
    int rows_per_process = N/num_P; // Filas que calculará cada proceso

	InicioMuestra(); // Inicia el cronómetro

    // Crea los procesos hijos
    for (int i = 0; i < num_P; i++) {
        pid_t pid = fork();
        
        if (pid == 0) { // Código del hijo
            int start_row = i * rows_per_process;
            // El último proceso toma las filas restantes si la división no es exacta
            int end_row = (i == num_P - 1) ? N : start_row + rows_per_process;
            
            // El hijo calcula su parte
			multiMatrix(matA, matB, matC, N, start_row, end_row); 
            
            // Si la matriz es pequeña, el hijo imprime su resultado parcial
			if(N<9){
           		printf("\nChild PID %d calculated rows %d to %d:\n", getpid(), start_row, end_row-1);
            	for (int r = start_row; r < end_row; r++) {
                	for (int c = 0; c < N; c++) {
                    	printf(" %.2f ", matC[N*r+c]);
                	}
                	printf("\n");
            	}
			}
            exit(0); // El hijo termina
        } else if (pid < 0) {
            perror("fork failed");
            exit(1);
        }
    }
    
    // El padre espera a que todos los hijos terminen
    for (int i = 0; i < num_P; i++) {
        wait(NULL);
    }
  	
	FinMuestra(); // Detiene el cronómetro e imprime el tiempo total
 
    // Libera la memoria reservada
	free(matA);
	free(matB);
	free(matC);

    return 0;
}
