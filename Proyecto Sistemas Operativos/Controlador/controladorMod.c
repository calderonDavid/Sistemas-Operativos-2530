/********************************************************
*  Pontificia Universidad Javeriana
*  Facultad de Ingeniería – Ingeniería de Sistemas
*
*  Proyecto: Sistema Simulador de Reservas de Parque
*  Archivo: agente.h
*  Descripción:
*     Archivo de cabecera del proceso cliente "Controlador de
*     reserva". Contiene las definiciones de constantes,
*     estructuras de datos, variables globales y los
*     prototipos de funciones empleadas en el módulo
*     principal y en las funciones auxiliares del controlador.
*
*  Autores:
*       David Calderon
*    	Santiago Bautista Velasquez
*	    Alejandro Lopez
*
*  Fecha: 16/11/25
********************************************************/

#include "controlador.h"

/* Variables globales de configuración */
int horaIni = 0;
int horaFin = 0;
int segHoras = 0;
int aforoMaximo = 0;
char nombrePipeRecibe[MAX_PIPE_NAME];

/* Estado de la simulación */
int horaActual = 0;
int terminarSimulacion = 0;

/* Aforo por hora */
int personas_en_hora[MAX_HORAS];

/* Reservas para estadísticas */
Reserva reservas[MAX_RESERVAS];
int num_reservas = 0;

/* Agentes registrados */
AgenteInfo agentes[MAX_AGENTES];

/* Estadísticas */
int cnt_aceptadas_hora      = 0;
int cnt_reprogramadas       = 0;
int cnt_negadas_total       = 0;
int cnt_negadas_extemp      = 0;
int cnt_negadas_sin_cupo    = 0;
int cnt_negadas_fuera_rango = 0;
int cnt_negadas_aforo       = 0;

/* Mutex */
pthread_mutex_t mutex_estado = PTHREAD_MUTEX_INITIALIZER;

/* Descriptor del pipe global */
int fd_pipe_recibe = -1;

/******* Implementaciones ********/

void parsearArgumentos(int argc, char *argv[]) {
    int i = 1; // Se inicializa el índice para recorrer los argumentos desde la posición 1

    if (argc < 11) { // Se verifica si la cantidad mínima de argumentos no fue suministrada
        fprintf(stderr,
            "Uso: %s -i horaIni -f horaFin -s segHoras -t aforoMaximo -p pipeRecibe\n",
            argv[0]); // Se imprime mensaje indicando al usuario el formato correcto de ejecución
        exit(EXIT_FAILURE); // Se finaliza el programa debido a la falta de argumentos obligatorios
    }

    while (i < argc) { // Se inicia un ciclo para recorrer todos los argumentos proporcionados
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) { // Se compara si el argumento actual es "-i"
            horaIni = atoi(argv[++i]); // Se convierte el siguiente argumento a entero y se asigna a horaIni
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) { // Se verifica si el argumento es "-f"
            horaFin = atoi(argv[++i]); // Se obtiene la hora final y se convierte a entero
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) { // Se verifica si el argumento es "-s"
            segHoras = atoi(argv[++i]); // Se convierte el número de segundos por hora y se asigna
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) { // Se verifica si el argumento es "-t"
            aforoMaximo = atoi(argv[++i]); // Se obtiene el valor del aforo máximo permitido
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) { // Se verifica si el argumento es "-p"
            strncpy(nombrePipeRecibe, argv[++i], MAX_PIPE_NAME - 1); // Se copia el nombre del pipe recibido por parámetro
            nombrePipeRecibe[MAX_PIPE_NAME - 1] = '\0'; // Se garantiza la terminación nula de la cadena
        } else { // Si no coincide con ninguno de los parámetros esperados
            fprintf(stderr, "Parámetro no reconocido o faltan argumentos: %s\n", argv[i]); // Se imprime mensaje de error
            exit(EXIT_FAILURE); // Se termina el programa por argumento inválido
        }
        i++; // Se incrementa el índice para pasar al siguiente argumento
    }
}

