Sistema Simulador de Reservas de Parque

Sistemas Operativos – Pontificia Universidad Javeriana Autores: Santiago Bautista Velásquez, Alejandro López, David Calderón Fecha: Noviembre 2025

Este proyecto implementa un sistema distribuido de reservas para un parque recreativo. Consta de dos procesos principales: un Controlador (servidor) y múltiples Agentes (clientes). La comunicación entre procesos se realiza mediante pipes FIFO. El sistema permite registrar solicitudes de reserva, evaluar disponibilidad de aforo, asignar bloques de dos horas, reprogramar solicitudes y generar un reporte final del día.

El agente lee un archivo CSV con solicitudes y las envía al controlador. Cada agente recibe respuestas a través de un pipe FIFO exclusivo. El controlador procesa solicitudes concurrentes y consolida los resultados en un informe completo.

Estructura del Proyecto

Compilación

El proyecto contiene un Makefile que compila automáticamente los módulos de Agente y Controlador.

Compilar todo: make all

Compilar solo el controlador: make controlador

Compilar solo el agente: make agente

Limpiar ejecutables: make clean

Ejecución

Ejecutar el Controlador: ./controlador -i 7 -f 19 -s 5 -t 50 -p pipeGeneral

Parámetros: -i hora inicial -f hora final -s duración de cada hora simulada en segundos -t aforo máximo permitido -p nombre del pipe FIFO principal

Ejecutar un Agente: ./agente -s Juan -a solicitudes_Juan.csv -p pipeGeneral

Parámetros: -s nombre del agente -a archivo CSV con solicitudes -p pipe de recepción del controlador

Cada agente recibe sus respuestas en un pipe nombrado automáticamente como: pipe_resp_

Pruebas Implementadas

El proyecto incluye siete pruebas diseñadas para validar escenarios funcionales, excepciones y concurrencia:

Prueba 0: Funcionamiento básico. Valida la correcta gestión del aforo y procesamiento simple de solicitudes. Prueba 1: Reprogramación por falta de cupo. Evalúa la asignación de bloques alternativos. Prueba 2: Solicitudes extemporáneas. Comprueba reprogramaciones y negaciones por horas pasadas. Prueba 3: Hora fuera de rango. Valida negaciones cuando la hora excede el horario del parque. Prueba 4: Exceso de aforo por familia. Verifica rechazo cuando una familia excede el aforo máximo. Prueba 5: Parque sin cupo. Revisa negaciones cuando no hay bloques disponibles en todo el día. Prueba 6: Múltiples agentes concurrentes. Valida funcionamiento simultáneo de varios procesos cliente. Prueba 7: Validación del reporte final. Comprueba que se generen correctamente los cinco elementos exigidos en el resumen.

Los archivos CSV correspondientes están en la carpeta Pruebas/.

Documento del Proyecto

Incluye:

Introducción

Diseño del sistema

Arquitectura y módulos

Plan de pruebas

Evidencias y análisis

Conclusiones

Disponible como: Documento_Proyecto.pdf

Tecnologías Utilizadas

Lenguaje C POSIX Threads (pthread) Pipes FIFO Linux Makefile Git y GitHub

Autor

Santiago Bautista Velásquez

Pontificia Universidad Javeriana – Ingeniería de Sistemas
