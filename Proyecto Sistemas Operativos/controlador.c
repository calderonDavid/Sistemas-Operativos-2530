/********************
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
*  
*******************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

/* Constantes de simulación */
#define HORA_MIN 7
#define HORA_MAX 19
#define MAX_HORAS (HORA_MAX + 1)

#define MAX_NOMBRE    64
#define MAX_FAMILIA   64
#define MAX_PIPE_NAME 128

#define MAX_AGENTES   32
#define MAX_RESERVAS  1024

/* Códigos de respuesta */
#define RESP_OK                0
#define RESP_REPROGRAMADA      1
#define RESP_NEG_EXTEMPORANEA  2
#define RESP_NEG_SIN_CUPO      3
#define RESP_NEG_FUERA_RANGO   4
#define RESP_NEG_AFORO_EXCEDE  5

/* Estructura para representar una reserva */
typedef struct {
    char familia[MAX_FAMILIA];
    int hora_inicio;   /* hora de entrada */
    int hora_fin;      /* hora de salida (excluyente, hora_inicio + 2) */
    int personas;
} Reserva;

/* Mensaje que envía un agente al controlador */
typedef struct {
    int tipo;   /* 0: registro agente, 1: solicitud reserva */
    char nombre_agente[MAX_NOMBRE];

    /* Para registro */
    char pipe_respuesta[MAX_PIPE_NAME];

    /* Para solicitud de reserva */
    char familia[MAX_FAMILIA];
    int hora_solicitada;
    int n_personas;
} MensajeSolicitud;

/* Mensaje que responde el controlador al agente */
typedef struct {
    int codigo_respuesta;  /* RESP_* */
    int hora_asignada;     /* hora de inicio asignada (si aplica) */
    int hora_asignada_fin; /* hora de fin (si aplica) */
    char mensaje[128];     /* texto descriptivo */
} MensajeRespuesta;

/* Información de cada agente registrado */
typedef struct {
    int activo;
    char nombre_agente[MAX_NOMBRE];
    char pipe_respuesta[MAX_PIPE_NAME];
    int fd_respuesta;  /* descriptor del FIFO de respuesta, -1 si no abierto */
} AgenteInfo;

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

/* Prototipos */
void parsearArgumentos(int argc, char *argv[]);
void validarParametros(void);
void inicializarEstado(void);
void crearYPonerPipeRecibe(void);

void *hiloReloj(void *arg);
void *hiloAtencionSolicitudes(void *arg);

void procesarMensaje(const MensajeSolicitud *msg);
void avanzarHoraSimulacion(void);
void imprimirEstadoHora(void);
void generarReporteFinal(void);

int  registrarAgente(const MensajeSolicitud *msg);
int  buscarIndiceAgente(const char *nombre_agente);
int  enviarRespuestaAAgente(const char *nombre_agente, const MensajeRespuesta *resp);

int  buscarBloqueDosHoras(int hora_inicio_busqueda, int personas, int *hora_encontrada);
void agregarReserva(const char *familia,int hora_inicio, int personas);
static void errorFatal(const char *msg);

/********   main   *********/

int main(int argc, char *argv[]) {
    pthread_t thReloj;
    pthread_t thSolicitudes;
    int rc;

    parsearArgumentos(argc, argv);
    validarParametros();
    inicializarEstado();
    crearYPonerPipeRecibe();

    rc = pthread_create(&thReloj, NULL, hiloReloj, NULL);
    if (rc != 0) {
        errorFatal("Error al crear hilo del reloj");
    }

    rc = pthread_create(&thSolicitudes, NULL, hiloAtencionSolicitudes, NULL);
    if (rc != 0) {
        errorFatal("Error al crear hilo de atención de solicitudes");
    }

    pthread_join(thReloj, NULL);
    pthread_join(thSolicitudes, NULL);

    /* 6. Generar reporte final del día */
    generarReporteFinal();

    /* 7. Cerrar y eliminar el pipe nominal (si sigue abierto) */
    if (fd_pipe_recibe != -1) {
        close(fd_pipe_recibe);
    }
    unlink(nombrePipeRecibe);

    for (int i = 0; i < MAX_AGENTES; i++) {
        if (agentes[i].activo && agentes[i].fd_respuesta != -1) {
            close(agentes[i].fd_respuesta);
        }
    }

    return 0;
}

/******* Implementaciones ********/

