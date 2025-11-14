/*
 * Fecha: 7/10/20225
 * Autor: David CALDERON
 * Materia Sistemas Operativos
 * Tema: Funcion Fork,creacuib de procesos
 */
//Se crean las librerias que se van a usar para el llamadado de fork()
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int processID = fork(); //Se asigna una variable para el llamado de la funcion fork()
    // Se compara el  ID del proceso actual con 0, si es mayor que 0, se esta ejecuanto el proceso padre.
    if (processID > 0) {
        printf("fork() devuelve un valor > 0. Este es el 'proceso padre' con ID: %d \n", getpid());
    }
    //Si el proceso es igual a 0,entonces el proceso hijo fue creado exiosamente.
    else if (processID == 0) {
        printf("fork() devuelve un valor 0. Este es un 'proceso hijo' recién creado con ID: %d \n", getpid());
        printf("El 'proceso padre' de este 'proceso hijo' tiene el ID: %d\n", getppid());
    }
    //SI es menor a 0, entonces hubo un error en la creacion del proceso hijo
    else {
        printf("fork() devuelve un valor < 0, por lo que la llamada al sistema fork() falló\n");
    }
    //Se muestra por pantalla un mensaje, para verificar el correcto funcinamiento de la funcion fork()
    printf("\nEsta es una única impresión. Si la llamada al sistema fork() ha tenido éxito \n");
    printf("tanto el 'proceso padre' como el 'proceso hijo' se ejecutarán simultáneamente, \n");
    printf("y esta impresión aparecerá dos veces.\n");

    return 0;
}
