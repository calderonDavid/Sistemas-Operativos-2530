# -----------------------------------------------------------
# Pontificia Universidad Javeriana
# Proyecto: Sistema Simulador de Reservas de Parque 
# Autores: David Santiago Calderon Idarraga,
#          Alejandro Lopez,
#          Santiago Bautista
# Fecha: 16/10/2025
# Materia: Sistemas Operativos
# Tema: Proyecto
#
# Descripción:
#   Este Makefile automatiza la compilación de los programas
#   del proyecto: controlador.c y agente.c, los cuales forman
#   parte del sistema simulador de reservas de parque.
#   Además, incluye una regla de limpieza para eliminar los
#   ejecutables generados.
# -----------------------------------------------------------


# Compilador a utilizar
GCC = gcc

# Flags del compilador:
#   -Wall   : habilita todas las advertencias básicas
#   -Wextra : habilita advertencias adicionales
#   -pthread: permite compilar programas con hilos POSIX
CFLAGS = -Wall -Wextra -pthread

# Lista de ejecutables a generar
PROGRAMAS = controlador agente

# ---------------------------------------------------------
#                Regla General ALL
# ---------------------------------------------------------
# Compila todos los ejecutables del proyecto

all: $(PROGRAMAS)


# ---------------------------------------------------------
#                Reglas de Compilación
# ---------------------------------------------------------

# Regla para compilar el programa 'controlador'
controlador: controlador.c
	$(GCC) $(CFLAGS) controlador.c -o controlador

# Regla para compilar el programa 'agente'
agente: Agente/agente.c agenteMod.c agente.h
	$(GCC) $(CFLAGS) Agente/agente.c Agente/agenteMod.c -o agente


# ---------------------------------------------------------
#               Limpieza de ejectuables
# ---------------------------------------------------------
# Elimina los ejecutables generados y muestra mensaje confirmando.
clean:
	rm -f $(PROGRAMAS)
