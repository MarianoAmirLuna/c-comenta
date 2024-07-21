#include "../include/inicializar_io.h"
#include "../include/i_gestor.h"

void inicializar_io(){
    iniciar_logs();
    iniciar_config_io();
    imprimir_config();
	iniciar_semaforos();
}

void iniciar_semaforos(){
	sem_init(&termino_ejecutar, 1, 0);
}

void iniciar_logs(){
	io_logger = log_create("io.log","", 1 ,LOG_LEVEL_INFO);

	if(io_logger == NULL){
		perror ("Hay un error al iniciar el log.");
		exit(EXIT_FAILURE);
	}


	io_log_debug = log_create("io.log","", 1 ,LOG_LEVEL_TRACE);

	if(io_log_debug == NULL){
		perror ("Hay un error al iniciar el log.");
		exit(EXIT_FAILURE);
	}

}

void iniciar_config_io(){
    io_config = config_create("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/entradasalida/entradasalida.config"); //esto te pide la ruta del config

	if ( io_config == NULL)	
	{
		perror("Hay un error al iniciar el config.");
		exit(EXIT_FAILURE);
	}

	IP_MEMORIA = config_get_string_value(io_config, "IP_MEMORIA");
	TIPO_INTERFAZ = config_get_string_value(io_config, "TIPO_INTERFAZ");
	PUERTO_MEMORIA = config_get_string_value(io_config, "PUERTO_MEMORIA");
	IP_KERNEL = config_get_string_value(io_config, "IP_KERNEL");
	PUERTO_KERNEL = config_get_string_value(io_config, "PUERTO_KERNEL");
	TIEMPO_UNIDAD_TRABAJO = config_get_int_value(io_config, "TIEMPO_UNIDAD_TRABAJO");
	PATH_BASE_DIALFS = config_get_string_value(io_config, "PATH_BASE_DIALFS");
	BLOCK_SIZE = config_get_int_value(io_config, "BLOCK_SIZE");
	BLOCK_COUNT = config_get_int_value(io_config, "BLOCK_COUNT");
	RETRASO_COMPACTACION = config_get_int_value(io_config, "RETRASO_COMPACTACION");

}

void imprimir_config(){
	log_debug(io_log_debug, "IP_MEMORIA: %s", IP_MEMORIA);
	log_debug(io_log_debug, "PUERTO_MEMORIA: %s", PUERTO_MEMORIA);
	log_debug(io_log_debug, "TIEMPO_UNIDAD_TRABAJO: %d", TIEMPO_UNIDAD_TRABAJO);
	log_debug(io_log_debug, "BLOCK_COUNT: %d", BLOCK_COUNT);
}