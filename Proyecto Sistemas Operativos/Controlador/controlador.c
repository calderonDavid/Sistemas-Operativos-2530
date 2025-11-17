/*******************************************************
*  Pontificia Universidad Javeriana
*  Facultad de Ingeniería – Ingeniería de Sistemas
*
*  Proyecto: Sistema Simulador de Reservas de Parque
*  Archivo: controlador.c
*  Descripción:
*     Proceso servidor "Controlador de Reserva".
*     Recibe solicitudes de reserva desde múltiples
*     Agentes de Reserva a través de un pipe nominal
*     y decide aceptar, reprogramar o negar las reservas
*     con base en el aforo por hora.
*
*  Autores:
*     	Santiago Bautista Velasquez
*	    Alejandro Lopez
*	    David Calderon
*
*  Fecha: 16/10/2025
*************************************************************/
#include "controlador.h"

int main(int argc, char *argv[]) {  
    pthread_t thReloj;          // Se declara el identificador del hilo que manejará el reloj de la simulación
    pthread_t thSolicitudes;    // Se declara el identificador del hilo que procesará las solicitudes
    int rc;                     // Se declara variable para guardar códigos de retorno de pthread_create

    parsearArgumentos(argc, argv);  // Se procesan los argumentos enviados por consola
    validarParametros();            // Se validan los parámetros recibidos (rangos y consistencia)
    inicializarEstado();            // Se inicializan variables globales y estructuras del controlador
    crearYPonerPipeRecibe();        // Se crea el pipe global del controlador y se abre en modo no bloqueante

    rc = pthread_create(&thReloj, NULL, hiloReloj, NULL);   // Se crea el hilo encargado del avance del tiempo
    if (rc != 0) {                                          // Se verifica si ocurrió un error al crear el hilo del reloj
        errorFatal("Error al crear hilo del reloj");        // Se imprime error y se termina la ejecución
    }

    rc = pthread_create(&thSolicitudes, NULL, hiloAtencionSolicitudes, NULL);   // Se crea el hilo que atiende solicitudes de agentes
    if (rc != 0) {                                                              // Se verifica si ocurrió un error al crear el hilo de solicitudes
        errorFatal("Error al crear hilo de atención de solicitudes");           // Se imprime error y se finaliza
    }

    pthread_join(thReloj, NULL);        // Se espera a que el hilo del reloj termine su ejecución
    pthread_join(thSolicitudes, NULL);  // Se espera a que el hilo de solicitudes finalice completamente
    generarReporteFinal();              // Se genera el reporte final de estadísticas y ocupación del parque

    if (fd_pipe_recibe != -1) {         // Se verifica que el descriptor del pipe global esté abierto
        close(fd_pipe_recibe);          // Se cierra el pipe global del controlador
    }
    unlink(nombrePipeRecibe);           // Se elimina el archivo FIFO asociado al pipe global

    for (int i = 0; i < MAX_AGENTES; i++) {                         // Se recorren todos los agentes registrados
        if (agentes[i].activo && agentes[i].fd_respuesta != -1) {   // Se verifica que el agente esté activo y su pipe abierto
            close(agentes[i].fd_respuesta);                         // Se cierra el pipe de respuesta de ese agente
        }
    }

    return 0; // Se finaliza la ejecución del programa correctamente
}
