#include "../include/io_kernel.h"
#include <utils/shared.h>

void atender_interfaz_kernel(int *arg){
    int fd_entradasalida_kernel = *arg;
	bool control_key = 1;
	t_buffer *un_buffer;

	while (control_key)
	{
		int cod_op = recibir_operacion(fd_entradasalida_kernel);
		printf("el codigo de operacion es: %d\n", cod_op);

		switch (cod_op)
		{
		case HABLAR_CON_IO:
		    un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
			int xd = extraer_int_del_buffer(un_buffer);
			printf("RECIBI EL MENSAJEEEEEEEEEEEEEEEEEE\n"); 

            break;
		case ENVIAR_IO_GEN_SLEEP:
            un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
			int unidades_trabajo = extraer_int_del_buffer(un_buffer);

			printf("mande a dormir a la io\n");

			usleep(unidades_trabajo * TIEMPO_UNIDAD_TRABAJO * 1000);

		case -1:

			control_key = 0;
			break;
		default:
			log_warning(logger, "Operacion desconocida de KERNEL - IO");
			break;
		}
	}
}