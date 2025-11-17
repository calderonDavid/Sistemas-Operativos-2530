/********************************************************
*  Pontificia Universidad Javeriana
*  Facultad de Ingeniería – Ingeniería de Sistemas
*
*  Proyecto: Sistema Simulador de Reservas de Parque
*  Archivo: agenteMod.c
*  Descripción:
*     Módulo auxiliar del proceso cliente "Agente de
*     Reserva". Implementa las funciones encargadas del
*     manejo de argumentos, registro del agente, envío
*     de solicitudes, recepción de respuestas y lectura
*     de archivos '.csv'. Funciona en conjunto con el
*     archivo principal agente.c.
*
*  Autores:
*     Santiago Bautista Velasquez
*     Alejandro Lopez
*     David Calderon
*
*  Fecha: 12/11/25
********************************************************/

#include "agente.h"

/* Variables globales */
// Se declaran variables globales para almacenar los parámetros recibidos.
char nombreAgente[MAX_NOMBRE] = "";         // Se almacena el nombre del agente.
char archivoSolicitudes[128] = "";          // Se almacena el archivo CSV de solicitudes.
char pipeRecibe[MAX_PIPE_NAME] = "";        // Se almacena el pipe hacia el controlador.
char pipeRespuesta[MAX_PIPE_NAME] = "";     // Se almacena el pipe por el cual llegan respuestas.

int fd_pipe_controlador = -1;               // Se almacena el descriptor del pipe del controlador.
int fd_pipe_respuesta   = -1;               // Se almacena el descriptor del pipe de respuesta.

/*********************************************
 *     Funciones Principales de Agente
 *********************************************/

void parsearArgumentos(int argc, char *argv[]) {
    int i = 1; 	// Se inicializa el índice para recorrer los argumentos desde la posición 1.

    if (argc < 7) {	// Se verifica si la cantidad mínima de argumentos no se cumple.
        fprintf(stderr,"Uso: %s -s nombreAgente -a archivoSolicitudes -p pipeRecibe\n", argv[0]);	// Se imprime mensaje de uso incorrecto.
        exit(EXIT_FAILURE);	// Se finaliza el programa por error.
    }

    while (i < argc) { 	// Se inicia el ciclo para procesar cada argumento.
        if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) { 		// Se compara si el argumento actual es -s.
            strncpy(nombreAgente, argv[++i], MAX_NOMBRE - 1); 		// Se copia el nombre del agente desde argv.
            nombreAgente[MAX_NOMBRE - 1] = '\0'; 			// Se asegura la terminación nula.
        } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) { 	// Se compara si el argumento actual es -a.
            strncpy(archivoSolicitudes, argv[++i], sizeof(archivoSolicitudes) - 1); // Se copia la ruta del archivo.
            archivoSolicitudes[sizeof(archivoSolicitudes) - 1] = '\0'; 	// Se asegura la terminación nula.
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) { 	// Se compara si el argumento actual es -p.
            strncpy(pipeRecibe, argv[++i], sizeof(pipeRecibe) - 1); 	// Se copia el pipe de recepción.
            pipeRecibe[sizeof(pipeRecibe) - 1] = '\0'; 			// Se asegura la terminación nula.
        } else {
            fprintf(stderr, "Argumento no reconocido: %s\n", argv[i]); // Se imprime mensaje de error por argumento inválido.
            exit(EXIT_FAILURE); // Se finaliza el programa.
        }
        i++; // Se incrementa el índice para continuar con el siguiente argumento.
    }

    if (strlen(nombreAgente) == 0 || strlen(archivoSolicitudes) == 0 || strlen(pipeRecibe) == 0) { // Se verifica si algún argumento requerido quedó vacío.
        fprintf(stderr, "Error: argumentos incompletos.\n");
        exit(EXIT_FAILURE);	// Se finaliza el programa.
    }
}

