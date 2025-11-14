/***********************0*******************************************************
 * Fecha: 26 de agosto 2025
 * Autor: David Santiago Calderon Idarraga
 * Materia: Sistemas Operativos
 * Challenge 4
*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
 int n, i, *ptr, sum = 0;                   // Se crean variables de tipo entero y un puntero de tipo entero
 printf("Enter number of elements: ");
 scanf("%d", &n);                           // Pide el tamaño y lo guarda en n
 
 ptr = (int*) calloc(n, sizeof(int));       // Reserva memoria para n enteros
 if(ptr == NULL) {
    printf("Error! memory not allocated.");
    exit(0);
 }
 
 
 for(i = 0; i < n; ++i) {                   //Pide los elementos que iran en la memoria reservada por el puntero
    printf("Enter elements: ");
    scanf("%d", ptr + i);
    sum += *(ptr + i);                      //Suma cada digito que se va agregando a la memoria
 }
 
 printf("Sum = %d", sum);                   //Se imprime el total de la suma
 free(ptr);                                 
 return 0;                                  //Libera la memoria de puntero ptr
}