void parsearArgumentos(int argc, char *argv[]) {
    int i = 1;

    if (argc < 11) {
        fprintf(stderr,
            "Uso: %s -i horaIni -f horaFin -s segHoras -t aforoMaximo -p pipeRecibe\n",
            argv[0]);
        exit(EXIT_FAILURE);
    }

    while (i < argc) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            horaIni = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            horaFin = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            segHoras = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            aforoMaximo = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            strncpy(nombrePipeRecibe, argv[++i], MAX_PIPE_NAME - 1);
            nombrePipeRecibe[MAX_PIPE_NAME - 1] = '\0';
        } else {
            fprintf(stderr, "Parámetro no reconocido o faltan argumentos: %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
        i++;
    }
}

void validarParametros(void) {
    if (horaIni < HORA_MIN || horaIni > HORA_MAX) {
        fprintf(stderr, "Error: horaIni (%d) fuera del rango [%d, %d]\n",
                horaIni, HORA_MIN, HORA_MAX);
        exit(EXIT_FAILURE);
    }
    if (horaFin < HORA_MIN || horaFin > HORA_MAX || horaFin <= horaIni) {
        fprintf(stderr, "Error: horaFin (%d) debe estar en [%d, %d] y ser > horaIni (%d)\n",
                horaFin, HORA_MIN, HORA_MAX, horaIni);
        exit(EXIT_FAILURE);
    }
    if (segHoras <= 0) {
        fprintf(stderr, "Error: segHoras debe ser mayor que 0\n");
        exit(EXIT_FAILURE);
    }
    if (aforoMaximo <= 0) {
        fprintf(stderr, "Error: aforo máximo debe ser mayor que 0\n");
        exit(EXIT_FAILURE);
    }
    if (strlen(nombrePipeRecibe) == 0) {
        fprintf(stderr, "Error: debe especificar un nombre de pipe (-p)\n");
        exit(EXIT_FAILURE);
    }
}

void inicializarEstado(void) {
    horaActual = horaIni;
    terminarSimulacion = 0;

    for (int h = 0; h < MAX_HORAS; h++) {
        personas_en_hora[h] = 0;
    }
    num_reservas = 0;

    for (int i = 0; i < MAX_AGENTES; i++) {
        agentes[i].activo = 0;
        agentes[i].fd_respuesta = -1;
        agentes[i].nombre_agente[0] = '\0';
        agentes[i].pipe_respuesta[0] = '\0';
    }

    cnt_aceptadas_hora      = 0;
    cnt_reprogramadas       = 0;
    cnt_negadas_total       = 0;
    cnt_negadas_extemp      = 0;
    cnt_negadas_sin_cupo    = 0;
    cnt_negadas_fuera_rango = 0;
    cnt_negadas_aforo       = 0;
}

void crearYPonerPipeRecibe(void) {
    int rc = mkfifo(nombrePipeRecibe, 0666);
    if (rc == -1) {
        if (errno != EEXIST) {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    }

    /* CAMBIO MINIMO: O_RDONLY → O_RDONLY | O_NONBLOCK para no bloquear el hilo */
    fd_pipe_recibe = open(nombrePipeRecibe, O_RDONLY | O_NONBLOCK);
    if (fd_pipe_recibe == -1) {
        perror("open pipeRecibe");
        exit(EXIT_FAILURE);
    }

    printf("Controlador: pipe de recepción '%s' creado y abierto.\n", nombrePipeRecibe);
    printf("Simulación desde hora %d hasta %d, %d seg/hora, aforo máximo %d personas.\n",
           horaIni, horaFin, segHoras, aforoMaximo);
}

/******* Hilo del reloj ********/

void *hiloReloj(void *arg) {
    (void)arg;

    while (1) {
        sleep(segHoras);

        pthread_mutex_lock(&mutex_estado);

        /* Si ya llegamos o superamos la hora final, marcamos fin de simulación */
        if (horaActual >= horaFin) {
            terminarSimulacion = 1;
            pthread_mutex_unlock(&mutex_estado);

            /* OJO: ya NO cerramos aquí el pipe, eso se hace al final en main */
            break;
        }

        /* Avanzar una hora de simulación e imprimir estado */
        avanzarHoraSimulacion();
        imprimirEstadoHora();

        pthread_mutex_unlock(&mutex_estado);
    }

    printf("Controlador: hiloReloj termina. Día de simulación finalizado.\n");
    return NULL;
}

/***** Hilo de atención de solicitudes (AJUSTADO SOLO PARA EAGAIN) *****/

void *hiloAtencionSolicitudes(void *arg) {
    (void)arg;
    ssize_t leidos;
    MensajeSolicitud msg;

    while (1) {
        pthread_mutex_lock(&mutex_estado);
        int terminar = terminarSimulacion;
        pthread_mutex_unlock(&mutex_estado);

        if (terminar) {
            break;
        }

        leidos = read(fd_pipe_recibe, &msg, sizeof(MensajeSolicitud));
        if (leidos == -1) {
            /* Si no hay datos (non-blocking), solo esperamos un momento y seguimos */
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(100000);
                continue;
            }
            /* Error real */
            perror("read pipeRecibe");
            break;
        } else if (leidos == 0) {
            /* EOF o sin escritores: esperamos un poco y volvemos a revisar terminarSimulacion */
            usleep(100000);
            continue;
        } else if (leidos != (ssize_t)sizeof(MensajeSolicitud)) {
            fprintf(stderr, "Advertencia: tamaño de mensaje inesperado (%zd bytes)\n",
                    leidos);
            continue;
        }

        procesarMensaje(&msg);
    }

    printf("Controlador: hiloAtencionSolicitudes termina.\n");
    return NULL;
}