void validarParametros(void) {ñe
    if (horaIni < HORA_MIN || horaIni > HORA_MAX) { // Se verifica si la hora inicial está fuera del rango permitido
        fprintf(stderr, "Error: horaIni (%d) fuera del rango [%d, %d]\n", horaIni, HORA_MIN, HORA_MAX); // Se imprime mensaje de error indicando el rango válido
        exit(EXIT_FAILURE); // Se finaliza el programa debido a parámetro inválido
    }

    if (horaFin < HORA_MIN || horaFin > HORA_MAX || horaFin <= horaIni) { // Se verifica si la hora final es inválida o menor/igual que la inicial
        fprintf(stderr, "Error: horaFin (%d) debe estar en [%d, %d] y ser > horaIni (%d)\n", horaFin, HORA_MIN, HORA_MAX, horaIni); // Se imprime mensaje específico indicando el error
        exit(EXIT_FAILURE); // Se termina la ejecución porque los parámetros no tienen coherencia
    }

    if (segHoras <= 0) { // Se verifica que el valor del tiempo por hora simulada sea mayor que 0
        fprintf(stderr, "Error: segHoras debe ser mayor que 0\n"); // Se informa que el valor no es válido
        exit(EXIT_FAILURE); // Se detiene el programa
    }

    if (aforoMaximo <= 0) { // Se verifica que el aforo máximo sea un entero positivo
        fprintf(stderr, "Error: aforo máximo debe ser mayor que 0\n"); // Se imprime mensaje de error
        exit(EXIT_FAILURE); // Se detiene ejecución
    }

    if (strlen(nombrePipeRecibe) == 0) { // Se comprueba que se haya proporcionado un nombre de pipe válido
        fprintf(stderr, "Error: debe especificar un nombre de pipe (-p)\n"); // Se advierte que el argumento es obligatorio
        exit(EXIT_FAILURE); // Se finaliza por falta de parámetro esencial
    }
}

void inicializarEstado(void) {
    horaActual = horaIni; // Se asigna la hora inicial de simulación como hora actual
    terminarSimulacion = 0; // Se indica que la simulación aún no debe finalizar

    for (int h = 0; h < MAX_HORAS; h++) { // Se recorre el arreglo que lleva el conteo de personas por hora
        personas_en_hora[h] = 0; // Se inicializa el número de personas en cada hora a 0
    }
    num_reservas = 0; // Se reinicia el contador total de reservas almacenadas

    for (int i = 0; i < MAX_AGENTES; i++) { // Se recorre la lista de agentes registrados
        agentes[i].activo = 0; // Se marca cada agente como inactivo
        agentes[i].fd_respuesta = -1; // Se indica que su pipe de respuesta aún no está abierto
        agentes[i].nombre_agente[0] = '\0'; // Se limpia el nombre del agente colocando terminador nulo
        agentes[i].pipe_respuesta[0] = '\0'; // Se limpia el nombre del pipe de respuesta
    }

    cnt_aceptadas_hora      = 0; // Se inicializa contador de solicitudes aceptadas en hora solicitada
    cnt_reprogramadas       = 0; // Se inicializa contador de solicitudes reprogramadas
    cnt_negadas_total       = 0; // Se inicializa contador total de solicitudes negadas
    cnt_negadas_extemp      = 0; // Se inicializa contador de negaciones por extemporáneas
    cnt_negadas_sin_cupo    = 0; // Se inicializa contador de negaciones por falta de cupo
    cnt_negadas_fuera_rango = 0; // Se inicializa contador de solicitudes fuera del rango permitido
    cnt_negadas_aforo       = 0; // Se inicializa contador de negaciones por exceder el aforo máximo
}

void crearYPonerPipeRecibe(void) {
    int rc = mkfifo(nombrePipeRecibe, 0666); // Se crea el FIFO con permisos de lectura y escritura para todos
    if (rc == -1) { // Se verifica si ocurrió un error al intentar crear el FIFO
        if (errno != EEXIST) { // Se comprueba si el error NO es porque el FIFO ya existía
            perror("mkfifo"); // Se imprime el mensaje de error generado por el sistema
            exit(EXIT_FAILURE); // Se termina la ejecución debido al fallo crítico
        }
    }

    fd_pipe_recibe = open(nombrePipeRecibe, O_RDONLY | O_NONBLOCK); // Se abre el FIFO en modo lectura y no bloqueante
    if (fd_pipe_recibe == -1) { // Se verifica si ocurrió un error al abrir el FIFO
        perror("open pipeRecibe"); // Se muestra el error correspondiente
        exit(EXIT_FAILURE); // Se detiene la ejecución al no poder operar sin este recurso
    }

    printf("Controlador: pipe de recepción '%s' creado y abierto.\n", nombrePipeRecibe); // Se informa que el pipe fue creado y abierto correctamente
    printf("Simulación desde hora %d hasta %d, %d seg/hora, aforo máximo %d personas.\n", horaIni, horaFin, segHoras, aforoMaximo); // Se imprime la configuración inicial de la simulación
}

