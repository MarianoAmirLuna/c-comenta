#include <../include/memoria.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <pthread.h>
#include "utils/shared.h"
#include <commons/log.h>



int main() {

	inicializar_memoria();	
	
	int socket_memoria = iniciar_servidor(PUERTO_ESCUCHA);
   
	log_trace(memoria_logger, "listo para escuchar al CPU");
	
	socket_cliente_CPU = esperar_cliente(socket_memoria);  //esto me esta rompiendo todo

	log_trace(memoria_logger, "listo para escuchar al KERNEL");
	socket_cliente_KERNEL = esperar_cliente(socket_memoria);

	log_trace(memoria_logger, "listo para escuchar al IO");
	socket_cliente_IO = esperar_cliente(socket_memoria);


	return 0;
}