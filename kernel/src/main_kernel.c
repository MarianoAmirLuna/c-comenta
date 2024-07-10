#include "../include/kernel.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <pthread.h>
#include "utils/shared.h"
#include <commons/log.h>
#include "../include/funciones_kernel.h"
#include "../include/consola.h"

int main(void)
{
	//recibe mensajes del interrupt / dispatch / io / memoria
	
	inicializar_kernel();
	fd_kernel = iniciar_servidor(PUERTO_ESCUCHA);

    fd_memoria = iniciar_conexion(PUERTO_MEMORIA, "MEMORIA",kernel_log_debug);

	fd_cpu_interrupt = iniciar_conexion(PUERTO_CPU_INTERRUPT, "CPU-INTERRUPT",kernel_log_debug);
	fd_cpu_dispatch = iniciar_conexion(PUERTO_CPU_DISPATCH, "CPU-DISPATCH",kernel_log_debug);

	pthread_t hilo_planificacion;
	//printf("llega aca\n");
	pthread_create(&hilo_planificacion, NULL, (void*)iniciar_planificacion, NULL);
	pthread_join(hilo_planificacion, NULL);

    //log_trace(kernel_logger, "listo para escuchar al IO");
	log_trace(kernel_log_debug, "listo para escuchar al IO");
	fd_io = esperar_cliente(fd_kernel);

	pthread_t hilo_cpu_interrupt;
	pthread_create(&hilo_cpu_interrupt, NULL, (void*)atender_kernel_interrupt, NULL);
	pthread_detach(hilo_cpu_interrupt);

	pthread_t hilo_cpu_dispatch;
	pthread_create(&hilo_cpu_dispatch, NULL, (void*)atender_kernel_dispatch, NULL);
	pthread_detach(hilo_cpu_dispatch);

	pthread_t hilo_io;
	pthread_create(&hilo_io, NULL, (void*)atender_kernel_io, NULL);
	pthread_detach(hilo_io);

	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_kernel_memoria, NULL);
	pthread_detach(hilo_memoria);

	

	//Iniciar la consola interactiva
	iniciar_consola_interactiva();
	
	log_debug(kernel_log_debug, "Advertencia de salida de Kernel");

	printf("llegue al while");

	while(1){
		printf("estoy en el while\n");
		sleep(20);
	}

	return 0;
}

//iniciar servidor => retorna socket servidor
//esperar cliente => retorna socket cliente
//iniciar conexion / crear conexion => retorna socket cliente

