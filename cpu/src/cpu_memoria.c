#include "../include/cpu_memoria.h"
#include "../include/funciones_cpu.h"
#include <utils/shared.h>

void atender_cpu_memoria()
{
	bool control_key = 1;
	t_buffer *un_buffer;
	while (control_key)
	{
		int cod_op = recibir_operacion(fd_memoria);
		printf("codigo de operacion: %d\n",cod_op);
		switch (cod_op)
		{
		case MENSAJE:

			break;
		case PAQUETE:

			break;
		case RECIBIR_INSTRUCCION:
			printf("llego la instruccion a cpu\n");
			un_buffer = recibir_todo_el_buffer(fd_memoria);

            instruccion_actual = extraer_string_del_buffer(un_buffer);

			//printf("La instruccion es: %s\n",instruccion_actual);

			sem_post(&wait_instruccion);
			
			break;
		case RECIBIR_TAMANIO:

			un_buffer = recibir_todo_el_buffer(fd_memoria);

			tamanio_pagina = extraer_int_del_buffer(un_buffer);

		    printf("llego el tamanio a cpu %d\n", tamanio_pagina);

			sem_post(&esperarTamanioDePagina);
			
			break;
		case ESCRITURA_HECHA:

			un_buffer = recibir_todo_el_buffer(fd_memoria);

			int direccion_fisica = extraer_int_del_buffer(un_buffer);
			int tamanio_a_escribir = extraer_int_del_buffer(un_buffer);

			if (tamanio_a_escribir == 1)
			{
				uint8_t dato8 = extraer_uint8_del_buffer(un_buffer);
				log_debug(cpu_log_debug, "PID: %d - Acción: ESCRIBIR - Dirección Física: %d - Valor: %u\n", pcb_ejecucion.pid, direccion_fisica, dato8);
			}
			else
			{
				uint32_t dato32 = extraer_uint32_del_buffer(un_buffer);
				log_debug(cpu_log_debug, "PID: %d - Acción: ESCRIBIR - Dirección Física: %d - Valor: %u\n", pcb_ejecucion.pid, direccion_fisica, dato32);
			}

		    //printf("A memoria le llegó lo que tenía que escribir.\n");
			sem_post(&esperarEscrituraDeMemoria);
			
			break;
		case LECTURA_HECHA:

			un_buffer = recibir_todo_el_buffer(fd_memoria);

			int dirFisicaDelDato = extraer_int_del_buffer(un_buffer);
			char* registroDatos = extraer_string_del_buffer(un_buffer);
			void *direccionRegistroDatos = (void *)get_registry(registroDatos);
			int tamanio = extraer_int_del_buffer(un_buffer);

			if(tamanio == 1){
				uint8_t dato8 = extraer_uint32_del_buffer(un_buffer);
				memcpy(direccionRegistroDatos,&dato8,1);
				log_debug(cpu_log_debug, "PID: %d - Acción: LEER - Dirección Física: %d - Valor: %u\n", pcb_ejecucion.pid, dirFisicaDelDato, dato8);
			}
			else{
                uint32_t dato32 = extraer_uint32_del_buffer(un_buffer);
				memcpy(direccionRegistroDatos,&dato32,4);
				log_debug(cpu_log_debug, "PID: %d - Acción: LEER - Dirección Física: %d - Valor: %u\n", pcb_ejecucion.pid, dirFisicaDelDato, dato32);
			}

		    printf("Ya leyo el MOV_IN.\n");

			sem_post(&esperarLecturaDeMemoria);
			
			break;						
		case RECIBIR_MARCO:

			un_buffer = recibir_todo_el_buffer(fd_memoria);
			marco = extraer_int_del_buffer(un_buffer);
		    //printf("llego marquitos a cpu %d\n", marco);

			sem_post(&esperarMarco);

			break;
		case CANT_INTRUCCIONES:
		    un_buffer = recibir_todo_el_buffer(fd_memoria);
			cantInstucciones = extraer_int_del_buffer(un_buffer);
			sem_post(&wait_instruccion);
		    break;
		case TERMINO_INSTRUCCION_MEMORIA:
	        un_buffer = recibir_todo_el_buffer(fd_memoria);
			int xd = extraer_int_del_buffer(un_buffer);	
			sem_post(&wait_instruccion);
		    break;
		case RECIBIR_CARACTER:
		    un_buffer = recibir_todo_el_buffer(fd_memoria);
			caracterGlobal = extraer_uint8_del_buffer(un_buffer);
			sem_post(&esperar_lectura_caracter);
			break;
		case MANDAR_LECTURA_DE_STRING:
		    un_buffer = recibir_todo_el_buffer(fd_memoria);
			stringLeido = extraer_string_del_buffer(un_buffer);
			sem_post(&esperarLecturaDeString);
			break;
		case OUT_OF_MEMORY:
		    un_buffer = recibir_todo_el_buffer(fd_memoria);
			int valorRandom = extraer_int_del_buffer(un_buffer);
			log_error(cpu_log_debug, "OUT OF MEMORY");
			terminePorOutOfMemory = true;
			sem_post(&wait_instruccion);
			break;
		case HABILITAR_PID:
		    un_buffer = recibir_todo_el_buffer(fd_memoria);
			int *pid_habilita = malloc(sizeof(int));
			*pid_habilita = extraer_int_del_buffer(un_buffer);
			list_add(procesosConPath,pid_habilita);
		    break;
		case -1:
			log_trace(cpu_log_debug, "Desconexion de CPU - MEMORIA");
			control_key = 0;
			break;
		default:
			log_warning(logger,"Operacion desconocida de CPU - MEMORIA");
			break;
		}
	}
}


