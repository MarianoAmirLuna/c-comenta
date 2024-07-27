#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include "../include/cpu.h"


int main(int argc, char** argv) {
//int main() {

	if (argc < 2) {
        fprintf(stderr, "Uso: %s <ruta_al_archivo_de_configuracion>\n", argv[0]);
        return EXIT_FAILURE;
    }

	inicializar_cpu(argv[1]);
	//inicializar_cpu("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/cpu/planificacion.config");

	fd_cpu_dispatch = iniciar_servidor(PUERTO_ESCUCHA_DISPATCH);

	fd_cpu_interrupt = iniciar_servidor(PUERTO_ESCUCHA_INTERRUPT);

    fd_memoria = iniciar_conexion(IP_MEMORIA, PUERTO_MEMORIA, "CPU", cpu_log_debug);
	
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
	

	return 0;
}

//iniciar servidor => retorna socket servidor
//esperar cliente => retorna socket cliente
//iniciar conexion => retorna socket cliente
