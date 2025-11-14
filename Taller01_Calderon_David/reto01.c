/***********************0***************************************************************
 * Fecha: 26 de agosto 2025
 * Autor: David Santiago Calderon Idarraga
 * Materia: Sistemas Operativos
 * Challenge 1
*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int main() {
 char *p;                           // Se crea un puntero de tipo char llamado p
 char *q = NULL;                    //Se crea un puntero a char llamado q
 printf("Address of p = %s\n", p);  // Imprime la dirección de p
 
 strcpy(p, "Hello, I'm the best in Operating Systems!!!");  // Copia la cadena en la dirección a la que apunte p
 printf("%s\n", p);                                         // Imprime lo que tenga p
 printf("About to copy \"Goodbye\" to q\n");                // Salida por pantalla
 printf("String copied\n");                                 // Mensaje fijo
 printf("%s\n", q);                                         // Imprime el punto q
 free(p);                                                   //Libera Memoria del puntero p
 free(q);                                                   //Libera Memoria del puntero q
 return 0;
}