#include "../include/kernel_io.h"
#include <utils/shared.h>
#include "../include/funciones_kernel.h"

void desbloquear_el_proceso_de_la_iterfaz(char *nombre_interfaz)
{

	interfaces_io *interfaz = encontrar_interfaz(nombre_interfaz);

	// busco la interfaz en de la lista y lo mando al planificador otra vez
	int *pid = malloc(sizeof(int));


	if (list_size(interfaz->procesos_bloqueados->elements) > 0)
	{
		pid = (int *)queue_pop(interfaz->procesos_bloqueados);

		printf("el pid que fue liberado %d\n", *pid);

		int qPrimaNueva = buscarQPrima(*pid);
		if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0 && qPrimaNueva != QUANTUM)
		{
			list_add(procesos_READY_priori, pid);
			mostrarUnaLista(procesos_READY_priori, "Ready Prioritario");
			log_info(kernel_log_debug, "PID: %d - agrege a q prima y le di un tiempo de %d", *pid, qPrimaNueva);
		}
		else
		{
			list_add(procesosREADY, pid);
			mostrarUnaLista(procesosREADY, "Ready");
		}
    }
	
	interfaz->estaLibre = true;
	sem_post(&nuevo_bucle);
	sem_post(&ciclo_instruccion_io);
	
}

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

			interfaces_io *nueva_interfaz = (interfaces_io *)malloc(sizeof(interfaces_io));

			un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);

			char *nombre_interfaz = extraer_string_del_buffer(un_buffer);
			char *tipo_interfaz = extraer_string_del_buffer(un_buffer);

			printf("el nombre dela interfaz: %s\n", nombre_interfaz);
			printf("el tipo dela interfaz: %s\n", tipo_interfaz);

			nueva_interfaz->fd_interfaz = fd_entradasalida_kernel;
			nueva_interfaz->nombre_interfaz = nombre_interfaz;
			nueva_interfaz->tipo_interfaz = tipo_interfaz;
			nueva_interfaz->procesos_bloqueados = queue_create();
			nueva_interfaz->instrucciones_ejecutar = queue_create();
			nueva_interfaz->estaLibre = true;

			list_add(lista_interfaces, nueva_interfaz);

			printf("agrege la nueva interfaz a la queue\n");

			int size = list_size(lista_interfaces);

			printf("el tamanio de la list interfaces: %d\n", size);
			break;
		case LIBERAR_INTERFAZ:

			un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
			char *name_interfaz = extraer_string_del_buffer(un_buffer);

			desbloquear_el_proceso_de_la_iterfaz(name_interfaz);

			break;
		case -1:
			log_info(kernel_log_debug, "Desconexion de KERNEL - IO");
			control_key = 0;
			break;
		default:
			log_warning(logger, "Operacion desconocida de KERNEL - IO");
			break;
		}
	}
}
