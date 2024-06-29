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

			printf("La instruccion es: %s\n",instruccion_actual);

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

			int basura = extraer_int_del_buffer(un_buffer);

		    printf("A memoria le llegó lo que tenía que escribir.\n");

			sem_post(&esperarEscrituraDeMemoria);
			
			break;
		case LECTURA_HECHA:

			un_buffer = recibir_todo_el_buffer(fd_memoria);

			int marianos = extraer_int_del_buffer(un_buffer);

		    printf("Ya leyo.\n");

			sem_post(&esperarLecturaDeMemoria);
			
			break;						
		case RECIBIR_MARCO:

			un_buffer = recibir_todo_el_buffer(fd_memoria);

			marco = extraer_int_del_buffer(un_buffer);

		    printf("llego marquitos a cpu %d\n", marco);

			sem_post(&esperarMarco);

			break;
		case ESCRIBIO_PRIMERA_PARTE:
		    un_buffer = recibir_todo_el_buffer(fd_memoria);

			int pepe = extraer_int_del_buffer(un_buffer);

			printf("se libero el semaforo que habilita escribir la segunda parte\n");

			sem_post(&mandarSegundaDF);

			break;
		case LEYO_PRIMERA_PARTE:
		    un_buffer = recibir_todo_el_buffer(fd_memoria);

			int marianete = extraer_int_del_buffer(un_buffer);

			printf("se libero el semaforo que habilita leer la segunda parte\n");

			sem_post(&mandarSegundaDFALeer);

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


