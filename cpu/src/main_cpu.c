#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include "../include/cpu.h"


int main(void) {

	inicializar_cpu();

	fd_cpu_dispatch = iniciar_servidor(PUERTO_ESCUCHA_DISPATCH);

	fd_cpu_interrupt = iniciar_servidor(PUERTO_ESCUCHA_INTERRUPT);

    fd_memoria = iniciar_conexion(PUERTO_MEMORIA, "CPU", cpu_log_debug);
	
	fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch);

	fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt);

	//atender los mensajes del kernel dispatch
	pthread_t hilo_kernel_dispatch;
	pthread_create(&hilo_kernel_dispatch, NULL, (void*)atender_cpu_kernel_dispatch, NULL);
	pthread_detach(hilo_kernel_dispatch);
	
	//atender los mensajes del kernel interrupt
	pthread_t hilo_kernel_interrupt;
	pthread_create(&hilo_kernel_interrupt, NULL, (void*)atender_cpu_kernel_interrupt, NULL);
	pthread_detach(hilo_kernel_interrupt);

    //atender los mensajes de la memoria

	//pthread_t hilo_procesamiento;
	//pthread_create(&hilo_procesamiento, NULL, (void*)procesamiento_cpu, NULL);
	//thread_detach(hilo_procesamiento);

	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_cpu_memoria, NULL);
	pthread_join(hilo_memoria, NULL);
	
	printf("llegue al while");

	while(1){
		printf("estoy en el while\n");
		sleep(20);
	}

	return 0;
}

//iniciar servidor => retorna socket servidor
//esperar cliente => retorna socket cliente
//iniciar conexion => retorna socket cliente
