#include "../include/cpu_kernel_dispatch.h"
#include <utils/shared.h>
#include "../include/funciones_cpu.h"

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
			PCB *pcb = atender_recibir_pcb(un_buffer);
			pcb_ejecucion = *pcb;
			numeroID_hilo = extraer_int_del_buffer(un_buffer);
			procesar_instruccion();        
			break;
		case -1:
			log_info(cpu_logger, "Desconexion de KERNEL - Dispatch");
			control_key = 0;
			break;
		default:
			log_warning(logger, "Operacion desconocida de KERNEL - Dispatch");
			break;
		}
	}
}