/****************************************************************************************
** Fecha: 03-Nov-2025
** Autor: David Calderon y Alejandro Lopez
** Materia: Sistemas Operativos
** Tema: Taller de Evaluación de Rendimiento
** Fichero: Interfaz (header) para mmClasicaFork
***************************************************************************************/

#ifndef MMCLASICAFORK_H
#define MMCLASICAFORK_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

// Variables para medir el tiempo de ejecución
extern struct timeval inicio, fin;

// Funciones para iniciar y finalizar el cronómetro
void InicioMuestra();
void FinMuestra();

// Función principal de multiplicación de matrices (porción)
void multiMatrix(double *mA, double *mB, double *mC, int D, int filaI, int filaF);

// Funciones auxiliares para imprimir e inicializar matrices
void impMatrix(double *matrix, int D);
void iniMatrix(double *mA, double *mB, int D);

#endif
