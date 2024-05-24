#include "../include/inicializar_kernel.h"

#include <stdlib.h>
#include <stdio.h>
#include "utils/shared.h"
#include <commons/log.h>
#include <commons/config.h>


void inicializar_kernel(){
	inicializar_logs();
	inicializar_configs();
    imprimir_configs();
	iniciar_planificacion();
}

void inicializar_logs(){
	kernel_logger = log_create("cliente.log", "CL_LOG", 1, LOG_LEVEL_INFO);

	if (kernel_logger == NULL)
	{
		perror("Hay un error al iniciar el log.");
		exit(EXIT_FAILURE);
	}

	kernel_log_debug = log_create("cliente.log", "CL_LOG", 1, LOG_LEVEL_TRACE);

	if (kernel_log_debug == NULL)
	{
		perror("Hay un error al iniciar el log.");
		exit(EXIT_FAILURE);
	}
}

void inicializar_configs(){
	kernel_config = config_create("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/kernel/kernel.config"); 

	if (kernel_config == NULL)
	{
		perror("Hay un error al iniciar el config.");
		exit(EXIT_FAILURE);
	}



	PUERTO_ESCUCHA = config_get_string_value(kernel_config,"PUERTO_ESCUCHA");
	IP_MEMORIA = config_get_string_value(kernel_config,"IP_MEMORIA");
	PUERTO_MEMORIA = config_get_string_value(kernel_config,"PUERTO_MEMORIA");
	IP_CPU = config_get_string_value(kernel_config,"IP_CPU");
	PUERTO_CPU_DISPATCH = config_get_string_value(kernel_config,"PUERTO_CPU_DISPATCH");
	PUERTO_CPU_INTERRUPT = config_get_string_value(kernel_config,"PUERTO_CPU_INTERRUPT");
	ALGORITMO_PLANIFICACION = config_get_string_value(kernel_config,"ALGORITMO_PLANIFICACION");
	QUANTUM = config_get_int_value(kernel_config,"QUANTUM");
	RECURSOS = config_get_string_value(kernel_config,"RECURSOS");
	INSTANCIAS_RECURSOS = config_get_string_value(kernel_config,"INSTANCIAS_RECURSOS");
	GRADO_MULTIPROGRAMACION = config_get_int_value(kernel_config,"GRADO_MULTIPROGRAMACION");
}

void imprimir_configs(){
	log_info(kernel_logger, "PUERTO_ESCUCHA: %s", PUERTO_ESCUCHA);
	log_warning(kernel_logger, "PUERTO_CPU_DISPATCH: %s", PUERTO_CPU_DISPATCH);
	log_debug(kernel_log_debug, "RECURSOS: %s", RECURSOS);
	log_trace(kernel_log_debug, "QUANTUM: %d", QUANTUM);
}
