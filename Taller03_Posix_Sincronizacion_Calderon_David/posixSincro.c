/*****************************************************
* Pontificia Universidad Javeriana
* Autores: David Calderon Idarraga - Alejandro Lopez Beltran
* Fecha: 13/11/2025
* Materia: Sistemas Operativos
* Tema: Productor–Consumidor usando hilos, mutex y variables de condición (pthread)
* Descripción:
* - Implementación del problema clásico Productor–Consumidor mediante:
*       * Hilos POSIX (pthread_create, pthread_join)
*       * Exclusión mutua con pthread_mutex_t
*       * Sincronización con pthread_cond_t (wait / signal)
* - Usa un buffer circular de tamaño N compartido entre productor y consumidor.
* - Controla condiciones de buffer lleno y buffer vacío evitando condiciones de carrera.
*****************************************************/


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5  // Tamaño del buffer

int buffer[N];     // Buffer circular
int count = 0;     // Elementos en el buffer
int in = 0;        // Índice del productor
int out = 0;       // Índice del consumidor

pthread_mutex_t buf_mutex = PTHREAD_MUTEX_INITIALIZER;   // Control de acceso
pthread_cond_t buf_cond = PTHREAD_COND_INITIALIZER;      // Señalización

// Función del productor
void *productor(void *arg) {
    int item;

    while (1) {
        item = rand() % 100; // Genera un número aleatorio

        pthread_mutex_lock(&buf_mutex);

        // Espera si el buffer está lleno
        while (count == N) {
            pthread_cond_wait(&buf_cond, &buf_mutex);
        }

        // Inserta el ítem en el buffer
        buffer[in] = item;
        in = (in + 1) % N;
        count++;

        printf("Productor produjo: %d\n", item);

        // Notifica al consumidor
        pthread_cond_signal(&buf_cond);
        pthread_mutex_unlock(&buf_mutex);

        sleep(1);
    }
}

// Función del consumidor
void *consumidor(void *arg) {
    int item;

    while (1) {
        pthread_mutex_lock(&buf_mutex);

        // Espera si buffer vacío
        while (count == 0) {
            pthread_cond_wait(&buf_cond, &buf_mutex);
        }

        // Extrae ítem
        item = buffer[out];
        out = (out + 1) % N;
        count--;

        printf("Consumidor consumió: %d\n", item);

        // Notifica al productor
        pthread_cond_signal(&buf_cond);
        pthread_mutex_unlock(&buf_mutex);

        sleep(1);
    }
}

int main() {
    pthread_t prod_thread, cons_thread;

    // Crear hilos
    pthread_create(&prod_thread, NULL, productor, NULL);
    pthread_create(&cons_thread, NULL, consumidor, NULL);

    // Esperar hilos
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    return 0;
}
