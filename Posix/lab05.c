/****************************************************
 * Pontificia Universidad Javeriana
 * Autor: David Santiago Calderon Idarraga
 * Materia: Sistemas Operativos
 * Fecha: 28/10/2025
 * Tema: Hilos con API POSIX
 *        Variables de Condición y Exclusión Mutua
 *
 * Descripción:
 * Suponer x*x es altamente costosa en términos de
 * cálculo computacional. Se desea calcular la suma
 * de los cuadrados de una cantidad de números.
 * Se desea aplicar la "paralelización" del cálculo
 * de la sumatoria de los cuadrados usando hilos.
 *
 * HAY CONDICION DE CARRERA
 ****************************************************/
//Se inclyen las librerias que se iran a utilizar
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

int acumulado = 0;
// Función que ejecutará cada hilo: calcula el cuadrado del número recibido
void *cuadrado(void *x) {
    int xi = (intptr_t)x;// Se obtiene el número enviado como argumento
    acumulado += (xi) * (xi);// Se suma su cuadrado al acumulado
    return NULL;   
}

int main(int argc, char *argv[]) {
    pthread_t hilos[20];
    // Se crean 20 hilos, cada uno con un número distinto (del 1 al 20)
    for (int i = 0; i < 20; i++) {
        int *valor = malloc(sizeof(int));// Reserva dinámica para el número
        *valor = i + 1;	 // Asigna el número a calcular
        pthread_create(&hilos[i], NULL, cuadrado, (void *)valor);// Crea el hilo
    }
    // Se espera que todos los hilos terminen antes de continuar
    for (int i = 0; i < 20; i++) {
        void *resultado;
        pthread_join(hilos[i], &resultado);
    }
    // Se imprime el resultado total
    printf("Acumulado = %d\n", acumulado);

}
