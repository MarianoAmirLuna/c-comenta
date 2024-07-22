#include "../include/kernel_dispatch.h"
#include <utils/shared.h>
#include "../include/funciones_kernel.h"

void atender_kernel_dispatch()
{
	bool control_key = 1;
	t_buffer *un_buffer;
	char *nombre_interfaz;

	while (control_key)
	{
		int cod_op = recibir_operacion(fd_cpu_dispatch);
		switch (cod_op)
		{
		case MENSAJE:
			//
			break;
		case PAQUETE:
			//
			break;
		case CONSULTA_PLANIFICACION:
			ciclo_planificacion();
			t_paquete *paquete_pid = crear_paquete();
			agregar_a_paquete(paquete_pid, procesoEXEC, sizeof(int));
			enviar_paquete(paquete_pid, fd_cpu_dispatch);
			destruir_paquete(paquete_pid);
			break;

		case RECIBIR_PCB:
			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			PCB *pcb_devuelto = atender_recibir_pcb(un_buffer);
			list_add(listaPCBs, pcb_devuelto);
			int codigo = extraer_int_del_buffer(un_buffer);
			estaCPULibre = true;
			sem_post(&esperar_devolucion_pcb);

			if (codigo == 1)
			{ // si hay cambio de contexto envio un 1 osea fue desalojado => le faltan instrucciones por ejecutar
				list_add(listaPCBs, pcb_devuelto);
				list_add(procesosREADY, &(pcb_devuelto->pid));
			}
			else if(contiene_numero(procesosEXIT,pcb_devuelto->pid))
			{ // ejecuto todas las instrucciones
				// significa que termino todas sus intrucciones y tengo que liberar los recursos y mandarlo a exit
				list_add(procesosEXIT,&(pcb_devuelto->pid));
				finalizarProceso(pcb_devuelto->pid); //agregar un poco mas de logica aca
			}
			break;
		case ENVIAR_IOGEN:
			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			char *nombreInterfaz = extraer_string_del_buffer(un_buffer);
			int unidadesTrabajo = extraer_int_del_buffer(un_buffer);
			break;

		case DESALOJO_POR_WAIT:
			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			PCB *pcb_devuelto_por_wait = atender_recibir_pcb(un_buffer);
			list_add(listaPCBs, pcb_devuelto_por_wait);
			estaCPULibre = true;
			sem_post(&esperar_devolucion_pcb);

			// list_add(procesosREADY, &(pcb_devuelto_por_wait->pid));

			char *nombre_recurso_wait = extraer_string_del_buffer(un_buffer);
			atender_wait(nombre_recurso_wait, &(pcb_devuelto_por_wait->pid));
			break;

		case DESALOJO_POR_SIGNAL:
			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			PCB *pcb_devuelto_por_signal = atender_recibir_pcb(un_buffer);
			estaCPULibre = true;
			sem_post(&esperar_devolucion_pcb);
			list_add(listaPCBs, pcb_devuelto_por_signal);
			list_add(procesosREADY, &(pcb_devuelto_por_signal->pid));

			char *nombre_recurso_signal = extraer_string_del_buffer(un_buffer);
			atender_signal(nombre_recurso_signal, &(pcb_devuelto_por_signal->pid));
			break;

		case INSTRUCCION_TIPO_IO: // significa que el proceso fue desalojado al ejecutar una instruccion de tipo io

			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			PCB *pcb_devuelto2 = atender_recibir_pcb(un_buffer);
			nombre_interfaz = extraer_string_del_buffer(un_buffer);
			char *tipo_instruccion = extraer_string_del_buffer(un_buffer);
			estaCPULibre = true;
			sem_post(&esperar_devolucion_pcb);

			printf("llego el pcb a cpu\n");
			printf("el nombre de la interfaz: %s\n",nombre_interfaz);

			interfaces_io *interfaz = encontrar_interfaz(nombre_interfaz);
			list_add(listaPCBs, pcb_devuelto2);

			if (interfaz == NULL) // si es NULL no la encuentra en la lista por lo tanto esta desconectada
			{
				printf("lo agrege a exit\n");
				list_add(procesosEXIT, &(pcb_devuelto2->pid));
			}
			else
			{
				if (!admiteOperacionInterfaz(nombre_interfaz, tipo_instruccion))
				{
					printf("lo agrege a exit 2.0\n");
					list_add(procesosEXIT, &(pcb_devuelto2->pid));
				}
				else
				{
					printf("lo agrege a la queue de procesos bloqueados\n");
					printf("de la interfaz: %s\n",interfaz->nombre_interfaz);
					queue_push(interfaz->procesos_bloqueados, &(pcb_devuelto2->pid)); // agrego el pid a la queue de bloqueados de dicha interfaz
				}
			}

			break;

		case ENVIAR_IO_GEN_SLEEP:
			int *unidades_trabajo = malloc(sizeof(int));
			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			nombre_interfaz = extraer_string_del_buffer(un_buffer);

			*unidades_trabajo = extraer_int_del_buffer(un_buffer);
			estaCPULibre = true;
			sem_post(&esperar_devolucion_pcb);
			interfaces_io *interfaz2 = encontrar_interfaz(nombre_interfaz);

			printf("el nombre de la insterfaz es: %s\n", nombre_interfaz);
			printf("las unidades de trabajo son: %d\n", *unidades_trabajo);

			if (interfaz2 != NULL && admiteOperacionInterfaz(nombre_interfaz, "IO_GEN_SLEEP"))
			{

				instruccion *instruccionXD = (instruccion *)malloc(sizeof(instruccion));

				instruccionXD->nombre_instruccion = "IO_GEN_SLEEP";
				instruccionXD->nombre_archivo = "";
				instruccionXD->lista_enteros = list_create();

				list_add(instruccionXD->lista_enteros, unidades_trabajo);

				queue_push(interfaz2->instrucciones_ejecutar,instruccionXD);
			}

			break;
		case ENVIAR_IO_STDIN_READ:
			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			nombre_interfaz = extraer_string_del_buffer(un_buffer);
			int *tamanio_restante_pag = malloc(sizeof(int));
			int *tamanio_escribir = malloc(sizeof(int));
			int *cant_direcciones = malloc(sizeof(int));

			*tamanio_restante_pag = extraer_int_del_buffer(un_buffer);
			*tamanio_escribir = extraer_int_del_buffer(un_buffer);
			*cant_direcciones = extraer_int_del_buffer(un_buffer);

			printf("restante escribir: %d\n",*tamanio_restante_pag);
			printf("tamanio_escribir: %d\n",*tamanio_escribir);
			printf("cant direcciones: %d\n",*cant_direcciones);

			estaCPULibre = true;
			sem_post(&esperar_devolucion_pcb);
			interfaces_io *interfaz3 = encontrar_interfaz(nombre_interfaz);

			if (interfaz3 != NULL && admiteOperacionInterfaz(nombre_interfaz, "IO_STDIN_READ"))
			{
				instruccion *instruccionXD2 = (instruccion *)malloc(sizeof(instruccion));
				instruccionXD2->nombre_instruccion = "IO_STDIN_READ";
				instruccionXD2->nombre_archivo = "";
				instruccionXD2->lista_enteros = list_create();

				list_add(instruccionXD2->lista_enteros, tamanio_restante_pag);
				list_add(instruccionXD2->lista_enteros, tamanio_escribir);
				list_add(instruccionXD2->lista_enteros, cant_direcciones);

				for (int i = 0; i < *cant_direcciones; i++)
				{
					int *df_p = malloc(sizeof(int));
					*df_p = extraer_int_del_buffer(un_buffer);

					printf("la df es: %d\n",*df_p);
					list_add(instruccionXD2->lista_enteros, df_p);
				}

				queue_push(interfaz3->instrucciones_ejecutar, instruccionXD2);
				printf("lo agrege a la queue\n");
			}

			break;
        case ENVIAR_IO_STDOUT_WRITE:

		    un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			nombre_interfaz = extraer_string_del_buffer(un_buffer);
			int *tamanio_escribir2 = malloc(sizeof(int));
			*tamanio_escribir2 = extraer_int_del_buffer(un_buffer);

            estaCPULibre = true;
			sem_post(&esperar_devolucion_pcb);
			interfaces_io *interfaz4 = encontrar_interfaz(nombre_interfaz);

			if (interfaz4 != NULL && admiteOperacionInterfaz(nombre_interfaz, "IO_STDOUT_WRITE"))
			{
				instruccion *instruccionXD3 = (instruccion *)malloc(sizeof(instruccion));

				instruccionXD3->nombre_instruccion = "IO_STDOUT_WRITE";
				instruccionXD3->nombre_archivo = "";
				instruccionXD3->lista_enteros = list_create();

				list_add(instruccionXD3->lista_enteros, tamanio_escribir2);

				for (int i = 0; i < *tamanio_escribir2; i++)
				{
					int *direccion_fisica_p = malloc(sizeof(int));
					*direccion_fisica_p = extraer_int_del_buffer(un_buffer);

					printf("la df es: %d\n", *direccion_fisica_p);
					list_add(instruccionXD3->lista_enteros, direccion_fisica_p);
				}

				queue_push(interfaz4->instrucciones_ejecutar, instruccionXD3);
				printf("lo agrege a la queue\n");
			}

            break;
		case -1:
			log_trace(kernel_log_debug, "Desconexion de KERNEL - Dispatch");
			// control_key = 0;
			break;
		default:
			log_warning(logger, "Operacion desconocida de KERNEL - Dispatch");
			break;
		}
	}
}