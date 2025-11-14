#!/usr/bin/perl
#**************************************************************
#         		Pontificia Universidad Javeriana
#     Autor: David Calderon y Alejandro Lopez
#     Fecha: 03-Nov-2025
#     Materia: Sistemas Operativos
#     Tema: Taller de Evaluación de Rendimiento
#     Fichero: script automatización (versión CSV único)
#****************************************************************/

use strict;
use warnings;
use File::Path qw(make_path);

my $Path = `pwd`;
chomp($Path);

# 1. Definir el nombre del archivo de salida ÚNICO
my $output_file = "$Path/resultados.csv";

# 2. Definir los ejecutables
my @Ejecutables = ("mmClasicaPosix", "mmClasicaFork", "mmClasicaOpenMP", "mmFilasOpenMP");

# 3. Definir los tamaños de matriz
my @Size_Matriz = ("100", "500", "1000", "1500");

# 4. Definir el número de hilos
my @Num_Hilos = (1, 2, 4, 8, 12, 16);

# 5. Definir el número de repeticiones
my $Repeticiones = 30; 

# Crear (o sobrescribir) el archivo de salida
open(my $fh, '>', $output_file) or die "No se pudo abrir el archivo '$output_file' $!";

# Escribir la cabecera del CSV
print $fh "Algoritmo,Tamano_Matriz,Num_Hilos,Repeticion,Tiempo_usec\n";

print "Iniciando batería de pruebas. Los resultados se guardarán en: $output_file\n";

# --- Bucles de Experimentación ---
foreach my $exe (@Ejecutables) {
    foreach my $size (@Size_Matriz) {
        foreach my $hilo (@Num_Hilos) {
            
            print "Ejecutando: $exe (Tamaño: $size, Hilos: $hilo)...\n";

            # Bucle de 30 repeticiones
            for (my $i=1; $i <= $Repeticiones; $i++) {
                
                # Construye el comando
                my $comando = "$Path/$exe $size $hilo";
                
                # Ejecuta y captura el tiempo
                my $tiempo = `$comando`;
                chomp($tiempo);
                $tiempo =~ s/^\s+|\s+$//g;
                
                # Escribe la línea de datos en el CSV
                # (ej: mmClasicaPosix,1000,8,1,123456)
                print $fh "$exe,$size,$hilo,$i,$tiempo\n";
            }
        }
    }
}

# Cerrar el archivo CSV
close($fh);

print "\n¡Finalizado! Todos los resultados están en $output_file\n";
