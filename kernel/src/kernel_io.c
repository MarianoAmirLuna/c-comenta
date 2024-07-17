#include "../include/kernel_io.h"
#include <utils/shared.h>

void atender_creacion_interfaz(int *arg)
{
	int fd_entradasalida_kernel = *arg;
	free(arg);
	bool control_key = 1;
	t_buffer *un_buffer;

	while (control_key)
	{
		int cod_op = recibir_operacion(fd_entradasalida_kernel);
		printf("el codigo de operacion es: %d\n", cod_op);
		switch (cod_op)
		{
		case CREAR_INTERFAZ:
		    printf("estoy por crear la interfaz\n");

		    interfaces_io* nueva_interfaz = malloc(sizeof(interfaces_io));
			un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);

            char* nombre_interfaz = extraer_string_del_buffer(un_buffer);
            char* tipo_interfaz = extraer_string_del_buffer(un_buffer);

            nueva_interfaz->fd_interfaz = fd_entradasalida_kernel;
            nueva_interfaz->nombre_interfaz = nombre_interfaz;
            nueva_interfaz->tipo_interfaz = tipo_interfaz;

            queue_push(lista_interfaces, nueva_interfaz);
            
			printf("agrege la nueva interfaz a la queue\n");

            free(nueva_interfaz);

			break;
		case -1:
			log_trace(kernel_log_debug, "Desconexion de KERNEL - IO");
			control_key = 0;
			break;
		default:
			log_warning(logger, "Operacion desconocida de KERNEL - IO");
			break;
		}
	}
}