void registrarAgente() {
    MensajeSolicitud msg; 	// Se declara estructura para enviar datos al controlador.
    MensajeRespuesta resp; 	// Se declara estructura para recibir datos del controlador.
    int escritos, leidos; 	// Se declaran variables para almacenar bytes enviados y leídos.

    /* Inicializamos SOLO los campos que vamos a usar */
    msg.tipo = 0; 			// Se define el tipo de mensaje como registro (0).
    msg.nombre_agente[0] = '\0'; 	// Se inicializa cadena nombre_agente.
    msg.pipe_respuesta[0] = '\0'; 	// Se inicializa cadena pipe_respuesta.
    msg.familia[0] = '\0'; 		// Se inicializa cadena familia.
    msg.hora_solicitada = 0; 		// Se inicializa hora solicitada en 0.
    msg.n_personas = 0; 		// Se inicializa cantidad de personas en 0.

    strncpy(msg.nombre_agente, nombreAgente, MAX_NOMBRE - 1); 		// Se copia el nombre del agente al mensaje.
    msg.nombre_agente[MAX_NOMBRE - 1] = '\0'; 				// Se asegura la terminación nula.
    strncpy(msg.pipe_respuesta, pipeRespuesta, MAX_PIPE_NAME - 1); 	// Se copia el nombre del pipe de respuesta.
    msg.pipe_respuesta[MAX_PIPE_NAME - 1] = '\0'; 			// Se asegura la terminación nula.
    
    /* Enviar registro al controlador */
    escritos = write(fd_pipe_controlador, &msg, sizeof(MensajeSolicitud));	// Se envía el mensaje al pipe del controlador.
    if (escritos != (int)sizeof(MensajeSolicitud)) {				// Se verifica si la escritura fue correcta.
        fprintf(stderr, "Error enviando registro al controlador.\n");		// Se imprime error.
        exit(EXIT_FAILURE);							// Se finaliza el programa.
    }

    /* AHORA sí: abrir mi pipe de respuesta para leer. */
    fd_pipe_respuesta = open(pipeRespuesta, O_RDONLY); 		// Se abre el pipe de respuesta del agente para lectura.
    if (fd_pipe_respuesta == -1) { 				// Se verifica si hubo error al abrir.
        perror("open pipeRespuesta"); 				// Se imprime error.
        exit(EXIT_FAILURE); 					// Se finaliza el programa.
    }

    /* Esperar respuesta del controlador */
    leidos = read(fd_pipe_respuesta, &resp, sizeof(MensajeRespuesta)); 		// Se lee la respuesta del controlador.
    if (leidos != (int)sizeof(MensajeRespuesta)) { 				// Se verifica que la lectura fue completa.
        fprintf(stderr, "Error leyendo respuesta de registro.\n"); 		// Se imprime error.
        exit(EXIT_FAILURE); 							// Se finaliza el programa.
    }
    
    printf("[AGENTE %s] %s\n", nombreAgente, resp.mensaje); // Se imprime el mensaje de registro exitoso.
}