void *hiloReloj(void *arg) {
    (void)arg; // Se descarta el argumento ya que no se utiliza en esta función

    while (1) { // Se inicia un ciclo infinito para simular el paso del tiempo
        sleep(segHoras); // Se espera la cantidad de segundos que corresponde a una "hora" simulada

        pthread_mutex_lock(&mutex_estado); // Se adquiere el mutex para evitar condiciones de carrera sobre el estado

        if (horaActual >= horaFin) { // Se verifica si ya se alcanzó o superó la hora final de simulación
            terminarSimulacion = 1; // Se indica que la simulación debe finalizar
            pthread_mutex_unlock(&mutex_estado); // Se libera el mutex antes de salir del hilo
            break; // Se interrumpe el ciclo para terminar el hilo
        }

        horaActual++; // Se incrementa la hora simulada actual
        imprimirEstadoHora(); // Se imprime el estado correspondiente a la nueva hora

        pthread_mutex_unlock(&mutex_estado); // Se libera el mutex para permitir que otros hilos accedan al estado
    }

    printf("Controlador: hiloReloj termina. Día de simulación finalizado.\n"); // Se informa que el hilo del reloj terminó
    return NULL; // Se retorna NULL para finalizar el hilo correctamente
}

void *hiloAtencionSolicitudes(void *arg) {
    (void)arg; // Se descarta el argumento ya que no se usa en esta función
    ssize_t leidos; // Se declara variable para almacenar cuántos bytes fueron leídos del pipe
    MensajeSolicitud msg; // Se declara estructura donde se almacenará el mensaje leído

    while (1) { // Se inicia un ciclo para atender solicitudes continuamente
        pthread_mutex_lock(&mutex_estado); // Se adquiere el mutex para revisar el estado de finalización
        int terminar = terminarSimulacion; // Se copia el valor de si la simulación debe terminar
        pthread_mutex_unlock(&mutex_estado); // Se libera el mutex para no bloquear otros hilos

        if (terminar) { // Se verifica si la simulación terminó
            break; // Se rompe el ciclo para finalizar el hilo
        }

        leidos = read(fd_pipe_recibe, &msg, sizeof(MensajeSolicitud)); // Se intenta leer un mensaje del pipe global
        if (leidos == -1) { // Se verifica si hubo un error en la lectura
            if (errno == EAGAIN || errno == EWOULDBLOCK) { // Se revisa si el pipe está vacío (modo no bloqueante)
                usleep(100000); // Se espera brevemente antes de volver a intentar leer
                continue; // Se vuelve al inicio del ciclo
            }
            perror("read pipeRecibe"); // Se imprime error en caso de un fallo real
            break; // Se termina el hilo por error grave
        } else if (leidos == 0) { // Se verifica si no se recibieron datos (EOF temporal)
            usleep(100000); // Se duerme un poco para evitar ocupación excesiva de CPU
            continue; // Se continúa al siguiente ciclo de lectura
        } else if (leidos != (ssize_t)sizeof(MensajeSolicitud)) { // Se verifica si el mensaje está incompleto o corrupto
            fprintf(stderr, "Advertencia: tamaño de mensaje inesperado (%zd bytes)\n", leidos); // Se advierte sobre el tamaño incorrecto
            continue; // Se ignora el mensaje defectuoso
        }

        procesarMensaje(&msg); // Se procesa el mensaje recibido enviándolo a la función central de lógica
    }

    printf("Controlador: hiloAtencionSolicitudes termina.\n"); // Se informa que el hilo ha finalizado
    return NULL; // Se retorna NULL como finalización correcta
}

