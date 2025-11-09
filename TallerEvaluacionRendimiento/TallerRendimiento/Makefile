#**************************************************************
#         		Pontificia Universidad Javeriana
#     Autor: David Calderon y Alejandro Lopez
#     Fecha: 03-Nov-2025
#     Materia: Sistemas Operativos
#     Tema: Taller de Evaluación de Rendimiento
#     Fichero: Makefile (CORREGIDO para nombres de módulo)
#****************************************************************/

GCC = gcc
CFLAGS = -lm -Wall
FOPENMP = -fopenmp -O3
POSIX = -lpthread

# Los PROGRAMAS (ejecutables) mantienen sus nombres originales
PROGRAMAS = mmClasicaFork mmClasicaPosix mmClasicaOpenMP mmFilasOpenMP

All: $(PROGRAMAS)

# Regla para mmClasicaFork (busca 'moduloClasicaFork.c')
mmClasicaFork: ClasicaFork/mmClasicaFork.c ClasicaFork/moduloClasicaFork.c ClasicaFork/mmClasicaFork.h
	$(GCC) $(CFLAGS) -IClasicaFork ClasicaFork/mmClasicaFork.c ClasicaFork/moduloClasicaFork.c -o $@

# Regla para mmClasicaPosix (busca 'moduloClasicaPosix.c')
mmClasicaPosix: ClasicaPosix/mmClasicaPosix.c ClasicaPosix/moduloClasicaPosix.c ClasicaPosix/mmClasicaPosix.h
	$(GCC) $(CFLAGS) $(POSIX) -IClasicaPosix ClasicaPosix/mmClasicaPosix.c ClasicaPosix/moduloClasicaPosix.c -o $@

# Regla para mmClasicaOpenMP (busca 'moduloClasicaOpenMP.c')
mmClasicaOpenMP: ClasicaOpenMP/mmClasicaOpenMP.c ClasicaOpenMP/moduloClasicaOpenMP.c ClasicaOpenMP/mmClasicaOpenMP.h
	$(GCC) $(CFLAGS) $(FOPENMP) -IClasicaOpenMP ClasicaOpenMP/mmClasicaOpenMP.c ClasicaOpenMP/moduloClasicaOpenMP.c -o $@

# Regla para mmFilasOpenMP (busca 'moduloFilasOpenMP.c')
mmFilasOpenMP: FilasOpenMP/mmFilasOpenMP.c FilasOpenMP/moduloFilasOpenMP.c FilasOpenMP/mmFilasOpenMP.h
	$(GCC) $(CFLAGS) $(FOPENMP) -IFilasOpenMP FilasOpenMP/mmFilasOpenMP.c FilasOpenMP/moduloFilasOpenMP.c -o $@

# Regla para limpiar los ejecutables
clean:
	$(RM) -f $(PROGRAMAS)
