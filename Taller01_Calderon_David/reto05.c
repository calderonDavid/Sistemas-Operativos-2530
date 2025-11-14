/***********************0*******************************************************
 * Fecha: 26 de agosto 2025
 * Autor: David Santiago Calderon Idarraga
 * Materia: Sistemas Operativos
 * Challenge 5
*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
 int *ptr, i , n1, n2;                                  // Se crean variables de tipo entero y un puntero de tipo entero
 printf("Enter size: ");                                // Pide el tamaño y lo guarda en n1
 scanf("%d", &n1);                                      
 
 ptr = (int*) malloc(n1 * sizeof(int));                 // Reserva memoria para n1 enteros
 printf("Addresses of previously allocated memory: ");
 
 for(i = 0; i < n1; ++i)                                // Recorre cada posición e imprime la direccion de cada reserva
    printf("\n\np = %p\n", ptr+i);
 printf("\n Enter the new size: ");
 scanf("%d", &n2);                                      // Pide el nuevo tamaño de la reserva
 // rellocating the memory
 ptr = realloc(ptr, n2 * sizeof(int));                  // Reasigna la reserva memoria para n2 enteros
 printf("Addresses of newly allocated memory: ");
 for(i = 0; i < n2; ++i)                                // Recorre cada posición e imprime la direccion de cada reserva
    printf("\n\np = %p\n", ptr+i);
 free(ptr);                                              // Libera la memoria del puntero ptr
     
}