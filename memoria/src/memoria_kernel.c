#include "../include/memoria_kernel.h"
#include <utils/shared.h>

void atender_memoria_kernel()
{
	bool control_key = 1;
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));
	recv(fd_kernel, &(paquete->codigo_operacion), sizeof(int), 0);

	while (control_key)
	{
		switch (paquete->codigo_operacion)
		{
		case CREAR_PROCESO_KM:
			recv(fd_kernel, &(paquete->buffer->size), sizeof(int), 0);
			paquete->buffer->stream = malloc(paquete->buffer->size);
			recv(fd_kernel, paquete->buffer->stream, paquete->buffer->size, 0);

		    path_conID* path_id = deserializar(paquete->buffer);
			//printf("%d\n",path_id->id);
			//printf("%d\n",path_id->path);
			break; //desp hay que destruir el paquete 

		case -1:
			log_trace(memoria_log_debug, "Desconexion de KERNEL - MEMORIA");
			control_key = 0;
			break;
		default:
			log_warning(logger, "Operacion desconocida de KERNEL - MEMORIA");
			break;
		}
	}
}

path_conID* deserializar(t_buffer* buffer){
	path_conID* path_id;

	//path_id->id = extraer_int_del_buffer(buffer);
	//path_id->path = extraer_string_del_buffer(buffer);

	int numero = extraer_int_del_buffer(buffer);
	char* palabra = extraer_string_del_buffer(buffer);

	printf("%d\n",numero);
	printf("%s\n",palabra);

	return path_id;
}