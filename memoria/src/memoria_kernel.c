#include "../include/memoria_kernel.h"


void atender_memoria_kernel()
{
	bool control_key = 1;
	t_buffer *un_buffer;
	while (control_key)
	{
		int cod_op = recibir_operacion(fd_kernel);
		switch (cod_op)
		{
		case MENSAJE:

			break;

		case PAQUETE:

			break;

		case CREAR_PROCESO_KM:
		    printf("llego\n");
			un_buffer = recibir_todo_el_buffer(fd_kernel);
			atender_crear_proceso(un_buffer);
			break;

		case -1:
			log_trace(memoria_log_debug, "Desconexion de KERNEL");
			control_key = 0;
			break;

		default:
			log_warning(logger,"Operacion desconocida de KERNEL");
			break;
		}
	}
}

void atender_crear_proceso(t_buffer* un_buffer){
	int pid = extraer_int_del_buffer(un_buffer);
	char* path = extraer_string_del_buffer(un_buffer);

	printf("%d\n",pid);
	printf("%s\n",path);
	
	free(path);
}
