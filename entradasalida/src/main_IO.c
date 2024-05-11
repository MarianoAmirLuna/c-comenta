#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <../include/IO.h>
#include <../include/inicializar_io.h>

int main(){

	
	inicializar_io();

	fd_memoria = iniciar_conexion(PUERTO_MEMORIA, "MEMORIA",io_log_debug);

	fd_kernel = iniciar_conexion(PUERTO_KERNEL, "KERNEL",io_log_debug);

	printf("EL socket de memoria es: %d\n",fd_memoria);
	printf("EL socket de kernel es: %d\n",fd_kernel);

	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_io_memoria, NULL);
	pthread_join(hilo_memoria, NULL);

	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_io_kernel, NULL);
	pthread_detach(hilo_kernel);

	
	return 0;
	
}

//iniciar servidor => retorna socket servidor
//esperar cliente => retorna socket cliente
//iniciar conexion / crear conexion => retorna socket cliente




