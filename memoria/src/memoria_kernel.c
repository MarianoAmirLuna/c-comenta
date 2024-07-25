#include "../include/memoria_kernel.h"
#include <pthread.h>
#include "../include/memoria_cpu.h"

path_conID *iniciar_path_id(int id, char *path)
{
	path_conID *estructura = malloc(sizeof(path_conID));
	estructura->id = id;
	estructura->path = path;

	return estructura;
}

void atender_crear_proceso(t_buffer *un_buffer)
{
	int pid = extraer_int_del_buffer(un_buffer);
	char *path = extraer_string_del_buffer(un_buffer);

	path_conID *path_con_id = iniciar_path_id(pid, path);
	tablaPaginas *tabla = inicializarTablaPaginas(pid);

	log_debug(memoria_log_debug, "PID: %d - Tamaño: %d", pid, tabla->cantMarcos);

	list_add(list_path_id, path_con_id);
	list_add(listaTablaPaginas, tabla);

	t_buffer *a_enviar = crear_buffer();

	a_enviar->size = 0;
	a_enviar->stream = NULL;

	cargar_int_al_buffer(a_enviar, pid);

	t_paquete *un_paquete = crear_super_paquete(HABILITAR_PID, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);
	usleep(RETARDO_RESPUESTA * 1000);
}

void atender_eliminar_proceso(t_buffer *un_buffer)
{
	int pid = extraer_int_del_buffer(un_buffer);
	tablaPaginas *tablaABorrar = obtener_tabla_pagina(pid);

	// log obligatorio de destruccion
	if (tablaABorrar != NULL)
	{
		log_debug(memoria_log_debug, "PID: %d - Tamaño: %d", pid, tablaABorrar->cantMarcos);

		list_remove_element(listaTablaPaginas, tablaABorrar); // Eliminamos 1ro la tabla de paginas de la lista de tabla de paginas
		int cantidadDeBitsVal = contarBitValidez(tablaABorrar);
		int numMarcoABorrar;

		// cambiamos el bit de validez del marco en el bitmap
		for (int i = 0; i < cantidadDeBitsVal; i++)
		{
			numMarcoABorrar = tablaABorrar->array[i].marco; // Marco es el de la memoria principal
			bitarray_clean_bit(frames_ocupados_ppal, numMarcoABorrar);
		}

		imprimirBitmapMemoriaPrincipal();
	}
	// Liberamos porque tabla era un maloc
	free(tablaABorrar);
}

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
			printf("llego el path a memoria\n");
			un_buffer = recibir_todo_el_buffer(fd_kernel);
			atender_crear_proceso(un_buffer);
			break;
		case ELIMINAR_PROCESO:
			printf("llego el pid para eliminar el proceso\n");
			un_buffer = recibir_todo_el_buffer(fd_kernel);
			atender_eliminar_proceso(un_buffer);
			break;
		case -1:
			log_trace(memoria_log_debug, "Desconexion de Kernel - Memoria");
			control_key = 0;
			break;
		default:
			// log_warning(logger,"Operacion desconocida de KERNEL");
			break;
		}
	}
}
