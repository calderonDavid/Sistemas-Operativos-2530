/*
 * Pontificia Universidad Javeriana
 *
 * Autor: David Santiago Calderon Idarraga
 * Fecha:4  de noviembre de 
 * Tema: Open MP
 * Descripcion:
 *  - Introduccion en laboratorio de OPENMP
 *  - Cláusulas y directivas para el programa
 */

#include <omp.h>      // Librería de OpenMP para programación paralela
#include <stdio.h>    // Librería estándar para entrada y salida
#include <time.h>     // Librería de manejo de tiempo
#include <stdlib.h>   // Librería estándar para funciones como atoi() y exit()

int main(int argc, char *argv[]){
    
    // Verifica que se haya pasado el número correcto de argumentos
    if(argc!=2){
    	printf("Error\n\t$.ejecutable numHIlos"); // Muestra mensaje si no se pasa un argumento
    	exit(0); // Termina la ejecución
    }

    // Convierte el argumento de texto (argv[1]) a número entero
    int numHilos = (int) atoi(argv[1]);

    // Imprime el número de hilos que el usuario solicitó
    printf("==== %d === \n", numHilos); 

    // Obtiene el número máximo de hilos disponibles en el sistema
    int maxHIlos = omp_get_max_threads();

    // Muestra el número máximo de hilos que el sistema puede ejecutar (incluye hyperthreading)
    printf("OpenMP ejecutando en CorES(HiperThreading= %d hilos\n",maxHIlos);
    
    /*Fija el número de hilos que se desee*/
    omp_set_num_threads(numHilos);

    // Región paralela de OpenMP
    #pragma omp parallel
    { 
        // Cada hilo ejecutará este bloque e imprimirá su ID único
        printf("Hola mundo desde el thread %d\n", omp_get_thread_num());
    }

    return 0;
}

