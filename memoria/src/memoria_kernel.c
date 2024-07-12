#include "../include/memoria_kernel.h"
#include <pthread.h>

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
	tablaPaginas tabla = inicializarTablaPaginas(pid);

	list_add(list_path_id, path_con_id);
	list_add(listaTablaPaginas, &tabla);

	usleep(RETARDO_RESPUESTA * 1000);
}

void atender_eliminar_proceso(t_buffer *un_buffer)
{
	int pid = extraer_int_del_buffer(un_buffer);
	tablaPaginas* tablaABorrar = obtener_tabla_pagina(pid);
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

/*
void atender_recibir_pcb(t_buffer *un_buffer)
{
	int pid = extraer_int_del_buffer(un_buffer);
	int program_counter = extraer_int_del_buffer(un_buffer);
	int quantum = extraer_int_del_buffer(un_buffer);
	uint8_t ax = extraer_uint8_del_buffer(un_buffer);
	uint8_t bx = extraer_uint8_del_buffer(un_buffer);
	uint8_t cx = extraer_uint8_del_buffer(un_buffer);
	uint8_t dx = extraer_uint8_del_buffer(un_buffer);
	uint32_t eax = extraer_uint32_del_buffer(un_buffer);
	uint32_t ebx = extraer_uint32_del_buffer(un_buffer);
	uint32_t ecx = extraer_uint32_del_buffer(un_buffer);
	uint32_t edx = extraer_uint32_del_buffer(un_buffer);
	uint32_t si = extraer_uint32_del_buffer(un_buffer);
	uint32_t di = extraer_uint32_del_buffer(un_buffer);

	PCB* pcb = inicializar_PCB(pid, program_counter, quantum, ax, bx, cx, dx, eax, ebx, ecx, edx, si, di);
}
*/

/*
PCB* inicializar_PCB(int pid, int program_counter, int quantum, uint8_t ax, uint8_t bx, uint8_t cx,
					uint8_t dx, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t si, uint32_t di)
{

	PCB* pcb = malloc(sizeof(PCB));

	pcb->pid = pid;
	pcb->program_counter = program_counter;
	pcb->quantum = quantum;
	pcb->registros_cpu.AX = ax;
	pcb->registros_cpu.BX = bx;
	pcb->registros_cpu.CX = cx;
	pcb->registros_cpu.DX = dx;
	pcb->registros_cpu.EAX = eax;
	pcb->registros_cpu.EBX = ebx;
	pcb->registros_cpu.ECX = ecx;
	pcb->registros_cpu.EDX = edx;
	pcb->registros_cpu.SI = si;
	pcb->registros_cpu.DI = di;

	return pcb;

} // recibe todos los registros por eso es tan largo
*/
