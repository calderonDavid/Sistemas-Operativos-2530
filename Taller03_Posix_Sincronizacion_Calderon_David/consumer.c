/*****************************************************
* Pontificia Universidad Javeriana
* Autores: David Calderon Idarraga - Alejandro Lopez Beltran
* Fecha: 13/11/2025
* Materia: Sistemas Operativos
* Tema: Consumidor con memoria compartida y semáforos
* Descripción:
* - Implementación del proceso consumidor usando:
*     * Memoria compartida POSIX (shm_open, mmap)
*     * Semáforos POSIX (sem_open, sem_wait, sem_post)
* - Consume mensajes de un buffer circular producido por producer.c
* - Controla concurrencia mediante vacio, lleno y mutex
*****************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define N 10
#define SHM_NAME "/shm_buffer"

// Estructura idéntica a producer.c para compatibilidad
typedef struct {
    char buffer[N][256];
    int entrada;
    int salida;
} compartir_datos;

int main() {
    // Abrir los semáforos existentes
    sem_t *vacio = sem_open("/sem_vacio", 0);
    sem_t *lleno = sem_open("/sem_lleno", 0);
    sem_t *mutex = sem_open("/sem_mutex", 0);

    if (vacio == SEM_FAILED || lleno == SEM_FAILED || mutex == SEM_FAILED) {
        perror("Error al abrir los semaforos");
        exit(1);
    }

    // Abrir la memoria compartida
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error al abrir memoria compartida");
        exit(1);
    }

    // Mapear memoria
    compartir_datos *datos = mmap(NULL, sizeof(compartir_datos),
                                PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (datos == MAP_FAILED) {
        perror("Error al mapear memoria");
        exit(1);
    }

    while (1) {
        // Espera a que haya elementos
        sem_wait(lleno);

        // Entra a sección crítica
        sem_wait(mutex);

        // Lee mensaje del buffer circular
        printf("Consumido: %s\n", datos->buffer[datos->salida]);
        datos->salida = (datos->salida + 1) % N;

        // Sale de sección crítica
        sem_post(mutex);

        // Indica que hay un espacio vacío nuevo
        sem_post(vacio);

        sleep(1); // Simula tiempo de procesamiento
    }

    return 0;
}