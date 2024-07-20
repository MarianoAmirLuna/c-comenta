#include "../include/io_kernel.h"
#include <utils/shared.h>
#include <readline/readline.h>
#include <commons/string.h>

void atender_interfaz_kernel(int *arg)
{
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
			break;

		case ENVIAR_IO_STDIN_READ:
			un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
			int tamanio_restante_pag_read = extraer_int_del_buffer(un_buffer);
			int tamanio_escribir_read = extraer_int_del_buffer(un_buffer);
			int cant_direcciones_read = extraer_int_del_buffer(un_buffer);
			char *input_usuario = readline("input >");

			t_buffer *buffer = crear_buffer();
			buffer->size = 0;
			buffer->stream = NULL;

			cargar_string_al_buffer(buffer, input_usuario);
			cargar_int_al_buffer(buffer, tamanio_restante_pag_read);
			cargar_int_al_buffer(buffer, tamanio_escribir_read);

			int numero;

			for (int i = 0; i < cant_direcciones_read; i++)
			{
				numero = extraer_int_del_buffer(un_buffer);
				cargar_int_al_buffer(buffer, numero);
			}

			t_paquete *paquete = crear_super_paquete(ESCRIBIR_MEMORIA, buffer);
			enviar_paquete(paquete, fd_memoria);
			destruir_paquete(paquete);

			break;

		case ENVIAR_IO_STDOUT_WRITE:
            un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
	        
            int tamanioXD = extraer_int_del_buffer(un_buffer);

			t_buffer *buffer2 = crear_buffer();
			buffer2->size = 0;
			buffer2->stream = NULL;

			cargar_string_al_buffer(buffer2,nombreInterACrear);
			cargar_int_al_buffer(buffer2,tamanioXD);

			for(int i = 0; i < tamanioXD; i++){

				numero = extraer_int_del_buffer(un_buffer);
				cargar_int_al_buffer(buffer2,numero);
			}

			t_paquete *paquete2 = crear_super_paquete(LEER_MEMORIA_PALABRA, buffer2);
			enviar_paquete(paquete2, fd_memoria);
			destruir_paquete(paquete2);

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