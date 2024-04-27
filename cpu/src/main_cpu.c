#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include "../include/cpu.h"

int main(void) {

	inicializar_cpu();

	int socket_CPU_DISPATCH = iniciar_servidor(PUERTO_ESCUCHA_DISPATCH);

	int socket_CPU_INTERRUPT = iniciar_servidor(PUERTO_ESCUCHA_INTERRUPT);

    iniciar_conexion(PUERTO_MEMORIA, "CPU");

	int socket_cliente_DISPATCH = esperar_cliente(socket_CPU_DISPATCH);

	int socket_cliente_INTERRUPT = esperar_cliente(socket_CPU_INTERRUPT);

	return 0;
}
