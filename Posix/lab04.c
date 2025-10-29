/*
 * Fecha: 28/10/2025
 * Autor: David Calderón
 * Materia: Sistemas Operativos
 * Tema: Uso de mutex para sincronizar el acceso a una variable compartida entre hilos
 * Descripción:
 * Se implementa un programa que crea tres hilos concurrentes, donde cada uno representa un trabajo.
 * Se utiliza un mutex para proteger la sección crítica al momento de modificar una variable global compartida.
 */
//Se incluyen las librerías necesarias para el manejo de hilos, mutex y entrada/salida estándar
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h> 

//Se declara un arreglo para los identificadores de los hilos
pthread_t tid[3];

int counter;

//Se define un mutex que controlará el acceso a la sección crítica
pthread_mutex_t lock;


void* compute(void *arg) {
    unsigned long i = 0;
    int my_job_number; //Se almacena el número de trabajo de cada hilo

    //Se inicia la sección crítica donde solo un hilo puede acceder al contador a la vez
    pthread_mutex_lock(&lock);
    counter += 1; //Se incrementa el contador global 
    my_job_number = counter;

    //Se libera el mutex para permitir que otro hilo entre y se muestra el hilo que esta trabajando
    pthread_mutex_unlock(&lock);
    printf("\n Job %d has started\n", my_job_number);

    //Se simula el trabajo del hilo
    sleep(1);
    //Se muestra cuando el hilo termina su trabajo
    printf("\n Job %d has finished\n", my_job_number);

    return NULL;
}

int main(void) {
    int i = 0;
    int error;

    //Se inicializa el mutex antes de su uso; si falla, el programa se detiene
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n Mutex init failed\n");
        return 1;
    }

    //Se crean tres hilos
    while (i < 3) {
        error = pthread_create(&(tid[i]), NULL, &compute, NULL);
        if (error != 0)
            printf("\nThread can't be created : [%s]", strerror(error));
        i++;
    }

    //Se espera a que los tres hilos terminen su ejecución antes de continuar
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);

    //Se destruye el mutex al finalizar el programa para liberar recursos del sistema
    pthread_mutex_destroy(&lock);
    return 0;
}