/******* Lógica de negocio *******/

void procesarMensaje(const MensajeSolicitud *msg) {
    if (msg->tipo == 0) {
        pthread_mutex_lock(&mutex_estado);
        int idx = registrarAgente(msg);
        int hora_actual_local = horaActual;
        pthread_mutex_unlock(&mutex_estado);

        printf("[Registro] Agente: %s, pipe respuesta: %s (idx=%d)\n",
               msg->nombre_agente, msg->pipe_respuesta, idx);

        MensajeRespuesta resp;
        resp.codigo_respuesta  = RESP_OK;
        resp.hora_asignada     = hora_actual_local;
        resp.hora_asignada_fin = hora_actual_local;
        snprintf(resp.mensaje, sizeof(resp.mensaje),
                 "Registro exitoso. Hora actual de simulación: %d",
                 hora_actual_local);

        enviarRespuestaAAgente(msg->nombre_agente, &resp);

    } else if (msg->tipo == 1) {
        MensajeRespuesta resp;
        int hora_actual_local;
        int hora_solicitada;
        int n_personas;

        pthread_mutex_lock(&mutex_estado);

        hora_actual_local = horaActual;
        hora_solicitada   = msg->hora_solicitada;
        n_personas        = msg->n_personas;

        printf("[Solicitud] Agente: %s, Familia: %s, Hora: %d, Personas: %d\n",
               msg->nombre_agente,
               msg->familia,
               hora_solicitada,
               n_personas);

        /* Validaciones */
        if (n_personas > aforoMaximo) {
            resp.codigo_respuesta  = RESP_NEG_AFORO_EXCEDE;
            resp.hora_asignada     = -1;
            resp.hora_asignada_fin = -1;
            snprintf(resp.mensaje, sizeof(resp.mensaje),
                     "Solicitud negada: número de personas (%d) excede el aforo máximo (%d).",
                     n_personas, aforoMaximo);
            cnt_negadas_total++;
            cnt_negadas_aforo++;

        } else if (hora_solicitada > horaFin) {
            resp.codigo_respuesta  = RESP_NEG_FUERA_RANGO;
            resp.hora_asignada     = -1;
            resp.hora_asignada_fin = -1;
            snprintf(resp.mensaje, sizeof(resp.mensaje),
                     "Solicitud negada: hora solicitada (%d) es mayor a la hora fin de simulación (%d).",
                     hora_solicitada, horaFin);
            cnt_negadas_total++;
            cnt_negadas_fuera_rango++;

        } else {
            int es_extemporanea = (hora_solicitada < hora_actual_local);
            int hora_asignada   = -1;
            int puede_ser_inicio = (hora_solicitada <= horaFin - 1);

            if (!es_extemporanea && puede_ser_inicio) {
                int h = hora_solicitada;
                if (personas_en_hora[h] + n_personas <= aforoMaximo &&
                    personas_en_hora[h + 1] + n_personas <= aforoMaximo) {
                    hora_asignada = h;
                }
            }

            if (hora_asignada != -1) {
                agregarReserva(msg->familia, hora_asignada, n_personas);
                resp.codigo_respuesta  = RESP_OK;
                resp.hora_asignada     = hora_asignada;
                resp.hora_asignada_fin = hora_asignada + 2;
                snprintf(resp.mensaje, sizeof(resp.mensaje),
                         "Reserva aceptada en la hora solicitada. Ingreso: %d, salida: %d.",
                         resp.hora_asignada, resp.hora_asignada_fin);
                cnt_aceptadas_hora++;

            } else {
                int inicio_busqueda = es_extemporanea ? hora_actual_local
                                                      : hora_solicitada;
                if (inicio_busqueda < horaIni) {
                    inicio_busqueda = horaIni;
                }

                int h_encontrada = -1;
                int ok_bloque = buscarBloqueDosHoras(inicio_busqueda,
                                                     n_personas,
                                                     &h_encontrada);

                if (ok_bloque) {
                    agregarReserva(msg->familia, h_encontrada, n_personas);
                    resp.codigo_respuesta  = RESP_REPROGRAMADA;
                    resp.hora_asignada     = h_encontrada;
                    resp.hora_asignada_fin = h_encontrada + 2;
                    snprintf(resp.mensaje, sizeof(resp.mensaje),
                             "Reserva reprogramada. Nueva hora de ingreso: %d, salida: %d.",
                             resp.hora_asignada, resp.hora_asignada_fin);
                    cnt_reprogramadas++;

                } else {
                    resp.hora_asignada     = -1;
                    resp.hora_asignada_fin = -1;

                    if (es_extemporanea) {
                        resp.codigo_respuesta = RESP_NEG_EXTEMPORANEA;
                        snprintf(resp.mensaje, sizeof(resp.mensaje),
                                 "Solicitud negada: la hora solicitada (%d) ya pasó y no se encontró bloque de 2 horas disponible.",
                                 hora_solicitada);
                        cnt_negadas_extemp++;
                    } else {
                        resp.codigo_respuesta = RESP_NEG_SIN_CUPO;
                        snprintf(resp.mensaje, sizeof(resp.mensaje),
                                 "Solicitud negada: no hay cupo disponible en ningún bloque de 2 horas.");
                        cnt_negadas_sin_cupo++;
                    }
                    cnt_negadas_total++;
                }
            }
        }

        pthread_mutex_unlock(&mutex_estado);

        enviarRespuestaAAgente(msg->nombre_agente, &resp);

    } else {
        printf("Mensaje con tipo desconocido: %d\n", msg->tipo);
    }
}

