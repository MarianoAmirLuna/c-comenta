#include "../include/memoria_io.h"
#include <utils/shared.h>
#include "../include/memoria_cpu.h"

int encontrar_fd_interfaz(char *nombre_buscado)
{
	for (int i = 0; i < list_size(lista_interfaces); i++)
	{
		interfaces_io_memoria *elemento = list_get(lista_interfaces, i);

		if (strcmp(elemento->nombre_interfaz, nombre_buscado) == 0)
		{
			printf("encontre el fd\n");
			return elemento->fd_interfaz;
		}
	}

	return -1; // No se encontró la interfaz con el nombre buscado
}

void leerMemoria(t_buffer *un_buffer)
{
	char* nombre_interfaz = extraer_string_del_buffer(un_buffer);
	int fd_encontrado = encontrar_fd_interfaz(nombre_interfaz);

	printf("el nombre_interfaz: %s\n",nombre_interfaz);
	printf("su fd %d\n",fd_encontrado);
	
	int length = extraer_int_del_buffer(un_buffer);

	printf("llegue a leer memoria\n");

	sleep(1);
	printf("me fui a mimir 1 sg\n");

	char palabra[length + 1];
	palabra[length] = '\0';

	for (int i = 0; i < length; i++)
	{

		uint8_t datoLeido;
		int df = extraer_int_del_buffer(un_buffer);
		memcpy(&datoLeido, memoriaPrincipal + df, 1);
		palabra[i] = (char)datoLeido;
	}

	printf("la palabra es: %s\n", palabra);

	char* palabra_final = (char*)malloc(length + 1);

	memcpy(palabra_final, palabra, length);
    // Asegura que la cadena esté terminada con un carácter nulo
    palabra_final[length] = '\0';

	t_buffer *buffer = crear_buffer();
	buffer->size = 0;
	buffer->stream = NULL;

	cargar_string_al_buffer(buffer,palabra_final);

	t_paquete *paquete = crear_super_paquete(DEVOLVER_STRING_STDOUT, buffer);
	enviar_paquete(paquete, fd_encontrado);
	destruir_paquete(paquete);
}

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

			interfaces_io_memoria *nueva_interfaz = (interfaces_io_memoria *)malloc(sizeof(interfaces_io_memoria));

			un_buffer = recibir_todo_el_buffer(fd_entradasalida_memoria);

			char *nombre_interfaz = extraer_string_del_buffer(un_buffer);

			printf("el nombre dela interfaz: %s\n", nombre_interfaz);

			nueva_interfaz->fd_interfaz = fd_entradasalida_memoria;
			nueva_interfaz->nombre_interfaz = nombre_interfaz;

			list_add(lista_interfaces, nueva_interfaz);

			printf("agrege la nueva interfaz a la queue\n");

			int size = list_size(lista_interfaces);

			printf("el tamanio de la list interfaces: %d\n", size);

			break;

		case ESCRIBIR_MEMORIA:
			un_buffer = recibir_todo_el_buffer(fd_entradasalida_memoria);
			escribirMemoria(un_buffer);
			break;

		case LEER_MEMORIA_PALABRA:
			un_buffer = recibir_todo_el_buffer(fd_entradasalida_memoria);
			leerMemoria(un_buffer);

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