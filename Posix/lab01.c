/*
 * Fecha: 21/10/2025
 * Autor: David Calderón
 * Materia: Sistemas Operativos
 * Tema: Hilos - Creación correcta
 * Descripción:
 * Se implementa un programa que crea dos hilos independientes utilizando la biblioteca pthread.
 * Se verifica si los hilos se crean correctamente mediante el valor de retorno de la función pthread_create.
 */
//Se incluyen las librerías necesarias para la creación y manejo de hilos, así como para la salida estándar
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//Se recibe un puntero genérico que se convierte a tipo char* para imprimir un mensaje, esto sera ejecutado por los hilos
void *print_message_function(void *ptr) {
    char *message;
    message = (char *) ptr;
    printf("%s \n", message);
}


void main() {
    //Se declaran las variables de tipo pthread_t para los identificadores de los hilos
    pthread_t thread1, thread2;

    //Se definen los mensajes que cada hilo mostrará
    char *message1 = "Thread 1";
    char *message2 = "Thread 2";

    //Se declaran las variables para almacenar los valores de retorno de la creación de hilos
    int iret1, iret2;

    //Se crean los dos hilos independientes, asignando la misma función con distintos mensajes
    iret1 = pthread_create(&thread1, NULL, print_message_function, (void*) message1);
    iret2 = pthread_create(&thread2, NULL, print_message_function, (void*) message2);

    //Se muestra por pantalla el valor retornado por la creación de cada hilo
    printf("Thread 1 returns: %d\n", iret1);
    printf("Thread 2 returns: %d\n", iret2);

    
    exit(0);
}
