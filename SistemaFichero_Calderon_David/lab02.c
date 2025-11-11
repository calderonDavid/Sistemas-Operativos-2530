/*************************************************************************
 * Pontificia Universidad Javeriana
 * Autor: [Tu Nombre]
 * Fecha: 11/10/2025
 * Materia: Sistemas Operativos
 * Tema: Sistemas de Ficheros
 * Descripción: Programa en C que solicita un directorio, muestra los archivos
 * que contiene, su modo de acceso, si tiene permisos de lectura para el propietario,
 * si es un directorio, y si ha sido modificado en los últimos 10 días.
 *************************************************************************/

#include <stdio.h>      // Para funciones de entrada/salida (printf, fgets, etc.)
#include <string.h>     // Para manejo de cadenas (strcpy, strcat, strlen, etc.)
#include <time.h>       // Manejo de fechas y tiempos (time, difftime, etc.)
#include <stdlib.h>     // Para funciones estándar como exit() o malloc()
#include <fcntl.h>      // Para manipular archivos (aunque en este programa no se usa directamente)
#include <unistd.h>     // Funciones del sistema UNIX
#include <sys/stat.h>   // Para la estructura 'stat' y macros como S_ISREG()
#include <dirent.h>     // Para manejar directorios (opendir, readdir, closedir)
#include <sys/types.h>  // Definición de tipos para el sistema de archivos


int main() {
    int er;                           
    char nomdir[100], nomfich[100], resp[30];  
    struct stat atr;                   // Estructura para almacenar atributos del archivo
    DIR *d;                            // Puntero al directorio
    struct dirent *rd1;                // Puntero a la entrada del directorio
    time_t fecha;                      // Variable para almacenar la fecha actual

    // Se solicita el nombre del directorio
    printf("Nombre directorio\n");
    if (fgets(nomdir, sizeof(nomdir), stdin) == NULL) {
        fprintf(stderr, "Error leyendo el nombre del directorio\n");
        return -1;
    }

    nomdir[strcspn(nomdir, "\n")] = '\0';

    // Se obtiene la fecha/hora actual 
    fecha = time(NULL);

    if ((d = opendir(nomdir)) == NULL) {
        perror("No existe ese directorio");
        return -1;
    }
    else {
        /* recorrer todas las entradas del directorio */
        while ((rd1 = readdir(d)) != NULL) {

            if ((strcmp(rd1->d_name, ".") != 0) && (strcmp(rd1->d_name, "..") != 0)) {

                strcpy(nomfich, nomdir);
                strcat(nomfich, "/");
                strcat(nomfich, rd1->d_name);

                //Se  imprimir la ruta completa
                printf("fichero: %s\n", nomfich);

                // Se obtiene los  atributos del fichero
                er = stat(nomfich, &atr);
                if (er == -1) {
                    perror("stat");
                    continue; 
                }

                printf("modo: %#o\n", atr.st_mode);

                // Se verificar permiso de lectura 
                if ((atr.st_mode & 0400) != 0)
                    printf("permiso R para propietario\n");
                else
                    printf("No permiso R para propietario\n");

                // Se verifica si es un directorio
                if (S_ISDIR(atr.st_mode))
                    printf("Es un directorio\n");

                // Se verifica si es un archivo regular
                if (S_ISREG(atr.st_mode)) {
                    //Muestra los ficheros modificados en los ultimos 10 dias
                    if ((fecha - 10*24*60*60) < atr.st_mtime) {
                        //Se imprime el nombre y fecha de modificación
                        printf("FICHERO: %s: fecha modif: %s", rd1->d_name, ctime(&atr.st_mtime));
                    }
                }
            }
        }
        closedir(d); /* cerrar directorio */
    }

}

