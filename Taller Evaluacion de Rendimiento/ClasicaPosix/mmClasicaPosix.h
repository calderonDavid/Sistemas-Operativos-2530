/****************************************************************************************
* Fecha: 03-Nov-2025
* Autor: David Calderon y Alejandro Lopez
* Materia: Sistemas Operativos
* Tema: Taller de Evaluación de Rendimiento
* Fichero: Interfaz (header) para mmClasicaPosix
***************************************************************************************/

#ifndef MMCLASICAPOSIX_H
#define MMCLASICAPOSIX_H

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

// Variables globales compartidas entre hilos
extern pthread_mutex_t MM_mutex;
extern double *matrixA, *matrixB, *matrixC;
extern struct timeval inicio, fin;

// Estructura para pasar datos a cada hilo
struct parametros{
	int nH;   // Número total de hilos
	int idH;  // ID de este hilo
	int N;    // Tamaño de la matriz
};

// Funciones de tiempo y auxiliares
void InicioMuestra();
void FinMuestra();
// Funciones auxiliares para imprimir e inicializar matrices
void iniMatrix(double *m1, double *m2, int D);
void impMatrix(double *matriz, int D);

// Función principal de multiplicación de matrices
void *multiMatrix(void *variables);

#endif
