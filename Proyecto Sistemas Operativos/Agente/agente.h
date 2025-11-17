/********************************************************
*  Pontificia Universidad Javeriana
*  Facultad de Ingeniería – Ingeniería de Sistemas
*
*  Proyecto: Sistema Simulador de Reservas de Parque
*  Archivo: agente.h
*  Descripción:
*     Archivo de cabecera del proceso cliente "Agente de
*     Reserva". Contiene las definiciones de constantes,
*     estructuras de datos, variables globales y los
*     prototipos de funciones empleadas en el módulo
*     principal y en las funciones auxiliares del agente.
*
*  Autores:
*     Santiago Bautista Velasquez
*     Alejandro Lopez
*     David Calderon
*
*  Fecha: 16/11/25
********************************************************/

#include <stdio.h>      // Se incluye la biblioteca estándar de entrada y salida
#include <stdlib.h>     // Se incluye funciones estándar como malloc, exit, atoi
#include <string.h>     // Se incluye funciones para manejo de cadenas
#include <unistd.h>     // Se incluyen funciones POSIX (read, write, close).
#include <errno.h>      // Se incluir manejo de códigos de error del sistema
#include <fcntl.h>      // Se habilita la apertura y configuración de archivos.
#include <sys/types.h>  // Se habilitan tipos del sistema.
#include <sys/stat.h>   // Se habilitan funciones para manejo de archivos y atributos.

// Se define el tamaño máximo permitido para nombres.
#define MAX_NOMBRE    64
// Se define el tamaño máximo permitido para nombre de familia.
#define MAX_FAMILIA   64
// Se define el tamaño máximo permitido para el nombre de un pipe.
#define MAX_PIPE_NAME 128
// Se define el tamaño máximo permitido para una línea del archivo CSV.
#define LINEA_MAX     256

/* Códigos de respuesta (mismos que en el controlador) */
#define RESP_OK                0
#define RESP_REPROGRAMADA      1
#define RESP_NEG_EXTEMPORANEA  2
#define RESP_NEG_SIN_CUPO      3
#define RESP_NEG_FUERA_RANGO   4
#define RESP_NEG_AFORO_EXCEDE  5

/* Mensaje que envía el Agente al Controlador */
// Se define la estructura usada para enviar solicitudes al controlador.
typedef struct {
    int tipo;                               // Se indica si el mensaje es registro o solicitud.
    char nombre_agente[MAX_NOMBRE];         // Se almacena el nombre del agente.
    char pipe_respuesta[MAX_PIPE_NAME];     // Se almacena el nombre del pipe de respuesta.
    char familia[MAX_FAMILIA];              // Se almacena el nombre de la familia solicitante.
    int hora_solicitada;                    // Se almacena la hora pedida.
    int n_personas;                         // Se almacena la cantidad de personas.
} MensajeSolicitud;

/* Mensaje de respuesta del Controlador */
// Se define la estructura usada para recibir respuestas del controlador.
typedef struct {
    int codigo_respuesta;                   // Se indica el tipo de respuesta.
    int hora_asignada;                      // Se almacena la hora final asignada.
    int hora_asignada_fin;                  // Se almacena la hora de salida.
    char mensaje[128];                      // Se almacena el mensaje descriptivo.
} MensajeRespuesta;

/* Variables globales */
// Se declaran variables globales para almacenar los parámetros recibidos.
extern char nombreAgente[MAX_NOMBRE];         // Se almacena el nombre del agente.
extern char archivoSolicitudes[128];          // Se almacena el archivo CSV de solicitudes.
extern char pipeRecibe[MAX_PIPE_NAME];        // Se almacena el pipe hacia el controlador.
extern char pipeRespuesta[MAX_PIPE_NAME];     // Se almacena el pipe por el cual llegan respuestas.
extern int fd_pipe_controlador;               // Se almacena el descriptor del pipe del controlador.
extern int fd_pipe_respuesta;                 // Se almacena el descriptor del pipe de respuesta.

/* Prototipos */
// Se declaran prototipos de funciones usadas en el programa.
void parsearArgumentos(int argc, char *argv[]);
void registrarAgente();
void procesarSolicitudes();
void limpiarNuevaLinea(char *str);
