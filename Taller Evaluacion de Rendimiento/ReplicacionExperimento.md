Replicacion de Experimento EVALUACION DE RENDIMIENTO

Esta guía está diseñada para ejecutarse en sistemas operativos basados en Linux, tales como:
  - Ubuntu
  - Debian
  - Linux Mint
  - Kali Linux
  - Pop!_OS

Para la ejecucion del Taller es necesario tener instalado el compilador de C.

**Pasos para la instalación de Compilador C**
1. Abrir Terminal del sistema operativo
2. Ejectura comando: sudo apt update
3. Instalacion: sudo apt install gcc -y
4. Verificacion de Instalacion: gcc --version

**Pasos de Ejecucion**
1. Ubicarse en la carpeta donde estan todos los archivos: cd /TallerEvaluaciondeRendimiento
2. Ejecutar comando: make clean (Asegura que no haya ejectuables o basura en el compilador)
3. Ejecutar comando: make All (Compilacion de todos los metodos de Paralelismo y Concurrencia)
4. Ejecutar el lanzador automatizado: ./lanzador.pl
