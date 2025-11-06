/*
 * Pontificia Universidad Javeriana
 *
 * Autor: David Santiago Calderon Idarraga
 * Fecha: 6 de noviembre de 2025
 * Tema: Open MP
 *
 * Descripcion:
 * - Calculo paralelo de la sumatoria de la funcion SENO.
 * - Mide el tiempo de ejecución usando gettimeofday.
 */

#include <omp.h>     // API de OpenMP para paralelismo
#include <stdio.h>   // Entrada/Salida estándar (printf)
#include <stdlib.h>  // Funciones de utilidad general (atoi, exit)
#include <math.h>    // Funciones matemáticas (sin)
#include <sys/time.h>// Funciones para medir tiempo (gettimeofday)

#define MIN(x,y) (((x) < (y))? (x): (y))// Directiva para obtener el mínimo entre dos valores

// Estructuras para medir el tiempo
struct timeval inicio, fin;

// Guarda el tiempo actual en la variable 'inicio'.
void InicioMuestra(){
    gettimeofday(&inicio, (void *)0);
}

// Detiene el cronómetro, calcula e imprime el tiempo transcurrido desde el inicio.
void FinMuestra(){
    gettimeofday(&fin, (void *)0);
    // Calcula diferencia en segundos y microsegundos
    fin.tv_usec -= inicio.tv_usec;
    fin.tv_sec  -= inicio.tv_sec;
    // Convierte todo a microsegundos
    double tiempo = (double) (fin.tv_sec) + (double)(fin.tv_usec) / 1000000.0;
    printf("Tiempo: %.9f s\n", tiempo);
}

// Función matemática costosa para simular carga de trabajo. Calcula una sumatoria de valores seno en un rango dependiente de i.
double funcion(int i){
    // Calcula un rango inicial y final en función de i
    int inicio_val = i*(i+4)/2;
    int final_val = inicio_val + i;
    double valor = 0;
    // Realiza una sumatoria de senos para simular trabajo pesado
    for(int j = inicio_val; j < final_val; j++){
        valor += sin(j);
    }
    return valor;
}

int main(int argc, char *argv[]){
    double sumatoria = 0;  // Acumulador de la suma total
    int i, N = 30000;      // Número de iteraciones 
    
    // Verifica que el usuario haya pasado el número de hilos como argumento
    if(argc != 2){
        printf("Uso: ./ejecutable numHilos\n");
        exit(1);
    }
    // Convierte el argumento a entero
    int numHilos = atoi(argv[1]);
    printf("==== %d ====\n", numHilos); 
    printf("Cores disponibles: %d\n", omp_get_max_threads());

    // Configura el número de hilos a usar en OpenMP
    omp_set_num_threads(numHilos);

    // Inicia la medición del tiempo
    InicioMuestra();

    // Cada hilo ejecuta una parte del bucle for.
    
    #pragma omp parallel for reduction(+ : sumatoria) //EVITA LA CONDICIÓN DE CARRERA asegura que cada hilo tengga su copia local de 'sumatoria'.
    for(i=0; i<N; i++){
        sumatoria += funcion(i);
    }
    // Muestra la sumatoria total calculada por todos los hilos
    printf("Sumatoria total: %f\n", sumatoria);
    
    // Finaliza la medición y muestra el tiempo total
    FinMuestra();
    return 0;
}
