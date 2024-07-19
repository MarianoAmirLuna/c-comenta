#include "../include/memoria_io.h"
#include <utils/shared.h>

void atender_creacion_interfaz(int *arg)
{
	int fd_entradasalida_memoria = *arg;
	free(arg);
	bool control_key = 1;
	t_buffer *un_buffer;

	while (control_key)
	{
		int cod_op = recibir_operacion(fd_entradasalida_memoria);
		printf("el codigo de operacion es: %d\n", cod_op);
		switch (cod_op)
		{
		case CREAR_INTERFAZ:
		    printf("estoy por crear la interfaz\n");

		    interfaces_io_memoria* nueva_interfaz = (interfaces_io_memoria*)malloc(sizeof(interfaces_io_memoria));

			un_buffer = recibir_todo_el_buffer(fd_entradasalida_memoria);

            char* nombre_interfaz = extraer_string_del_buffer(un_buffer);

			printf("el nombre dela interfaz: %s\n",nombre_interfaz);

            nueva_interfaz->fd_interfaz = fd_entradasalida_memoria; 
            nueva_interfaz->nombre_interfaz = nombre_interfaz;

            list_add(lista_interfaces, nueva_interfaz);
            
			printf("agrege la nueva interfaz a la queue\n");

			int size = list_size(lista_interfaces);

			printf("el tamanio de la list interfaces: %d\n",size);

			break;
		case -1:
			printf("Desconexion de KERNEL - IO");
			control_key = 0;
			break;
		default:
			log_warning(logger, "Operacion desconocida de KERNEL - IO");
			break;
		}
	}
}