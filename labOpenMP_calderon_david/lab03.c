/*
 * Pontificia Universidad Javeriana
 *
 * Autor: David Santiago Calderon Idarraga
 * Programa: ejemplo_race_condition.c
 * Fecha: 6 de noviembre de 2025
 * Materia: Sistemas Operativos
 * Tema: Open MP
 */

#include <omp.h>   // Se incluye la biblioteca de OpenMP para paralelismo.
#include <stdio.h> // Se incluye para funciones de entrada/salida.
#include <stdlib.h> // Se incluye para funciones estándar.

int main(){
    int i;
    const int N = 1000;
    
    // Se inicializan las variables 'a' y 'b' en el hilo principal.
    int a = 50; 
    int b = 0;  

    // Se paraleliza el bucle for.
    // private(i) Cada hilo tiene su propio contador i.
    // private(a) Cada hilo tiene una copia de a.
    // b es compartida por defecto.
    #pragma omp parallel for private(i) private(a)
    for (i=0; i<N; i++){
        /*
         ¡CONDICIÓN DE CARRERA!
         Todos los hilos intentan escribir en la MISMA variable b compartida.
         */
        b = a + i;
        //Se sobrescriben resultados y el valor final es impredecible.
    }
     //Se imprimen las variables del hilo principal.
     //b tendrá un valor aleatorio e incorrecto debido a la condición de carrera.
     
    printf("a = %d b = %d (Se espera a=50 b = 1049)\n", a, b);
    return 0;
}
