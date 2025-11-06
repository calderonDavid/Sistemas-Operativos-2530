/*
 * Pontificia Universidad Javeriana
 *
 * Autor: David Santiago Calderon Idarraga
 * Programa: ejemplo_variables_privadas.c
 * Fecha: 5 de noviembre de 2025
 * Materia: Sistemas Operativos
 * Tema: Open MP
 */

#include <omp.h>   // Se incluye la biblioteca de OpenMP para paralelismo.
#include <stdio.h> // Se incluye para funciones de entrada/salida (printf).
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
    // private(b) Cada hilo tiene una copia de b.
    // Esto soluciona la condición de carrera, pues ya no se comparte b.
    #pragma omp parallel for private(i)private(a) private(b)
    for (i=0; i<N; i++){
        b = a + i;//Se realiza el cálculo usando las variables 'a' y 'b' privadas. Se guardan en la 'b' privada.  
    }

     //Se imprimen las variables del hilo principal.
     //a vale 50 (la original nunca se tocó).
     //b vale 0 (la original nunca se tocó).
    printf("a = %d b = %d (Se espera a=50 b = 1049)\n", a, b);
    return 0;
}