void procesarMensaje(const MensajeSolicitud *msg) {
    if (msg->tipo == 0) { // Se verifica si el mensaje es de tipo registro de agente
        pthread_mutex_lock(&mutex_estado); // Se adquiere el mutex para proteger el estado global
        int idx = registrarAgente(msg); // Se registra el agente nuevo o se actualiza si ya existe
        int hora_actual_local = horaActual; // Se copia la hora actual localmente para evitar inconsistencia
        pthread_mutex_unlock(&mutex_estado); // Se libera el mutex tras modificar o consultar el estado

        printf("[Registro] Agente: %s, pipe respuesta: %s (idx=%d)\n", msg->nombre_agente, msg->pipe_respuesta, idx); // Se imprime información del registro realizado

        MensajeRespuesta resp; // Se crea la estructura de respuesta
        resp.codigo_respuesta  = RESP_OK; // Se asigna código de éxito
        resp.hora_asignada     = hora_actual_local; // Se devuelve la hora actual como referencia
        resp.hora_asignada_fin = hora_actual_local; // Se deja igual porque el registro no ocupa horas
        snprintf(resp.mensaje, sizeof(resp.mensaje), "Registro exitoso. Hora actual de simulación: %d", hora_actual_local); // Se genera el mensaje de texto para el agente
        enviarRespuestaAAgente(msg->nombre_agente, &resp); // Se envía la respuesta al agente mediante su pipe

    } else if (msg->tipo == 1) { // Se verifica si el mensaje es una solicitud de reserva
        MensajeRespuesta resp; // Se declara estructura para enviar la respuesta
        int hora_actual_local; // Se declara variable para almacenar la hora actual
        int hora_solicitada; // Se declara variable para la hora pedida en la solicitud
        int n_personas; // Se declara variable para cantidad de personas solicitadas

        pthread_mutex_lock(&mutex_estado); // Se adquiere el mutex para operar sobre el estado seguro

        hora_actual_local = horaActual; // Se obtiene la hora simulada actual
        hora_solicitada   = msg->hora_solicitada; // Se toma la hora solicitada por el agente
        n_personas        = msg->n_personas; // Se toma la cantidad de personas indicadas en la solicitud

        printf("[Solicitud] Agente: %s, Familia: %s, Hora: %d, Personas: %d\n", msg->nombre_agente, msg->familia, hora_solicitada, n_personas); // Se imprime información detallada de la solicitud recibida

        if (n_personas > aforoMaximo) { // Se verifica si la solicitud excede el aforo permitido
            resp.codigo_respuesta  = RESP_NEG_AFORO_EXCEDE; // Se asigna código de rechazo por aforo
            resp.hora_asignada     = -1; // Se indica que no hay hora asignada
            resp.hora_asignada_fin = -1; // Se deja sin hora final
            snprintf(resp.mensaje, sizeof(resp.mensaje), "Solicitud negada: número de personas (%d) excede el aforo máximo (%d).", n_personas, aforoMaximo); // Se explica la razón del rechazo
            cnt_negadas_total++; // Se incrementa el contador de negaciones totales
            cnt_negadas_aforo++; // Se incrementa el contador de negaciones por aforo

        } else if (hora_solicitada > horaFin) { // Se verifica si la hora solicitada está fuera del rango permitido
            resp.codigo_respuesta  = RESP_NEG_FUERA_RANGO; // Se asigna rechazo por rango inválido
            resp.hora_asignada     = -1; // No hay hora asignada
            resp.hora_asignada_fin = -1; // Sin hora final
            snprintf(resp.mensaje, sizeof(resp.mensaje), "Solicitud negada: hora solicitada (%d) es mayor a la hora fin de simulación (%d).", hora_solicitada, horaFin); // Se explica el rechazo
            cnt_negadas_total++; // Se incrementa contador total de negadas
            cnt_negadas_fuera_rango++; // Se incrementa por fuera de rango

        } else {
            int es_extemporanea = (hora_solicitada < hora_actual_local); // Se determina si la solicitud es pasada
            int hora_asignada   = -1; // Se inicializa la hora asignada como inválida
            int puede_ser_inicio = (hora_solicitada <= horaFin - 1); // Se verifica si la hora puede ser inicio de un bloque de 2 horas

            if (!es_extemporanea && puede_ser_inicio) { // Si la solicitud no es pasada y puede iniciar un bloque
                int h = hora_solicitada; // Se toma la hora solicitada para evaluar disponibilidad
                if (personas_en_hora[h] + n_personas <= aforoMaximo &&
                    personas_en_hora[h + 1] + n_personas <= aforoMaximo) { // Se verifica si ambas horas del bloque tienen aforo
                    hora_asignada = h; // Se asigna la hora solicitada como válida
                }
            }

            if (hora_asignada != -1) { // Se verifica si se pudo asignar la hora solicitada
                agregarReserva(msg->familia, hora_asignada, n_personas); // Se registra la reserva en el sistema
                resp.codigo_respuesta  = RESP_OK; // Se indica que la reserva fue aceptada
                resp.hora_asignada     = hora_asignada; // Se informa la hora asignada
                resp.hora_asignada_fin = hora_asignada + 2; // Se asignan dos horas consecutivas
                snprintf(resp.mensaje, sizeof(resp.mensaje), "Reserva aceptada en la hora solicitada. Ingreso: %d, salida: %d.", resp.hora_asignada, resp.hora_asignada_fin); // Se prepara mensaje confirmatorio
                cnt_aceptadas_hora++; // Se incrementa contador de reservas aceptadas en la hora original

            } else {
                int inicio_busqueda = es_extemporanea ? hora_actual_local : hora_solicitada; // Se define la hora para buscar un bloque disponible
                if (inicio_busqueda < horaIni) { // Se ajusta si la hora es menor que la hora mínima de simulación
                    inicio_busqueda = horaIni;
                }

                int h_encontrada = -1; // Se inicializa la variable donde se guardará la hora encontrada
                int ok_bloque = buscarBloqueDosHoras(inicio_busqueda, n_personas, &h_encontrada); // Se busca un bloque alternativo de 2 horas

                if (ok_bloque) { // Si se encuentra un bloque adecuado
                    agregarReserva(msg->familia, h_encontrada, n_personas); // Se registra la reserva reprogramada
                    resp.codigo_respuesta  = RESP_REPROGRAMADA; // Se indica reprogramación
                    resp.hora_asignada     = h_encontrada; // Se retorna la hora alternativa
                    resp.hora_asignada_fin = h_encontrada + 2; // Se establece el bloque de dos horas
                    snprintf(resp.mensaje, sizeof(resp.mensaje), "Reserva reprogramada. Nueva hora de ingreso: %d, salida: %d.", resp.hora_asignada, resp.hora_asignada_fin); // Se informa de la reprogramación
                    cnt_reprogramadas++; // Se incrementa el contador de reprogramadas

                } else { // Si no se encontró bloque disponible en ningún horario válido
                    resp.hora_asignada     = -1; // Se marca que no se pudo asignar hora
                    resp.hora_asignada_fin = -1; // Se deja sin hora final

                    if (es_extemporanea) { // Se verifica si fue negada por extemporánea
                        resp.codigo_respuesta = RESP_NEG_EXTEMPORANEA; // Se asigna código de extemporánea
                        snprintf(resp.mensaje, sizeof(resp.mensaje), "Solicitud negada: la hora solicitada (%d) ya pasó y no se encontró bloque de 2 horas disponible.", hora_solicitada); // Se genera mensaje explicativo
                        cnt_negadas_extemp++; // Se incrementa contador de negaciones por extemporaneidad
                    } else {
                        resp.codigo_respuesta = RESP_NEG_SIN_CUPO; // Se marca como negada por falta de cupo
                        snprintf(resp.mensaje, sizeof(resp.mensaje), "Solicitud negada: no hay cupo disponible en ningún bloque de 2 horas."); // Mensaje explicativo
                        cnt_negadas_sin_cupo++; // Se incrementa contador de negaciones por cupo
                    }
                    cnt_negadas_total++; // Se suman al total de negadas
                }
            }
        }

        pthread_mutex_unlock(&mutex_estado); // Se libera el mutex tras finalizar el procesamiento

        enviarRespuestaAAgente(msg->nombre_agente, &resp); // Se envía la respuesta final al agente que la solicitó

    } else { // Si el tipo de mensaje no es 0 ni 1
        printf("Mensaje con tipo desconocido: %d\n", msg->tipo); // Se imprime advertencia de tipo de mensaje inválido
    }
}

