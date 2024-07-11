#include "../include/cpu_kernel_dispatch.h"
#include <utils/shared.h>
#include "../include/funciones_cpu.h"


/*PCB *atender_recibir_pcb(t_buffer *un_buffer)
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

	pcb_ejecucion = inicializar_PCB(pid, program_counter, quantum, ax, bx, cx, dx, eax, ebx, ecx, edx, si, di);

	//printf("el pid es: %d\n", pid);
	printf("el pid del pcb_ejecucion: %d\n",pcb_ejecucion.pid);
}*/


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
		case ENVIAR_PID:
		    un_buffer = recibir_todo_el_buffer(fd_kernel_dispatch);
			int pid = extraer_int_del_buffer(un_buffer);
			procesar_instruccion(pid);
			break;
		case RECIBIR_PCB:
		    un_buffer = recibir_todo_el_buffer(fd_kernel_dispatch);
			PCB* pcb = atender_recibir_pcb(un_buffer);
			pcb_ejecucion = *pcb;
			procesar_instruccion();
		    break;
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