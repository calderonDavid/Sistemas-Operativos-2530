/*
 * Fecha: 28/10/2025
 * Autor: David Calderón
 * Materia: Sistemas Operativos
 * Tema: Implementación de mutex con hilos sincronizados mediante join
 * Descripción:
 * Se implementa un programa que crea múltiples hilos para incrementar un contador global.
 * Se utiliza un mutex para garantizar que solo un hilo acceda a la variable compartida a la vez,
 * evitando condiciones de carrera. Además, se sincroniza la finalización de los hilos.
 */
//Se incluyen las librerías necesarias para el manejo de hilos, mutex y entrada/salida estándar
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//Se define el número total de hilos que se crearán
#define NTHREADS 10

//Se declara e inicializa el mutex para proteger la sección crítica
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int counter = 0;

//Se define la función que ejecutará cada hilo
void *thread_function(void *arg) {
    int i = *(int *)arg;

    //Se muestra el número del hilo y su identificador de sistema
    printf("Thread number: %d | Thread ID %ld\n", i, pthread_self());

    //Se bloquea el mutex
    pthread_mutex_lock(&mutex1);

    //Se incrementa el contador 
    counter++;

    //Se libera el mutex p
    pthread_mutex_unlock(&mutex1);

    return NULL;
}


void main() {
    pthread_t thread_id[NTHREADS];
    int i;

    //Se crean los hilos en un bucle
    for (i = 0; i < NTHREADS; i++) {
        pthread_create(&thread_id[i], NULL, thread_function, &i);
    }

    //Se espera a que todos los hilos terminen su ejecución 
    for (i = 0; i < NTHREADS; i++) {
        pthread_join(thread_id[i], NULL);
    }
    //Se imprime el valor final del contador
    printf("Final counter value: %d\n", counter);
    return;
}

