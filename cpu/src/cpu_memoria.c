#include "../include/cpu_memoria.h"
#include <utils/shared.h>

void atender_cpu_memoria()
{
	bool control_key = 1;
	t_buffer *un_buffer;
	while (control_key)
	{
		int cod_op = recibir_operacion(fd_memoria);
		switch (cod_op)
		{
		case MENSAJE:

			break;
		case PAQUETE:

			break;
		case RECIBIR_INSTRUCCION:
			printf("llego la instruccion a cpu\n");
			un_buffer = recibir_todo_el_buffer(fd_memoria);

            char* instruccion = extraer_string_del_buffer(un_buffer);

			printf("La instruccion es: %s\n",instruccion);
			
			break;
		case -1:
			log_trace(cpu_log_debug, "Desconexion de CPU - MEMORIA");
			control_key = 0;
			break;
		default:
			log_warning(logger,"Operacion desconocida de CPU - MEMORIA");
			break;
		}
	}
}

