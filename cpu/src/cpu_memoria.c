#include "../include/cpu_memoria.h"
#include <utils/shared.h>

PCB inicializar_PCB(int pid, int program_counter, int quantum, uint8_t ax, uint8_t bx, uint8_t cx,
					uint8_t dx, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t si, uint32_t di)
{

	PCB pcb;

	pcb.pid = pid;
	pcb.program_counter = program_counter;
	pcb.quantum = quantum;
	pcb.registros_cpu.AX = ax;
	pcb.registros_cpu.BX = bx;
	pcb.registros_cpu.CX = cx;
	pcb.registros_cpu.DX = dx;
	pcb.registros_cpu.EAX = eax;
	pcb.registros_cpu.EBX = ebx;
	pcb.registros_cpu.ECX = ecx;
	pcb.registros_cpu.EDX = edx;
	pcb.registros_cpu.SI = si;
	pcb.registros_cpu.DI = di;

	return pcb;

} // recibe todos los registros por eso es tan largo

PCB extraer_pcb(t_buffer* un_buffer){

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

	PCB pcb = inicializar_PCB(pid, program_counter, quantum, ax, bx, cx, dx, eax, ebx, ecx, edx, si, di);

	return pcb;
}

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
		case RECIBIR_MARCO:

			un_buffer = recibir_todo_el_buffer(fd_memoria);

			marco = extraer_int_del_buffer(un_buffer);

		    printf("llego marquitos a cpu %d\n", marco);

			sem_post(&esperarMarco);
			
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


