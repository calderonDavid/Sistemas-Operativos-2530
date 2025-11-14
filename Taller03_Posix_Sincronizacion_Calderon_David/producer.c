/*****************************************************
* Pontificia Universidad Javeriana
* Autores: David Calderon Idarraga - Alejandro Lopez Beltran
* Fecha: 13/11/2025
* Materia: Sistemas Operativos
* Tema: Productor con memoria compartida y semáforos
* Descripción:
* - Implementación del proceso productor usando:
*       * Memoria compartida POSIX (shm_open, mmap)
*       * Semáforos POSIX (sem_open, sem_wait, sem_post)
* - El usuario ingresa mensajes que se almacenan en un 
*   buffer circular compartido con el consumidor.
* - Controla concurrencia mediante vacio, lleno y mutex.
*****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define N 10   // Tamaño del buffer circular
#define SHM_NAME "/shm_buffer" // Nombre de la memoria compartida

// Estructura compartida entre productor y consumidor
typedef struct {
    char buffer[N][256];  // Buffer de 10 posiciones, cada una con 256 chars
    int entrada;          // Índice donde el productor escribe
    int salida;           // Índice donde el consumidor lee
} compartir_datos;

int main() {
    // Abre los semáforos existentes
    sem_t *vacio = sem_open("/sem_vacio", 0);   // Cuenta espacios vacíos
    sem_t *lleno = sem_open("/sem_lleno", 0);   // Cuenta espacios llenos
    sem_t *mutex = sem_open("/sem_mutex", 0);   // Exclusión mutua

    if (vacio == SEM_FAILED || lleno == SEM_FAILED || mutex == SEM_FAILED) {
        perror("Error al abrir los semáforos");
        exit(1);
    }

    // Abrir la memoria compartida ya creada
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error al abrir la memoria compartida");
        exit(1);
    }

    // Mapear memoria compartida
    compartir_datos *datos = mmap(NULL, sizeof(compartir_datos),
                                  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (datos == MAP_FAILED) {
        perror("Error al mapear memoria compartida");
        exit(1);
    }

    char mensaje[256];

    while (1) {
        printf("Ingrese un mensaje: ");
        fgets(mensaje, sizeof(mensaje), stdin);
        mensaje[strcspn(mensaje, "\n")] = '\0'; // Quita salto de línea

        // Espera un espacio vacío
        sem_wait(vacio);

        // Entra a sección crítica
        sem_wait(mutex);

        // Escribe mensaje en buffer circular
        strcpy(datos->buffer[datos->entrada], mensaje);
        datos->entrada = (datos->entrada + 1) % N;

        // Sale de sección crítica
        sem_post(mutex);

        // Informa que hay un nuevo espacio lleno
        sem_post(lleno);
    }

    return 0;
}