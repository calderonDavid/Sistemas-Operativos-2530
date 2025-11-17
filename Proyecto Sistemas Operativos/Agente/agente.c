/********************************************************
*  Pontificia Universidad Javeriana
*  Facultad de Ingeniería – Ingeniería de Sistemas
*
*  Proyecto: Sistema Simulador de Reservas de Parque
*  Archivo: agente.c
*  Descripción:
*     Proceso cliente "Agente de Reserva".
*     Lee un archivo CSV con solicitudes de reserva y
*     se comunica con el Controlador de Reserva mediante
*     pipes nominales.
*
*  Autores:
*    	Santiago Bautista Velasquez
*	Alejandro Lopez
*	David Calderon
*
*  Fecha: 16/11/25
********************************************************/
#include "agente.h"

int main(int argc, char *argv[]) {

    parsearArgumentos(argc, argv);	// Se procesan los argumentos recibidos desde la línea de comandos.

    /* Construir el nombre del pipe de respuesta: pipe_resp_<agente> */
    snprintf(pipeRespuesta, sizeof(pipeRespuesta), "pipe_resp_%s", nombreAgente);	// Se construye el nombre del pipe de respuesta usando el nombre del agente.

    /* Crear el pipe de respuesta (pero NO lo abrimos todavía) */
    if (mkfifo(pipeRespuesta, 0666) == -1) {		// Se crea el pipe FIFO que usará el controlador para enviar respuestas al agente.
        if (errno != EEXIST) {				        // Se verifica si el error no corresponde a que el pipe ya existe.
            perror("mkfifo pipeRespuesta");		    // Se imprime error, si la creación del pipe falla.
            exit(EXIT_FAILURE);				        // Se finaliza el programa .
        }
    }

    /* Abrir el pipe del controlador en modo escritura */
    fd_pipe_controlador = open(pipeRecibe, O_WRONLY);	// Se abre el pipe del controlador para enviarle mensajes.
    if (fd_pipe_controlador == -1) {   
        perror("open pipeRecibe");			            // Se imprime un mensaje de error si no se puede abrir el pipe del controlador.
        exit(EXIT_FAILURE);   				            // Se cierra el programa porque no hay conexión con el controlador.
    }

    /* Registrar agente con el controlador (dentro se abre el pipeRespuesta) */
    registrarAgente();      // Se registra el agente ante el controlador enviando un mensaje inicial.

    /* Ahora ya tenemos fd_pipe_respuesta abierto y podemos procesar solicitudes */
    procesarSolicitudes();	// Se procesan todas las solicitudes del archivo CSV y se envían al controlador.
    printf("Agente %s termina.\n", nombreAgente);

    
    close(fd_pipe_controlador);		// Se cierra el descriptor del pipe hacia el controlador.
    close(fd_pipe_respuesta);		// Se cierra el descriptor del pipe por el cual se reciben respuestas.
    unlink(pipeRespuesta);		    // Se elimina físicamente el pipe FIFO de respuesta del agente.

    return 0;
}