void imprimirEstadoHora() {
    printf("\n[Simulación] Ha transcurrido una hora. Hora actual: %d\n", horaActual); // Se imprime mensaje indicando el avance de una hora en la simulación y su valor actual

    int personas_dentro = 0; // Se inicializa un contador para sumar cuántas personas están dentro del parque

    printf("Familias que salen en esta hora: "); // Se imprime encabezado para mostrar familias que están saliendo
    int hay_salen = 0; // Se inicializa bandera para saber si alguna familia sale
    for (int i = 0; i < num_reservas; i++) { // Se recorre la lista de reservas registradas
        if (reservas[i].hora_fin == horaActual) { // Se verifica si la hora de salida coincide con la hora actual
            printf("%s(%d) ", reservas[i].familia, reservas[i].personas); // Se imprime nombre de familia y número de personas que salen
            hay_salen = 1; // Se marca que al menos una familia sale en esta hora
        }
    }
    if (!hay_salen) { // Se verifica si ninguna familia salió en esta hora
        printf("ninguna"); // Se imprime mensaje indicando ausencia de salidas
    }
    printf("\n"); // Se imprime un salto de línea para formato

    printf("Familias que entran en esta hora: "); // Se imprime encabezado para las entradas
    int hay_entran = 0; // Se inicializa bandera para saber si alguna familia entra
    for (int i = 0; i < num_reservas; i++) { // Se recorre nuevamente la lista de reservas
        if (reservas[i].hora_inicio == horaActual) { // Se verifica si la reserva inicia en esta hora
            printf("%s(%d) ", reservas[i].familia, reservas[i].personas); // Se imprime la familia y las personas que entran
            hay_entran = 1; // Se marca que hubo al menos una entrada
        }
    }
    if (!hay_entran) { // Se verifica si ninguna familia entró en esta hora
        printf("ninguna"); // Se imprime mensaje indicando ausencia de entradas
    }
    printf("\n"); // Se imprime un salto de línea para formato

    for (int i = 0; i < num_reservas; i++) { // Se recorre la lista completa de reservas para calcular personas dentro
        if (reservas[i].hora_inicio <= horaActual &&
            reservas[i].hora_fin > horaActual) { // Se verifica si la familia está dentro del horario activo
            personas_dentro += reservas[i].personas; // Se suman las personas de esa familia al total dentro del parque
        }
    }

    printf("Personas en el parque en esta hora: %d\n", personas_dentro); // Se imprime el total de personas presentes en el parque
}

