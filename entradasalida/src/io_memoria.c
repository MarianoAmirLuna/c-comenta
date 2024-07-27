#include "../include/io_memoria.h"
#include <utils/shared.h>
#include "../include/io_kernel.h"

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
		case DEVOLVER_STRING_STDOUT:
		    un_buffer = recibir_todo_el_buffer(fd_entradasalida_memoria);
			char* mensajeFINAL = extraer_string_del_buffer(un_buffer);

			printf("mensaje obtenido: %s\n",mensajeFINAL);

			avisarKernelTerminoEjecutarIO();

			break;
		case LIBERAR_INTERFAZ:
		    un_buffer = recibir_todo_el_buffer(fd_entradasalida_memoria);
			int inutil = extraer_int_del_buffer(un_buffer);
            avisarKernelTerminoEjecutarIO();
		case DEVOLVER_STRING_FS:
		    un_buffer = recibir_todo_el_buffer(fd_entradasalida_memoria);
			palabraIOWrite = extraer_string_del_buffer(un_buffer);
			sem_post(&esperar_palabra_memoria);

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
