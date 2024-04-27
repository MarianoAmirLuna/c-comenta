#include "../include/kernel.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <pthread.h>
#include "utils/shared.h"
#include <commons/log.h>

int main(void)
{
	//recibe mensajes del interrupt / dispatch / io / memoria

	inicializar_kernel();

	fd_kernel = iniciar_servidor(PUERTO_ESCUCHA);

    fd_memoria = iniciar_conexion(PUERTO_MEMORIA, "MEMORIA",kernel_log_debug);

	fd_cpu_interrupt = iniciar_conexion(PUERTO_CPU_INTERRUPT, "CPU-INTERRUPT",kernel_log_debug);

	fd_cpu_dispatch = iniciar_conexion(PUERTO_CPU_DISPATCH, "CPU-DISPATCH",kernel_log_debug);

    log_trace(kernel_logger, "listo para escuchar al IO");
	fd_io = esperar_cliente(fd_kernel);

    atender_kernel_interrupt();

	atender_kernel_dispatch();

	atender_kernel_io();

	atender_kernel_memoria();

	return 0;
}

//iniciar servidor => retorna socket servidor
//esperar cliente => retorna socket cliente
//iniciar conexion / crear conexion => retorna socket cliente