/* Avanza la hora */
void avanzarHoraSimulacion(void) {
    horaActual++;
}

/* Imprime estado de la hora actual */
void imprimirEstadoHora(void) {
    printf("\n[Simulación] Ha transcurrido una hora. Hora actual: %d\n",
           horaActual);

    int personas_dentro = 0;

    printf("Familias que salen en esta hora: ");
    int hay_salen = 0;
    for (int i = 0; i < num_reservas; i++) {
        if (reservas[i].hora_fin == horaActual) {
            printf("%s(%d) ", reservas[i].familia, reservas[i].personas);
            hay_salen = 1;
        }
    }
    if (!hay_salen) {
        printf("ninguna");
    }
    printf("\n");

    printf("Familias que entran en esta hora: ");
    int hay_entran = 0;
    for (int i = 0; i < num_reservas; i++) {
        if (reservas[i].hora_inicio == horaActual) {
            printf("%s(%d) ", reservas[i].familia, reservas[i].personas);
            hay_entran = 1;
        }
    }
    if (!hay_entran) {
        printf("ninguna");
    }
    printf("\n");

    for (int i = 0; i < num_reservas; i++) {
        if (reservas[i].hora_inicio <= horaActual &&
            reservas[i].hora_fin > horaActual) {
            personas_dentro += reservas[i].personas;
        }
    }

    printf("Personas en el parque en esta hora: %d\n", personas_dentro);
}

/* Reporte final */
void generarReporteFinal(void) {
    int h;
    int max_personas = -1;
    int min_personas = 1000000;

    for (h = horaIni; h <= horaFin; h++) {
        int p = personas_en_hora[h];
        if (p > max_personas) max_personas = p;
        if (p < min_personas) min_personas = p;
    }

    printf("\n========== REPORTE FINAL DEL CONTROLADOR ==========\n");
    printf("Parámetros de simulación: horaIni=%d, horaFin=%d, aforoMáximo=%d personas\n",
           horaIni, horaFin, aforoMaximo);

    printf("\na) Horas pico (mayor número de personas = %d): ", max_personas);
    for (h = horaIni; h <= horaFin; h++) {
        if (personas_en_hora[h] == max_personas) {
            printf("%d ", h);
        }
    }
    printf("\n");

    printf("b) Horas de menor ocupación (menor número de personas = %d): ", min_personas);
    for (h = horaIni; h <= horaFin; h++) {
        if (personas_en_hora[h] == min_personas) {
            printf("%d ", h);
        }
    }
    printf("\n");

    printf("\nEstadísticas de solicitudes:\n");
    printf("c) Cantidad de solicitudes negadas (total): %d\n", cnt_negadas_total);
    printf("d) Cantidad de solicitudes aceptadas en la hora solicitada: %d\n", cnt_aceptadas_hora);
    printf("e) Cantidad de solicitudes reprogramadas: %d\n", cnt_reprogramadas);

    printf("\nDetalle de negaciones:\n");
    printf("   - Negadas por extemporáneas sin reprogramación: %d\n", cnt_negadas_extemp);
    printf("   - Negadas por falta de cupo: %d\n", cnt_negadas_sin_cupo);
    printf("   - Negadas por hora fuera de rango: %d\n", cnt_negadas_fuera_rango);
    printf("   - Negadas porque exceden aforo máximo: %d\n", cnt_negadas_aforo);
    printf("===================================================\n");
}

