/*
 * Fecha: 28/10/2025
 * Autor: David Calderón
 * Materia: Sistemas Operativos
 * Tema: Sincronización de hilos con mutex y variables de condición
 * Descripción:
 * Se implementa un programa que utiliza dos hilos para incrementar un contador global de forma concurrente.
 * Se emplean mutex y variables de condición para controlar cuándo un hilo debe detenerse y cuándo puede continuar,
 * evitando conflictos de acceso y demostrando el uso de sincronización en entornos multihilo.
 */
//Se incluyen las librerías necesarias para manejar hilos, mutex y operaciones de entrada/salida
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Se declaran los mutex para proteger las secciones críticas y la condición para sincronizar los hilos
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_cond = PTHREAD_COND_INITIALIZER;


int count = 0;

// Se definen las constantes que controlan el comportamiento del conteo
#define COUNT_DONE 15
#define COUNT_HALT1 4
#define COUNT_HALT2 11

// Se define la función ejecutada por el primer hilo
void *count01() {
    for (;;) {
        // Se bloquea el mutex de condición para evaluar el estado del contador
        pthread_mutex_lock(&condition_mutex);

        while (count >= COUNT_HALT1 && count <= COUNT_HALT2) {
            pthread_cond_wait(&condition_cond, &condition_mutex);
        }

        // Se libera el mutex de condición una vez que la condición cambia
        pthread_mutex_unlock(&condition_mutex);

        // Se bloquea el mutex del contador para incrementar de forma segura
        pthread_mutex_lock(&count_mutex);
        count++;
        // Se imprime el valor actual del contador junto al identificador del hilo
        printf("Counter value functionCount ==> 01 <==: %d\n", count);
        pthread_mutex_unlock(&count_mutex);

        if (count >= COUNT_DONE) return NULL;
    }
}

// Se define la función ejecutada por el segundo hilo
void *count02() {
    for (;;) {
        // Se bloquea el mutex de condición para verificar si debe enviarse una señal
        pthread_mutex_lock(&condition_mutex);
        if (count < COUNT_HALT1 || count > COUNT_HALT2) {
            pthread_cond_signal(&condition_cond);
        }

        // Se libera el mutex de condición
        pthread_mutex_unlock(&condition_mutex);

        // Se bloquea el mutex del contador para incrementar de forma segura
        pthread_mutex_lock(&count_mutex);
        count++;
        // Se imprime el valor actual del contador junto al identificador del hilo
        printf("Counter value functionCount ==> 02 <==: %d\n", count);
        pthread_mutex_unlock(&count_mutex);

        if (count >= COUNT_DONE) return NULL;
    }
}


int main() {
    pthread_t thread1, thread2;

    // Se crean los dos hilos, asignando cada uno a su respectiva función
    pthread_create(&thread1, NULL, &count01, NULL);
    pthread_create(&thread2, NULL, &count02, NULL);

    // Se espera a que ambos hilos terminen su ejecución antes de cerrar el programa
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    exit(0);
}
