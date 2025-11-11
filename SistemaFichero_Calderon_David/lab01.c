/*************************************************************************
 * Pontificia Universidad Javeriana
 * Autor: David Santiago Calderon Idarraga
 * Fecha: 11/10/2025
 * Materia: Sistemas Operativos
 * Tema: Sistemas de FIcheros
 * Descripción: Programa en C que lea de teclado el nombre
 * de un directorio y muestre en pantalla el nombre y el tamaño de
 * los ficheros que contiene.
 *************************************************************************/
 
#include <sys/stat.h>   // Para la estructura 'stat' y macros como S_ISREG()
#include <fcntl.h>      // Para manipular archivos (aunque en este programa no se usa directamente)
#include <stdio.h>      // Para funciones de entrada/salida (printf, fgets, etc.)
#include <unistd.h>     // Para funciones del sistema Unix
#include <stdlib.h>     // Para funciones estándar como exit() o malloc()
#include <string.h>     // Para manejo de cadenas (strcpy, strcat, strlen, etc.)
#include <dirent.h>     // Para manejar directorios (opendir, readdir, closedir)

int main() {
    DIR *d;				// Puntero al directorio
    char nomdir[256], nomfich[256];	// Arrays para almacenar el nombre del directorio y del archivo
    struct stat datos;			// Estructura que contendrá información
    struct dirent *direc;		// Estructura que almacena la información de una entrada
    
    
    //Se solicita el nombre del directorio al usuario ===
    printf ("Introduzca el Nombre de un Directorio: ");
    fgets (nomdir,sizeof(nomdir),stdin);
     /*Eliminamos el \n del Nombre del Fichero*/
    nomdir[strcspn(nomdir, "\n")] = '\0';

    //Se intenta abrir el directorio
    if ((d=opendir(nomdir))==NULL) {		// La funcion opendir() devuelve NULL si el directorio no existe
        printf ("El directorio no existe\n");
        return -1;
    }
    
    
    while ((direc=readdir(d)) !=NULL) {          // readdir() devuelve NULL al llegar al final
    	// Copia el nombre del directorio, le agrega el caracter '/' y le añade el nombre del archivo
        strcpy(nomfich, nomdir);                  
        strcat(nomfich, "/");                     
        strcat(nomfich, direc->d_name);
          
        if (S_ISREG(datos.st_mode)) {
            printf ("Nombre: %s\t|\t Tamaño: %ld\n",direc->d_name,datos.st_size);
        }
    }/*Fin del While*/
    
    closedir(d);
}