void generarReporteFinal() {
    int h; // Se declara variable para recorrer las horas de simulación
    int max_personas = -1; // Se inicializa el contador de máximo de personas observadas en cualquier hora
    int min_personas = 1000000; // Se inicializa el contador de mínimo de personas con un valor muy alto

    for (h = horaIni; h <= horaFin; h++) { // Se recorre cada hora del rango simulado
        int p = personas_en_hora[h]; // Se obtiene cuántas personas estuvieron registradas en esa hora
        if (p > max_personas) max_personas = p; // Se actualiza el máximo si esta hora tiene más personas
        if (p < min_personas) min_personas = p; // Se actualiza el mínimo si esta hora tiene menos personas
    }

    printf("\n========== REPORTE FINAL DEL CONTROLADOR ==========\n"); // Se imprime encabezado del reporte final
    printf("Parámetros de simulación: horaIni=%d, horaFin=%d, aforoMáximo=%d personas\n", horaIni, horaFin, aforoMaximo); // Se imprimen los parámetros básicos usados en la simulación

    printf("\na) Horas pico (mayor número de personas = %d): ", max_personas); // Se imprime etiqueta para horas pico
    for (h = horaIni; h <= horaFin; h++) { // Se recorren todas las horas buscando horas con máximo de personas
        if (personas_en_hora[h] == max_personas) { // Se verifica si esta hora cumple el valor máximo
            printf("%d ", h); // Se imprime la hora correspondiente
        }
    }
    printf("\n"); // Se salta a la siguiente línea

    printf("b) Horas de menor ocupación (menor número de personas = %d): ", min_personas); // Se imprime etiqueta para horas de mínima ocupación
    for (h = horaIni; h <= horaFin; h++) { // Se recorren todas las horas para hallar las de menor ocupación
        if (personas_en_hora[h] == min_personas) { // Se verifica si esta hora coincide con el mínimo
            printf("%d ", h); // Se imprime la hora encontrada
        }
    }
    printf("\n"); // Se imprime salto de línea para formato

    printf("\nEstadísticas de solicitudes:\n"); // Se imprime encabezado para estadísticas de solicitudes
    printf("c) Cantidad de solicitudes negadas (total): %d\n", cnt_negadas_total); // Se imprime cantidad total de solicitudes negadas
    printf("d) Cantidad de solicitudes aceptadas en la hora solicitada: %d\n", cnt_aceptadas_hora); // Se imprimen reservas aceptadas en la hora pedida
    printf("e) Cantidad de solicitudes reprogramadas: %d\n", cnt_reprogramadas); // Se imprimen solicitudes aceptadas pero movidas de hora

    printf("\nDetalle de negaciones:\n"); // Se imprime encabezado de detalle de negaciones
    printf("   - Negadas por extemporáneas sin reprogramación: %d\n", cnt_negadas_extemp); // Se imprime cantidad de negaciones por solicitud atrasada
    printf("   - Negadas por falta de cupo: %d\n", cnt_negadas_sin_cupo); // Se imprime negaciones por no encontrar bloque disponible
    printf("   - Negadas por hora fuera de rango: %d\n", cnt_negadas_fuera_rango); // Se imprimen negaciones por solicitar horas inválidas
    printf("   - Negadas porque exceden aforo máximo: %d\n", cnt_negadas_aforo); // Se imprimen negaciones por superar aforo permitido
    printf("===================================================\n"); // Se imprime cierre del reporte
}

