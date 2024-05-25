#include "../include/cpu_kernel_dispatch.h"
#include <utils/shared.h>

PCB inicializar_PCB(int pid, int program_counter, int quantum,char* path, uint8_t ax, uint8_t bx, uint8_t cx,
					uint8_t dx, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t si, uint32_t di)
{

	PCB pcb;

	pcb.pid = pid;
	pcb.program_counter = program_counter;
	pcb.quantum = quantum;
	pcb.pathTXT = path;
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

void atender_cpu_kernel_dispatch()
{
	bool control_key = 1;
	t_buffer *un_buffer;
	while (control_key)
	{
		int cod_op = recibir_operacion(fd_kernel_dispatch);
		switch (cod_op)
		{
		case MENSAJE:

			break;
		case PAQUETE:

			break;
		case RECIBIR_PCB:
			printf("llego el pcb a cpu\n");
			un_buffer = recibir_todo_el_buffer(fd_kernel_dispatch);
			atender_recibir_pcb(un_buffer);
			break;
		case INICIAR_CPU:
		    printf("iniciando el cpu...\n");
			un_buffer = recibir_todo_el_buffer(fd_kernel_dispatch);
			extraer_string_del_buffer(un_buffer);

			sem_post(&arrancar_cpu);
		case -1:
			log_trace(cpu_log_debug, "Desconexion de KERNEL - Dispatch");
			control_key = 0;
			break;
		default:
			log_warning(logger, "Operacion desconocida de KERNEL - Dispatch");
			break;
		}
	}
}

void atender_recibir_pcb(t_buffer *un_buffer)
{
	int pid = extraer_int_del_buffer(un_buffer);
	int program_counter = extraer_int_del_buffer(un_buffer);
	int quantum = extraer_int_del_buffer(un_buffer);
	char* path = extraer_string_del_buffer(un_buffer);
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

	pcb_ejecucion = inicializar_PCB(pid, program_counter, quantum, path, ax, bx, cx, dx, eax, ebx, ecx, edx, si, di);

	//printf("el pid es: %d\n", pid);
	printf("el pid del pcb_ejecucion: %d\n",pcb_ejecucion.pid);
}
