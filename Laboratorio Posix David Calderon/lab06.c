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
 ****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

// Mutex para proteger la variable compartida 'acumulado'
pthread_mutex_t acumuladoMutex = PTHREAD_MUTEX_INITIALIZER;

// Variable global donde se acumulan los cuadrados
int acumulado = 0;

// Función que ejecuta cada hilo: calcula el cuadrado del número recibido
void *cuadrado(void *x) {
    int xi = (intptr_t)x; // Se obtiene el número del hilo (se envía como puntero convertido)
    
    // Se bloquea el mutex antes de modificar la variable global
    pthread_mutex_lock(&acumuladoMutex);
    acumulado += xi * xi; 
    //Se desbloquear el mutex
    pthread_mutex_unlock(&acumuladoMutex);
    
    return NULL; // No retorna nada útil
}

int main(int argc, char *argv[]) {
    pthread_t hilos[20]; // Arreglo para 20 hilos

    // Creación de hilos: cada hilo calcula el cuadrado de un número del 1 al 20
    for (int i = 0; i < 20; i++) {
        int *valor = malloc(sizeof(int));
        *valor = i + 1; // Número que el hilo procesará
        pthread_create(&hilos[i], NULL, cuadrado, (void *)valor);
    }

    // Se espera a que todos los hilos terminen
    for (int i = 0; i < 20; i++) {
        pthread_join(hilos[i], NULL);
    }

    // Se imprime el resultado acumulado
    printf("Acumulado = %d\n", acumulado);
}

