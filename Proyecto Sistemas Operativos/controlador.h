/********************************************************
*  Pontificia Universidad Javeriana
*  Facultad de Ingeniería – Ingeniería de Sistemas
*
*  Proyecto: Sistema Simulador de Reservas de Parque
*  Archivo: agente.h
*  Descripción:
*     Archivo de cabecera del proceso cliente "AControlador de
*     reserva". Contiene las definiciones de constantes,
*     estructuras de datos, variables globales y los
*     prototipos de funciones empleadas en el módulo
*     principal y en las funciones auxiliares del controlador.
*
*  Autores:
*     Santiago Bautista Velasquez
*     Alejandro Lopez
*     David Calderon
*
*  Fecha: 16/11/25
********************************************************/
#ifndef CONTROLADOR_H
#define CONTROLADOR_H

#include <stdio.h>          // Se incluye la librería estándar para entrada y salida
#include <stdlib.h>         // Se incluye la librería estándar para funciones generales
#include <string.h>         // Se incluye para manejo de cadenas
#include <unistd.h>         // Se incluye para llamadas al sistema POSIX
#include <errno.h>          // Se incluye para manejo de errores del sistema
#include <fcntl.h>          // Se incluye para manejo de archivos y flags
#include <sys/types.h>      // Se incluye para tipos de datos del sistema
#include <sys/stat.h>       // Se incluye para manejo de permisos y estados
#include <pthread.h>        // Se incluye para manejo de hilos POSIX

#define HORA_MIN 7
#define HORA_MAX 19
#define MAX_HORAS (HORA_MAX + 1)

#define MAX_NOMBRE    64
#define MAX_FAMILIA   64
#define MAX_PIPE_NAME 128

#define MAX_AGENTES   32
#define MAX_RESERVAS  1024

#define RESP_OK                0
#define RESP_REPROGRAMADA      1
#define RESP_NEG_EXTEMPORANEA  2
#define RESP_NEG_SIN_CUPO      3
#define RESP_NEG_FUERA_RANGO   4
#define RESP_NEG_AFORO_EXCEDE  5

/* ----------- ESTRUCTURAS ----------- */

typedef struct {
    char familia[MAX_FAMILIA];
    int hora_inicio;
    int hora_fin;
    int personas;
} Reserva;

typedef struct {
    int tipo;
    char nombre_agente[MAX_NOMBRE];
    char pipe_respuesta[MAX_PIPE_NAME];
    char familia[MAX_FAMILIA];
    int hora_solicitada;
    int n_personas;
} MensajeSolicitud;

typedef struct {
    int codigo_respuesta;
    int hora_asignada;
    int hora_asignada_fin;
    char mensaje[128];
} MensajeRespuesta;

typedef struct {
    int activo;
    char nombre_agente[MAX_NOMBRE];
    char pipe_respuesta[MAX_PIPE_NAME];
    int fd_respuesta;
} AgenteInfo;

/* ----------- VARIABLES GLOBALES EXPORTADAS ----------- */

extern int horaIni;
extern int horaFin;
extern int segHoras;
extern int aforoMaximo;
extern char nombrePipeRecibe[MAX_PIPE_NAME];

extern int horaActual;
extern int terminarSimulacion;

extern int personas_en_hora[MAX_HORAS];
extern Reserva reservas[MAX_RESERVAS];
extern int num_reservas;

extern AgenteInfo agentes[MAX_AGENTES];

extern pthread_mutex_t mutex_estado;
extern int fd_pipe_recibe;

/* ----------- PROTOTIPOS DE FUNCIONES ----------- */

void parsearArgumentos(int argc, char *argv[]);
void validarParametros();
void inicializarEstado();
void crearYPonerPipeRecibe();

void *hiloReloj(void *arg);
void *hiloAtencionSolicitudes(void *arg);

void procesarMensaje(const MensajeSolicitud *msg);
void avanzarHoraSimulacion();
void imprimirEstadoHora();
void generarReporteFinal();

int registrarAgente(const MensajeSolicitud *msg);
int buscarIndiceAgente(const char *nombre_agente);
int enviarRespuestaAAgente(const char *nombre_agente, const MensajeRespuesta *resp);

int buscarBloqueDosHoras(int hora_inicio_busqueda, int personas, int *hora_encontrada);
void agregarReserva(const char *familia, int hora_inicio, int personas);

void errorFatal(const char *msg);

#endif

