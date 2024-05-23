#include "../include/inicializar_cpu.h"
#include <utils/shared.h>

void inicializar_cpu(){
    iniciar_logs();
    iniciar_config_cpu();
    imprimir_config();
	iniciar_semaforos();
}

void iniciar_logs(){
	cpu_logger = log_create("cliente.log","", 1 ,LOG_LEVEL_INFO);

	if(cpu_logger == NULL){
		perror ("Hay un error al iniciar el log.");
		exit(EXIT_FAILURE);
	}


	cpu_log_debug = log_create("cliente.log","", 1 ,LOG_LEVEL_TRACE);

	if(cpu_log_debug == NULL){
		perror ("Hay un error al iniciar el log.");
		exit(EXIT_FAILURE);
	}
}
void iniciar_config_cpu(){
    cpu_config = config_create("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/cpu/cpu.config"); //esto te pide la ruta del config

	if ( cpu_config == NULL)	
	{
		perror("Hay un error al iniciar el config.");
		exit(EXIT_FAILURE);
	}

	IP_MEMORIA = config_get_string_value(cpu_config, "IP_MEMORIA");
	PUERTO_MEMORIA = config_get_string_value(cpu_config, "PUERTO_MEMORIA");
	PUERTO_ESCUCHA_DISPATCH = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_DISPATCH");
	PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_INTERRUPT");

}
void imprimir_config(){
    log_info(cpu_logger, "IP_MEMORIA: %s", IP_MEMORIA);
	log_warning(cpu_log_debug, "PUERTO_MEMORIA: %s", PUERTO_MEMORIA);
	log_debug(cpu_log_debug, "PUERTO_ESCUCHA_DISPATCH: %s", PUERTO_ESCUCHA_DISPATCH);
	log_trace(cpu_log_debug, "PUERTO_ESCUCHA_INTERRUPT: %s", PUERTO_ESCUCHA_INTERRUPT);

}

void iniciar_semaforos(){
	printf("se creo el semaforo");
	sem_init(&arrancar_cpu, 1, 0);
}

