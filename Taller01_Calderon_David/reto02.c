/***********************0***************************************************************
 * Fecha: 26 de agosto 2025
 * Autor: David Santiago Calderon Idarraga
 * Materia: Sistemas Operativos
 * Challenge 2
*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
 char *q = NULL;                                                // Puntero tipo char iniciado en NULL
 printf("Requesting space for \"Goodbye\" \n");                 // Aviso
 q = (char *)malloc(strlen("Goodbye")+1);                       // Reserva memoria para la palabra
 if (!q) {                                                      // Si malloc falla, entonces mostrar error y salir
    perror("Failed to allocate space because");
    exit(1);
 }
 printf("About to copy \"Goodbye\" to q at address %s \n", q);  // Imprime un mensaje con la dirección de q
 strcpy(q, "Goodbye");                                          // Copia la palabra en la memoria
 printf("String copied\n");
 printf("%s \n", q);                                            // Imprime el contenido de q
 free(p);                                                     //Libera Memoria del puntero p
 free(q);                                                     //Libera Memoria del puntero q
 return 0;
}