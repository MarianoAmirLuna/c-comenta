#include "../include/io_memoria.h"
#include <utils/shared.h>

void atender_interfaz_memoria(int *arg)
{
    int fd_entradasalida_memoria = *arg;
	bool control_key = 1;
	t_buffer *un_buffer;

	while (control_key)
	{
		int cod_op = recibir_operacion(fd_entradasalida_memoria);
		printf("el codigo de operacion es: %d\n", cod_op);

		switch (cod_op)
		{
            
		case HABLAR_CON_IO:
		    un_buffer = recibir_todo_el_buffer(fd_entradasalida_memoria);
			int xd = extraer_int_del_buffer(un_buffer);
			printf("logre recibir el mensaje de la lista de memoria\n");

            break;
		case -1:

			control_key = 0;
			break;
		default:
			log_warning(logger, "Operacion desconocida de KERNEL - IO");
			break;
		}
	}
}