int registrarAgente(const MensajeSolicitud *msg) {
    int idx = buscarIndiceAgente(msg->nombre_agente); // Se busca si el agente ya está registrado previamente
    if (idx >= 0) { // Se verifica si el agente ya existe en la lista
        strncpy(agentes[idx].pipe_respuesta, msg->pipe_respuesta, MAX_PIPE_NAME - 1); // Se actualiza el nombre del pipe de respuesta del agente
        agentes[idx].pipe_respuesta[MAX_PIPE_NAME - 1] = '\0'; // Se asegura la terminación nula de la cadena
        return idx; // Se retorna el índice del agente existente
    }

    for (int i = 0; i < MAX_AGENTES; i++) { // Se recorre la lista de agentes para buscar un espacio libre
        if (!agentes[i].activo) { // Se verifica si este espacio corresponde a un agente inactivo (disponible)
            agentes[i].activo = 1; // Se marca este espacio como ocupado, indicando que el agente está activo

            strncpy(agentes[i].nombre_agente, msg->nombre_agente, MAX_NOMBRE - 1); // Se copia el nombre del agente recibido en el mensaje
            agentes[i].nombre_agente[MAX_NOMBRE - 1] = '\0'; // Se garantiza que la cadena termine adecuadamente

            strncpy(agentes[i].pipe_respuesta, msg->pipe_respuesta, MAX_PIPE_NAME - 1); // Se copia el nombre del pipe de respuesta del agente
            agentes[i].pipe_respuesta[MAX_PIPE_NAME - 1] = '\0'; // Se asegura la terminación nula

            agentes[i].fd_respuesta = -1; // Se inicializa el descriptor del pipe de respuesta como no abierto
            return i; // Se retorna el índice donde el nuevo agente fue registrado
        }
    }

    fprintf(stderr, "No hay espacio para más agentes registrados.\n"); // Se imprime error si no hay más espacio para registrar nuevos agentes
    return -1; // Se retorna -1 indicando que no fue posible registrar un nuevo agente
}

int buscarIndiceAgente(const char *nombre_agente) {
    for (int i = 0; i < MAX_AGENTES; i++) { // Se recorre la lista completa de agentes registrados
        if (agentes[i].activo && // Se verifica si el agente en esta posición está activo
            strcmp(agentes[i].nombre_agente, nombre_agente) == 0) { // Se compara el nombre buscado con el nombre almacenado
            return i; // Se retorna el índice del agente encontrado
        }
    }
    return -1; // Se retorna -1 si no se encontró ningún agente con ese nombre
}

