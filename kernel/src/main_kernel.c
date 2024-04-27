#include "../include/kernel.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <pthread.h>
#include "utils/shared.h"
#include <commons/log.h>


int main(void)
{
	
	inicializar_kernel();

	int socket_KERNEL = iniciar_servidor(PUERTO_ESCUCHA);

    iniciar_conexion(PUERTO_MEMORIA, "MEMORIA");

	iniciar_conexion(PUERTO_CPU_INTERRUPT, "CPU-INTERRUPT");

	iniciar_conexion(PUERTO_CPU_DISPATCH, "CPU-DISPATCH");

    log_trace(kernel_logger, "listo para escuchar al IO");
	int socket_cliente_INTERRUPT = esperar_cliente(socket_KERNEL);

	return 0;
}

