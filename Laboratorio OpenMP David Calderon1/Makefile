###################################################################
#                    Pontificia Universidad
# Autor: David Santiago Calderon Idarraga
# Fichero: Automatización Compilación
#
# Descripción:
# Este Makefile automatiza la compilación de varios programas en Cque usan OpenMP. 
###################################################################


# 'gcc' es el compilador estándar para programas en C.
GCC = gcc

# FLAGS (banderas de compilación):
#  -fopenmp : habilita el uso de la API OpenMP para paralelismo.
#  -lm      : enlaza la biblioteca matemática (libm), necesaria para funciones como sin(), cos(), etc.
FLAGS = -fopenmp -lm
MODULO =

# Lista de los programas a compilar. 
PROGRAMAS = lab01 lab02 lab03 lab04 lab05
lab01:
	$(GCC) $@.c -o $@ $(FLAGS)
lab02:
	$(GCC) $@.c -o $@ $(FLAGS)

lab03:
	$(GCC) $@.c -o $@ $(FLAGS)

lab04:
	$(GCC) $@.c -o $@ $(FLAGS)

lab05:
	$(GCC) $@.c -o $@ $(FLAGS)

# Regla para limpiar los ejecutables generados.
clean:
	$(RM) $(PROGRAMAS)

