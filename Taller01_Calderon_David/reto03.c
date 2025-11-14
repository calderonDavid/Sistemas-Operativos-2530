/***********************0***************************************************************
 * Fecha: 26 de agosto 2025
 * Autor: David Santiago Calderon Idarraga
 * Materia: Sistemas Operativos
 * Challenge 3
*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
 int *ptr;                                          // Se crea un puntero a entero                                          
 ptr = malloc(15 * sizeof(*ptr));                   // Reserva memoria para 15 enteros

 if (ptr != NULL) {                                 // Si el puntero no esta aputnado a nulo, guardar el numero 480  en la posicion 6
 *(ptr + 5) = 480; 
 printf("Value of 6th integer is %d\n", *(ptr+5));  // Imprime el valor del sexto numero guardado en la posicion
 }
 free(ptr);                                         //Libera Memoria del puntero ptr
 return 0;
}
 
 