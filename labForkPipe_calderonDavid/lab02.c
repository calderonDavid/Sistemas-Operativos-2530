/*
 * Fecha: 7/10/20225
 * Autor: David CALDERON
 * Materia Sistemas Operativos
 * Tema: Funcion Fork,creacion de procesos
 * Duplicidad y caos de creación
 * Descripcion: Al principio, se presenta un caos al ejecutar el programa, despues de varias ejecueciones, el caos tiene a desaparecer y aparece un orden al momento de imprimir el proceso padres e hijo 
 */
//Se crean las librerias que se van a usar para el llamadado de fork()
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int processID = fork(); //Se asgina una variable de tipo entero para guardar el id del proceso hijo creado con fork()
    //Se compara el id del proceso creado y compara  con el 0 si es mayor, para determinar si es el padre o el hijo
    if (processID > 0) {
        printf("\n 'proceso padre'.... \n");
    }//Si no es mayor, lo compara si es igual a 0, para saber si es el proceso padre
    else if (processID == 0) {
        printf("\n 'proceso hijo' recien creado \n");
    }
    else {//Si no es es ningun de los dos entonces hubo un problea al momento de crear el proceso, y problemas con la funcion fork()
        printf("\n llamada al sistema fork() falló\n");
    }
    //Se imprimen los numeros del 0 al 4
    printf("\n\n Imprimiendo ... \n");
    for (int i = 0; i < 5; i++)
        printf(" %d ", i);
    printf("\n Fin \n");
    return 0;
}
