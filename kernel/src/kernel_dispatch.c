#include "../include/kernel_dispatch.h"
#include <utils/shared.h>
#include "../include/funciones_kernel.h"

void atender_kernel_dispatch()
{
	bool control_key = 1;
	t_buffer *un_buffer;
	char *nombre_interfaz;
	int tiempo_q_prima;
	int numero_hiloXD;
	char* nombreArchivo;
	interfaces_io *interfazXD;
	instruccion *instruccion_io;
	int tamanio;
	int registroPuntero;

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

		case RECIBIR_PCB:
			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			PCB *pcb_devuelto = atender_recibir_pcb(un_buffer);
			list_add(listaPCBs, pcb_devuelto);

			int codigo = extraer_int_del_buffer(un_buffer);
            numero_hiloXD = extraer_int_del_buffer(un_buffer);
			removerNumeroLista(lista_id_hilos,numero_hiloXD);
			
			estaCPULibre = true;
			sem_post(&esperar_devolucion_pcb);
			quantum_global_reloj = QUANTUM;
			sem_post(&contador_q);
			sem_post(&esperar_termine_ejecutar_pcb_cpu);

			detener_tiempo();
			//log_trace(kernel_log_debug, "PID: %d - Desalojado por ", pcb_devuelto->pid);

			if (codigo == 1)
			{ // si hay cambio de contexto envio un 1 osea fue desalojado => le faltan instrucciones por ejecutar

				pthread_mutex_lock(&proteger_lista_ready);
				list_add(procesosREADY, &(pcb_devuelto->pid));
				pthread_mutex_unlock(&proteger_lista_ready);
			}
			else
			{ // ejecuto todas las instrucciones
				// significa que termino todas sus intrucciones y tengo que liberar los recursos y mandarlo a exit
				list_add(procesosEXIT, &(pcb_devuelto->pid));
				//finalizarProceso(pcb_devuelto->pid); // agregar un poco mas de logica aca
			}
			sem_post(&nuevo_bucle);

			break;
		case DESALOJO_POR_WAIT:
			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			PCB *pcb_devuelto_por_wait = atender_recibir_pcb(un_buffer);
			list_add(listaPCBs, pcb_devuelto_por_wait);
			estaCPULibre = true;
			sem_post(&esperar_devolucion_pcb);
			quantum_global_reloj = QUANTUM;
			sem_post(&contador_q);
			sem_post(&esperar_termine_ejecutar_pcb_cpu);
			char *nombre_recurso_wait = extraer_string_del_buffer(un_buffer);
			numero_hiloXD = extraer_int_del_buffer(un_buffer);
			removerNumeroLista(lista_id_hilos,numero_hiloXD);

			detener_tiempo();
			tiempo_q_prima = tiempo_transcurrido_milisegundos(start_time, end_time);
			// actualizarQPrimaProceso(estaEJecutando,tiempo_q_prima);
			//printf("el contador dio: %d\n", tiempo_q_prima);
			//printf("ACAAAAAAAAAAAAAAAAAAAA\n");

			// list_add(procesosREADY, &(pcb_devuelto_por_wait->pid));

			atender_wait(nombre_recurso_wait, &(pcb_devuelto_por_wait->pid));
			sem_post(&nuevo_bucle);
			break;

		case DESALOJO_POR_SIGNAL:
			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			PCB *pcb_devuelto_por_signal = atender_recibir_pcb(un_buffer);
			estaCPULibre = true;
			sem_post(&esperar_devolucion_pcb);
			list_add(listaPCBs, pcb_devuelto_por_signal);
			list_add(procesosREADY, &(pcb_devuelto_por_signal->pid));
			quantum_global_reloj = QUANTUM;
			sem_post(&contador_q);
			sem_post(&esperar_termine_ejecutar_pcb_cpu);
			char *nombre_recurso_signal = extraer_string_del_buffer(un_buffer);
			numero_hiloXD = extraer_int_del_buffer(un_buffer);
			removerNumeroLista(lista_id_hilos,numero_hiloXD);

			detener_tiempo();
			tiempo_q_prima = tiempo_transcurrido_milisegundos(start_time, end_time);
			// actualizarQPrimaProceso(estaEJecutando,tiempo_q_prima);
			//printf("el contador dio: %d\n", tiempo_q_prima);
			//printf("ACAAAAAAAAAAAAAAAAAAAA\n");

			atender_signal(nombre_recurso_signal, &(pcb_devuelto_por_signal->pid));
			sem_post(&nuevo_bucle);
			break;

		case INSTRUCCION_TIPO_IO: // significa que el proceso fue desalojado al ejecutar una instruccion de tipo io
								  // TIENE QUE VER CON EL PCB NO LA INSTRUCCION
			// TIENE QUE VER CON EL PCB NO LA INSTRUCCION
			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			PCB *pcb_devuelto2 = atender_recibir_pcb(un_buffer);
			nombre_interfaz = extraer_string_del_buffer(un_buffer);
			char *tipo_instruccion = extraer_string_del_buffer(un_buffer);
			estaCPULibre = true;
			sem_post(&esperar_devolucion_pcb);
			quantum_global_reloj = QUANTUM;
			sem_post(&contador_q);
			sem_post(&esperar_termine_ejecutar_pcb_cpu);
			numero_hiloXD = extraer_int_del_buffer(un_buffer);
			removerNumeroLista(lista_id_hilos,numero_hiloXD);

			detener_tiempo();

			tiempo_q_prima = tiempo_transcurrido_milisegundos(start_time, end_time);
			actualizarQPrimaProceso(pcb_devuelto2->pid,tiempo_q_prima);
			//log_trace(kernel_log_debug, "PID: %d - Desalojado por IO", pcb_devuelto2->pid);

			//printf("el contador dio: %d\n", tiempo_q_prima);
			//printf("ACAAAAAAAAAAAAAAAAAAAA\n");

			//printf("llego el pcb a cpu\n");
			//printf("el nombre de la interfaz: %s\n", nombre_interfaz);

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
					printf("de la interfaz: %s\n", interfaz->nombre_interfaz);
					queue_push(interfaz->procesos_bloqueados, &(pcb_devuelto2->pid)); //agrego el pid a la queue de bloqueados de dicha interfaz
				}
			}
			sem_post(&nuevo_bucle);

			break;

		case ENVIAR_IO_GEN_SLEEP:
			int *unidades_trabajo = malloc(sizeof(int));
			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			nombre_interfaz = extraer_string_del_buffer(un_buffer);

			*unidades_trabajo = extraer_int_del_buffer(un_buffer);

			// sem_post(&esperar_devolucion_pcb);
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

				queue_push(interfaz2->instrucciones_ejecutar, instruccionXD);
				sem_post(&ciclo_instruccion_io);
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

			printf("restante escribir: %d\n", *tamanio_restante_pag);
			printf("tamanio_escribir: %d\n", *tamanio_escribir);
			printf("cant direcciones: %d\n", *cant_direcciones);

			// sem_post(&esperar_devolucion_pcb);
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

					printf("la df es: %d\n", *df_p);
					list_add(instruccionXD2->lista_enteros, df_p);
				}

				queue_push(interfaz3->instrucciones_ejecutar, instruccionXD2);
				sem_post(&ciclo_instruccion_io);
				printf("lo agrege a la queue\n");
			}

			break;
		case ENVIAR_IO_STDOUT_WRITE:

			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			nombre_interfaz = extraer_string_del_buffer(un_buffer);
			int *tamanio_escribir2 = malloc(sizeof(int));
			*tamanio_escribir2 = extraer_int_del_buffer(un_buffer);

			// sem_post(&esperar_devolucion_pcb);
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
				sem_post(&ciclo_instruccion_io);
				printf("lo agrege a la queue\n");
			}

			break;
		case ENVIAR_IO_FS_CREATE:
		    un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			nombre_interfaz = extraer_string_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);

			interfazXD = encontrar_interfaz(nombre_interfaz);

			if (interfazXD != NULL && admiteOperacionInterfaz(nombre_interfaz, "IO_FS_CREATE"))
			{
				instruccion_io = (instruccion *)malloc(sizeof(instruccion));

				instruccion_io->nombre_instruccion = "IO_FS_CREATE";
				instruccion_io->nombre_archivo = nombreArchivo;
				instruccion_io->lista_enteros = list_create();

				queue_push(interfazXD->instrucciones_ejecutar, instruccion_io);
				sem_post(&ciclo_instruccion_io);
				printf("lo agrege a la queue\n");
			}

		    break;
		
		case ENVIAR_IO_FS_DELETE:
            un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			nombre_interfaz = extraer_string_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);

			interfazXD = encontrar_interfaz(nombre_interfaz);

			if (interfazXD != NULL && admiteOperacionInterfaz(nombre_interfaz, "IO_FS_DELETE"))
			{
				instruccion_io = (instruccion *)malloc(sizeof(instruccion));

				instruccion_io->nombre_instruccion = "IO_FS_DELETE";
				instruccion_io->nombre_archivo = nombreArchivo;
				instruccion_io->lista_enteros = list_create();

				queue_push(interfazXD->instrucciones_ejecutar, instruccion_io);
				sem_post(&ciclo_instruccion_io);
				printf("lo agrege a la queue\n");
			}

		    break;
		case ENVIAR_IO_FS_TRUNCATE:
		    un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			nombre_interfaz = extraer_string_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);
			int* tamanio_truncate = malloc(sizeof(int)); 
			*tamanio_truncate = extraer_int_del_buffer(un_buffer);

			interfazXD = encontrar_interfaz(nombre_interfaz);

			if (interfazXD != NULL && admiteOperacionInterfaz(nombre_interfaz, "IO_FS_TRUNCATE"))
			{
				instruccion_io = (instruccion *)malloc(sizeof(instruccion));

				instruccion_io->nombre_instruccion = "IO_FS_TRUNCATE";
				instruccion_io->nombre_archivo = nombreArchivo;
				instruccion_io->lista_enteros = list_create();

				list_add(instruccion_io->lista_enteros,tamanio_truncate);

				queue_push(interfazXD->instrucciones_ejecutar, instruccion_io);
				sem_post(&ciclo_instruccion_io);
				printf("lo agrege a la queue\n");
			}

		    break;
		
		case ENVIAR_IO_FS_WRITE:
		    un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			nombre_interfaz = extraer_string_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);
			int* registro_puntero_write = malloc(sizeof(int));
			int* tamanio_write = malloc(sizeof(int));

			*registro_puntero_write = extraer_int_del_buffer(un_buffer);
			*tamanio_write = extraer_int_del_buffer(un_buffer);

			interfazXD = encontrar_interfaz(nombre_interfaz);

            if (interfazXD != NULL && admiteOperacionInterfaz(nombre_interfaz, "IO_FS_WRITE"))
			{
				instruccion_io = (instruccion *)malloc(sizeof(instruccion));

				instruccion_io->nombre_instruccion = "IO_FS_WRITE";
				instruccion_io->nombre_archivo = nombreArchivo;
				instruccion_io->lista_enteros = list_create();

				list_add(instruccion_io->lista_enteros,registro_puntero_write);
				list_add(instruccion_io->lista_enteros,tamanio_write);

				for (int i = 0; i < *tamanio_write; i++)
				{
					int *direccion_fisica_write = malloc(sizeof(int));
					*direccion_fisica_write = extraer_int_del_buffer(un_buffer);

					printf("la df es: %d\n", *direccion_fisica_write);
					list_add(instruccion_io->lista_enteros, direccion_fisica_write);
				}

				queue_push(interfazXD->instrucciones_ejecutar, instruccion_io);
				sem_post(&ciclo_instruccion_io);
				printf("lo agrege a la queue\n");
			}
		    
		    break;
		case ENVIAR_IO_FS_READ:
		    un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			nombre_interfaz = extraer_string_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);

			int *bytes_restantes = malloc(sizeof(int));
			int *tamanio_escribirXD = malloc(sizeof(int));
			int *cant_direccionesXD = malloc(sizeof(int));
			int *registro_puntero = malloc(sizeof(int));

			*registro_puntero = extraer_int_del_buffer(un_buffer);
			*bytes_restantes = extraer_int_del_buffer(un_buffer);
			*tamanio_escribirXD = extraer_int_del_buffer(un_buffer);
			*cant_direccionesXD = extraer_int_del_buffer(un_buffer);

			printf("restante escribir: %d\n", *bytes_restantes);
			printf("tamanio_escribir: %d\n", *tamanio_escribirXD);
			printf("cant direcciones: %d\n", *cant_direccionesXD);

			// sem_post(&esperar_devolucion_pcb);
			interfazXD = encontrar_interfaz(nombre_interfaz);

			if (interfazXD != NULL && admiteOperacionInterfaz(nombre_interfaz, "IO_FS_READ"))
			{
				instruccion_io = (instruccion *)malloc(sizeof(instruccion));
				instruccion_io->nombre_instruccion = "IO_FS_READ";
				instruccion_io->nombre_archivo = nombreArchivo;
				instruccion_io->lista_enteros = list_create();
				
                list_add(instruccion_io->lista_enteros, registro_puntero);
				list_add(instruccion_io->lista_enteros, bytes_restantes);
				list_add(instruccion_io->lista_enteros, tamanio_escribirXD);
				list_add(instruccion_io->lista_enteros, cant_direccionesXD);

				for (int i = 0; i < *cant_direccionesXD; i++)
				{
					int *df_read = malloc(sizeof(int));
					*df_read = extraer_int_del_buffer(un_buffer);

					printf("la df es: %d\n", *df_read);
					list_add(instruccion_io->lista_enteros, df_read);
				}

				queue_push(interfazXD->instrucciones_ejecutar, instruccion_io);
				sem_post(&ciclo_instruccion_io);
				printf("lo agrege a la queue\n");
			}
		    break;
		case CONSULTA_PLANIFICACION:
			ciclo_planificacion();
			t_paquete *paquete_pid = crear_paquete();
			agregar_a_paquete(paquete_pid, procesoEXEC, sizeof(int));
			enviar_paquete(paquete_pid, fd_cpu_dispatch);
			destruir_paquete(paquete_pid);
			break;
		case ENVIAR_IOGEN: // creo que esta de mas
			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			char *nombreInterfaz = extraer_string_del_buffer(un_buffer);
			int unidadesTrabajo = extraer_int_del_buffer(un_buffer);
			break;
		case CONSULTA_PID:
			un_buffer = recibir_todo_el_buffer(fd_cpu_interrupt);
			consulta_pid_ejecucion = extraer_int_del_buffer(un_buffer);
			//sem_post(&esperar_consulta_pid);
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