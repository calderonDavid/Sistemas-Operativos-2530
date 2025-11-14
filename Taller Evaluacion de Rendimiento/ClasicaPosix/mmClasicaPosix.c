/****************************************************************************************
* Fecha: 03-Nov-2025
* Autor: David Calderon y Alejandro Lopez
* Materia: Sistemas Operativos
* Tema: Taller de Evaluación de Rendimiento
* Programa: Multiplicación de Matrices algoritmo clásico
* Versión: Paralelismo con Hilos Pthreads "Posix" 
***************************************************************************************/

#include "mmClasicaPosix.h"

int main(int argc, char *argv[]){
	// Validación de Argumentos
	if (argc < 3){
		printf("Ingreso de argumentos \n $./ejecutable tamMatriz numHilos\n");
		exit(0);	
	}
	// Lectura de Argumentos
	int N = atoi(argv[1]); 
	int n_threads = atoi(argv[2]); 


    	pthread_t p[n_threads]; // Array para guardar los IDs de los hilos creados
    	pthread_attr_t atrMM;   // Variable para los atributos de los hilos

       // Reserva memoria para las matrices (globales)
	matrixA  = (double *)calloc(N*N, sizeof(double));
	matrixB  = (double *)calloc(N*N, sizeof(double));
	matrixC  = (double *)calloc(N*N, sizeof(double));

        // Inicialización de natrices e imprimirlas por pantalla
	iniMatrix(matrixA, matrixB, N);
	impMatrix(matrixA, N);
	impMatrix(matrixB, N);

	InicioMuestra(); // Inicia el cronómetro

	pthread_mutex_init(&MM_mutex, NULL); // Inicializa el mutex
	pthread_attr_init(&atrMM);           // Inicializa los atributos
	pthread_attr_setdetachstate(&atrMM, PTHREAD_CREATE_JOINABLE);

    	for (int j=0; j<n_threads; j++){
        	// Reserva memoria para los argumentos de *este* hilo
		struct parametros *datos = (struct parametros *) malloc(sizeof(struct parametros)); 
		datos->idH = j;
		datos->nH  = n_threads;
		datos->N   = N;

        	// Crea el hilo. Le pasa su ID, atributos, la función que debe ejecutar (multiMatrix) y sus argumentos (datos)
        	pthread_create(&p[j],&atrMM,multiMatrix,(void *)datos);
	}

    // El hilo principal se bloquea aquí hasta que todos los hilos creados terminen
    	for (int j=0; j<n_threads; j++)
        	pthread_join(p[j],NULL);
        	
    	FinMuestra(); // Para el cronómetro e imprime el tiempo
	
	impMatrix(matrixC, N); // Imprime el resultado final

	//Se libera memoria
	free(matrixA);
	free(matrixB);
	free(matrixC);

	pthread_attr_destroy(&atrMM); // Destruye atributos
	pthread_mutex_destroy(&MM_mutex); // Destruye el mutex
	pthread_exit (NULL); // Termina el hilo principal

	return 0;
}
