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
	atender_cpu_kernel_dispatch();
	
	//atender los mensajes del kernel interrupt
	atender_cpu_kernel_interrupt();

    //atender los mensajes de la memoria
	atender_cpu_memoria();

	return 0;
}

//iniciar servidor => retorna socket servidor
//esperar cliente => retorna socket cliente
//iniciar conexion => retorna socket cliente
