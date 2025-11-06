/*
 * Pontificia Universidad Javeriana
 *
 * Autor: David Santiago Calderon Idarraga
 * Fecha: 4 de noviembre de 
 * Tema: Open MP
 * Descripcion:
 *  - Introduccion en laboratorio de OPENMP
 *  - Cláusulas y directivas para el programa
 */

#include <omp.h>     // Librería que permite usar directivas y funciones de OpenMP
#include <stdio.h>   // Librería estándar para imprimir y leer datos (printf, etc.)
#include <stdlib.h>  // Librería estándar para manejo de memoria y utilidades (atoi, exit)

int main(int argc, char *argv[]){
    // Obtiene el número máximo de hilos que el sistema puede usar
    // Esto depende del número de núcleos físicos y virtuales (HyperThreading)
    int maxHIlos = omp_get_max_threads();

    // Imprime la cantidad máxima de hilos detectada en el sistema
    printf("OpenMP ejecutando en CorES(HiperThreading= %d hilos\n", maxHIlos);
    
    // Por defecto, OpenMP creará tantos hilos como cores/hilos de hardware haya disponibles.
    #pragma omp parallel
    { 
        // omp_get_thread_num() devuelve el número identificador del hilo (0, 1, 2, ...)
        printf("Hola mundo desde el thread %d\n", omp_get_thread_num());
    }
    return 0;
}

