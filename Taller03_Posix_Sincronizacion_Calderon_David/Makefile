#############################################################
# Pontificia Universidad Javeriana
# Autores: David Calderon Idarraga - Alejandro Lopez Beltran
# Fecha: 13/11/2025
# Materia: Sistemas Operativos
# Tema: Makefile para compilación de Productor, Consumidor y posixSincro
# Descripción:
# - Automatiza la compilación de los programas:
#       * producer.c  (memoria compartida + semáforos POSIX)
#       * consumer.c  (memoria compartida + semáforos POSIX)
#       * posixSincro.c (sincronización con hilos pthread)
# - Incluye reglas para compilar, enlazar y limpiar ejecutables.
#############################################################

# Opciones del compilador
CC = gcc
# Flags para C, -g para debug, -Wall para warnings
CFLAGS = -g -Wall
# Flags para enlazar (linking)
# -lrt es para la librería Real Time (necesaria para shm_open y sem_open)
# -pthread es para la librería Pthreads
LDFLAGS_POSIX = -lrt
LDFLAGS_PTHREAD = -pthread

# Define los nombres de los ejecutables
TARGETS = producer consumer posixSincro

# Regla por defecto: compila todo
all: $(TARGETS)

# Regla para compilar producer
producer: producer.c
	$(CC) $(CFLAGS) -o producer producer.c $(LDFLAGS_POSIX)

# Regla para compilar consumer
consumer: consumer.c
	$(CC) $(CFLAGS) -o consumer consumer.c $(LDFLAGS_POSIX)

# Regla para compilar posixSincro
posixSincro: posixSincro.c
	$(CC) $(CFLAGS) -o posixSincro posixSincro.c $(LDFLAGS_PTHREAD)

# Regla para limpiar los ejecutables generados
clean:
	rm -f $(TARGETS)