void procesarSolicitudes() {

    FILE *f = fopen(archivoSolicitudes, "r"); 	// Se abre el archivo '.csv' de solicitudes
    if (!f) { 					// Se verifica si la apertura falló
        perror("fopen archivoSolicitudes"); 	// Se imprime error
        exit(EXIT_FAILURE); 			// Se finaliza el programa
    }

    char linea[LINEA_MAX];		// Se declara buffer para leer cada línea del '.csv'

    while (fgets(linea, LINEA_MAX, f) != NULL) { // Se lee cada línea del archivo

        limpiarNuevaLinea(linea); // Se elimina el salto de línea final
        if (strlen(linea) < 3) continue; // Se evita procesar líneas vacías

        /* Parsear CSV: familia,hora,personas */
        char *token = strtok(linea, ","); 		// Se obtiene la familia
        if (!token) continue; 				// Se valida el token
        char familia[MAX_FAMILIA]; 			// Se declara buffer para almacenar familia
        strncpy(familia, token, MAX_FAMILIA - 1); 	// Se copia el valor familia
        familia[MAX_FAMILIA - 1] = '\0'; 		// Se asegura la terminación nula

        token = strtok(NULL, ","); 		// Se obtiene la hora
        if (!token) continue; 			// Se valida
        int hora = atoi(token); 		// Se convierte a entero

        token = strtok(NULL, ","); 		// Se obtiene el número de personas.
        if (!token) continue; 			// Se valida
        int personas = atoi(token); 		// Se convierte a entero

        /* Preparar mensaje */
        MensajeSolicitud msg; 	// Se declara estructura para enviar datos al controlador.
        MensajeRespuesta resp; 	// Se declara estructura para recibir datos del controlador.
        int escritos, leidos; 	// Se declaran variables para almacenar bytes enviados y leídos.

        msg.tipo = 1; 			        // Se define el tipo de mensaje como solicitud (1).
        msg.nombre_agente[0] = '\0'; 	// Se inicializa cadena nombre_agente.
        msg.pipe_respuesta[0] = '\0'; 	// Se inicializa cadena pipe_respuesta.
        msg.familia[0] = '\0'; 		    // Se inicializa cadena familia.
        msg.hora_solicitada = hora; 	// Se inicializa hora solicitada en la hora solicitada.
        msg.n_personas = personas; 		// Se inicializa cantidad de personas de la familia.

        strncpy(msg.nombre_agente, nombreAgente, MAX_NOMBRE - 1); 		// Se copia nombre del agente al mensaje.
        msg.nombre_agente[MAX_NOMBRE - 1] = '\0'; 				        // Se asegura la terminación nula.
        strncpy(msg.pipe_respuesta, pipeRespuesta, MAX_PIPE_NAME - 1); 	// Se copia el pipe respuesta.
        msg.pipe_respuesta[MAX_PIPE_NAME - 1] = '\0'; 				    // Se asegura la terminación nula.
        strncpy(msg.familia, familia, MAX_FAMILIA - 1); 			    // Se copia el nombre de familia.
        msg.familia[MAX_FAMILIA - 1] = '\0'; 					        // Se asegura la terminación nula.

        /* Enviar la solicitud */
        escritos = write(fd_pipe_controlador, &msg, sizeof(MensajeSolicitud)); 		    // Se envía el mensaje al controlador.
        if (escritos != (int)sizeof(MensajeSolicitud)) { 				                // Se verifica la escritura.
            fprintf(stderr, "[AGENTE %s] Error enviando solicitud.\n", nombreAgente); 	// Se imprime error.
            break; 									                                    // Se sale del ciclo.
        }

        /* Recibir respuesta */
        leidos = read(fd_pipe_respuesta, &resp, sizeof(MensajeRespuesta)); 		        // Se lee respuesta del controlador.
        if (leidos != (int)sizeof(MensajeRespuesta)) { 					                // Se verifica lectura.
            fprintf(stderr, "[AGENTE %s] Error leyendo respuesta.\n", nombreAgente); 	// Se imprime error.
            break; 									                                    // Se sale del ciclo.
        }
        
        /* Imprimir respuesta */
        printf("[AGENTE %s] Familia: %s → %s\n", nombreAgente, familia, resp.mensaje);	// Se imprime la respuesta del controlador.

        sleep(2);	// Se espera 2 segundos antes de procesar la siguiente solicitud.
    }

    fclose(f);	// Se cierra el archivo de solicitudes.
}

void limpiarNuevaLinea(char *str) {
    int n = strlen(str);		// Se obtiene la longitud de la cadena.
    if (n > 0 && str[n - 1] == '\n') {	// Se verifica si el último carácter es un salto de línea.
        str[n - 1] = '\0';		// Se reemplaza el salto de línea por terminación nula.
    }
}