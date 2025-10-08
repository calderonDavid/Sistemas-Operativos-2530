/*
 * Autor: David CALDERON
 * Fecha: 07 de octubre de 2025
 * MAteria: Sistemas Operativos
 * COMnicacion entre proceos usando PIPE
 * Descripción:implementa comunicación entre un proceso padre e hijo mediante un pipe para enviar un mensaje del padre al hijo
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int pipefd[2]; // Se declara un arreglo para los descriptores del pipe
    pid_t pHijo_PID;

    // Se crea el pipe y se verifica si ocurrió un error 
    if (pipe(pipefd) == -1) {
        perror("PIPE");
        exit(EXIT_FAILURE);
    }

    // Se crea un proceso hijo usando fork()
    pHijo_PID = fork(); // Creación proceso HIJO
    if (pHijo_PID == -1) {
        perror("FORK");
        exit(EXIT_FAILURE);
    }


    if (pHijo_PID == 0) {
        // Código del proceso hijo
        // Se ejecuta el código del proceso hijo

        // Se cierra el descriptor de escritura, ya que el hijo solo lee
        close(pipefd[1]); // Cierra el descriptor de escritura
        char mensaje[100];
        int lecturaBYTES;

        // Se lee el mensaje enviado por el padre a través del pipe
        lecturaBYTES = read(pipefd[0], mensaje, sizeof(mensaje));
        if (lecturaBYTES == -1) {
            perror("LECTURA!");
            exit(EXIT_FAILURE);
        }

        // Se imprime el mensaje recibido desde el padre
        printf("Proceso HIJO: Recibe mensaje del PADRE: %.*s \n", lecturaBYTES, mensaje);
	// Se cierra el descriptor de lectura
        close(pipefd[0]);

    }  else {
	// Se ejecuta el código del proceso padre

        // Se cierra el descriptor de lectura, ya que el padre solo escribe
    	close(pipefd[0]);
    	char mensajePADRE[] = "Hola HIJO desde el PADRE!!";
    	int escrituraBYTES;
	// Se escribe el mensaje en el pipe para que lo lea el hijo
    	escrituraBYTES = write(pipefd[1], mensajePADRE, strlen(mensajePADRE));
    	if (escrituraBYTES == -1) {
        	perror("ESCRITURA");
        	exit(EXIT_FAILURE);
    	}
	// Se cierra el descriptor de escritura
    	close(pipefd[1]);
    }

    return 0;
}
