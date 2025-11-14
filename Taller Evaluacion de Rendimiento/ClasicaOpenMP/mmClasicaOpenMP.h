/****************************************************************************************
* Fecha: 03-Nov-2025
* Autor: David Calderon y Alejandro Lopez
* Materia: Sistemas Operativos
* Tema: Taller de Evaluación de Rendimiento
* Fichero: Interfaz (header) para mmClasicaOpenMP
***************************************************************************************/

#ifndef MMCLASICAOPENMP_H
#define MMCLASICAOPENMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>

// Variables de tiempo
extern struct timeval inicio, fin;

// Funciones para iniciar y finalizar el cronómetro
void InicioMuestra();
void FinMuestra();
// Funciones auxiliares para imprimir e inicializar matrices
void impMatrix(double *matrix, int D);
void iniMatrix(double *m1, double *m2, int D);
// Función principal de multiplicación de matrices
void multiMatrix(double *mA, double *mB, double *mC, int D);

#endif