/****** Manejo de agentes y reservas *****/

int registrarAgente(const MensajeSolicitud *msg) {
    int idx = buscarIndiceAgente(msg->nombre_agente);
    if (idx >= 0) {
        strncpy(agentes[idx].pipe_respuesta, msg->pipe_respuesta, MAX_PIPE_NAME - 1);
        agentes[idx].pipe_respuesta[MAX_PIPE_NAME - 1] = '\0';
        return idx;
    }

    for (int i = 0; i < MAX_AGENTES; i++) {
        if (!agentes[i].activo) {
            agentes[i].activo = 1;
            strncpy(agentes[i].nombre_agente, msg->nombre_agente, MAX_NOMBRE - 1);
            agentes[i].nombre_agente[MAX_NOMBRE - 1] = '\0';

            strncpy(agentes[i].pipe_respuesta, msg->pipe_respuesta, MAX_PIPE_NAME - 1);
            agentes[i].pipe_respuesta[MAX_PIPE_NAME - 1] = '\0';

            agentes[i].fd_respuesta = -1;
            return i;
        }
    }

    fprintf(stderr, "No hay espacio para más agentes registrados.\n");
    return -1;
}

int buscarIndiceAgente(const char *nombre_agente) {
    for (int i = 0; i < MAX_AGENTES; i++) {
        if (agentes[i].activo &&
            strcmp(agentes[i].nombre_agente, nombre_agente) == 0) {
            return i;
        }
    }
    return -1;
}

int enviarRespuestaAAgente(const char *nombre_agente,
                           const MensajeRespuesta *resp) {
    int idx = buscarIndiceAgente(nombre_agente);
    if (idx < 0) {
        fprintf(stderr, "No se encontró el agente '%s' para enviar respuesta.\n",
                nombre_agente);
        return -1;
    }

    AgenteInfo *ag = &agentes[idx];

    if (ag->fd_respuesta == -1) {
        ag->fd_respuesta = open(ag->pipe_respuesta, O_WRONLY);
        if (ag->fd_respuesta == -1) {
            perror("open pipe_respuesta agente");
            return -1;
        }
    }

    int escritos = write(ag->fd_respuesta, resp, sizeof(MensajeRespuesta));
    if (escritos != (int)sizeof(MensajeRespuesta)) {
        fprintf(stderr,
                "Advertencia: se escribieron %d bytes en el pipe de respuesta del agente '%s'\n",
                escritos, nombre_agente);
        return -1;
    }

    return 0;
}

int buscarBloqueDosHoras(int hora_inicio_busqueda,
                         int personas,
                         int *hora_encontrada) {
    int h_ini = hora_inicio_busqueda;
    if (h_ini < horaIni) {
        h_ini = horaIni;
    }

    for (int h = h_ini; h <= horaFin - 1; h++) {
        if (personas_en_hora[h] + personas <= aforoMaximo &&
            personas_en_hora[h + 1] + personas <= aforoMaximo) {
            *hora_encontrada = h;
            return 1;
        }
    }

    return 0;
}

void agregarReserva(const char *familia,
                    int hora_inicio,
                    int personas) {
    if (hora_inicio < 0 || hora_inicio > HORA_MAX - 1) {
        return;
    }

    int hora_fin = hora_inicio + 2;

    personas_en_hora[hora_inicio] += personas;
    if (hora_inicio + 1 <= HORA_MAX) {
        personas_en_hora[hora_inicio + 1] += personas;
    }

    if (num_reservas < MAX_RESERVAS) {
        strncpy(reservas[num_reservas].familia, familia, MAX_FAMILIA - 1);
        reservas[num_reservas].familia[MAX_FAMILIA - 1] = '\0';
        reservas[num_reservas].hora_inicio = hora_inicio;
        reservas[num_reservas].hora_fin    = hora_fin;
        reservas[num_reservas].personas    = personas;
        num_reservas++;
    } else {
        fprintf(stderr, "Advertencia: se alcanzó el máximo de reservas almacenadas.\n");
    }
}

/******* Funciones auxiliares ******/

static void errorFatal(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