int enviarRespuestaAAgente(const char *nombre_agente, const MensajeRespuesta *resp) {
    int idx = buscarIndiceAgente(nombre_agente); // Se busca el índice del agente usando su nombre
    if (idx < 0) { // Se verifica si el agente no fue encontrado
        fprintf(stderr, "No se encontró el agente '%s' para enviar respuesta.\n", nombre_agente); // Se imprime mensaje de error indicando agente inexistente
        return -1; // Se retorna -1 indicando falla
    }

    AgenteInfo *ag = &agentes[idx]; // Se obtiene un apuntador a la estructura del agente encontrado

    if (ag->fd_respuesta == -1) { // Se verifica si aún no se ha abierto el pipe de respuesta del agente
        ag->fd_respuesta = open(ag->pipe_respuesta, O_WRONLY); // Se abre el FIFO del agente en modo solo escritura
        if (ag->fd_respuesta == -1) { // Se valida si hubo error al abrir el pipe
            perror("open pipe_respuesta agente"); // Se imprime el error generado por el sistema
            return -1; // Se retorna -1 indicando que no se pudo abrir el pipe
        }
    }

    int escritos = write(ag->fd_respuesta, resp, sizeof(MensajeRespuesta)); // Se escribe la respuesta completa en el pipe del agente
    if (escritos != (int)sizeof(MensajeRespuesta)) { // Se verifica si no se escribieron todos los bytes esperados
        fprintf(stderr,
                "Advertencia: se escribieron %d bytes en el pipe de respuesta del agente '%s'\n",
                escritos, nombre_agente); // Se advierte que la escritura fue incompleta
        return -1; // Se retorna -1 indicando error en la comunicación
    }

    return 0; // Se retorna 0 indicando que la respuesta fue enviada correctamente
}

int buscarBloqueDosHoras(int hora_inicio_busqueda, int personas, int *hora_encontrada) {
    int h_ini = hora_inicio_busqueda; // Se inicializa la hora inicial de búsqueda con la hora solicitada

    if (h_ini < horaIni) { // Se verifica si la hora inicial cae antes del inicio de la simulación
        h_ini = horaIni; // Se ajusta la hora de búsqueda al límite permitido
    }

    for (int h = h_ini; h <= horaFin - 1; h++) { // Se recorren todas las horas posibles hasta una antes de la horaFin
        if (personas_en_hora[h] + personas <= aforoMaximo && // Se verifica que en la primera hora del bloque haya cupo
            personas_en_hora[h + 1] + personas <= aforoMaximo) { // Se verifica que en la segunda hora también haya cupo
            *hora_encontrada = h; // Se guarda la hora en la que inicia el bloque válido
            return 1; // Se retorna 1 indicando que sí se encontró un bloque disponible
        }
    }

    return 0; // Se retorna 0 indicando que no hay bloques de dos horas con el espacio requerido
}

void agregarReserva(const char *familia, int hora_inicio, int personas) {
    if (hora_inicio < 0 || hora_inicio > HORA_MAX - 1) { // Se verifica que la hora de inicio esté dentro del rango permitido
        return; // Se retorna sin hacer nada si la hora es inválida
    }

    int hora_fin = hora_inicio + 2; // Se calcula la hora fin de la reserva sumando 2 horas

    personas_en_hora[hora_inicio] += personas; // Se incrementa la cantidad de personas en la hora de inicio
    if (hora_inicio + 1 <= HORA_MAX) { // Se verifica que la hora siguiente esté dentro del rango válido
        personas_en_hora[hora_inicio + 1] += personas; // Se incrementa la cantidad de personas en la segunda hora del bloque
    }

    if (num_reservas < MAX_RESERVAS) { // Se verifica que aún haya espacio en el arreglo de reservas
        strncpy(reservas[num_reservas].familia, familia, MAX_FAMILIA - 1); // Se copia el nombre de la familia a la estructura
        reservas[num_reservas].familia[MAX_FAMILIA - 1] = '\0'; // Se asegura el fin de cadena
        reservas[num_reservas].hora_inicio = hora_inicio; // Se guarda la hora de inicio de la reserva
        reservas[num_reservas].hora_fin    = hora_fin; // Se guarda la hora de fin de la reserva
        reservas[num_reservas].personas    = personas; // Se guarda la cantidad de personas
        num_reservas++; // Se incrementa el contador de reservas almacenadas
    } else {
        fprintf(stderr, "Advertencia: se alcanzó el máximo de reservas almacenadas.\n"); // Se informa que no hay espacio para más reservas
    }
}

void errorFatal(const char *msg) {
    perror(msg); // Se imprime el mensaje de error junto con la descripción del errno del sistema
    exit(EXIT_FAILURE); // Se finaliza inmediatamente la ejecución del programa indicando fallo
}


