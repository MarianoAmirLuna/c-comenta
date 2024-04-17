#include "main_kernel.h"
#include <stdlib.h>
#include <stdio.h>
//#include <utils/hello.h>


int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd); //dentro de esta funcion hay un accept la cual es bloqueante
	//retorna un nuevo socket (file descriptor) que representa la conexión BIDIRECCIONAL entre ambos procesos.

	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd); //en base a la operacion te da un valor para desp clasificar los datos que se enviaron en el switch
		//sirve para filtrar y saber que mandaste
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}
