#include "../include/cpu_kernel_interrupt.h"
#include <utils/shared.h>

void atender_cpu_kernel_interrupt(){
    bool control_key = 1;
	t_buffer *un_buffer;
	while (control_key) {
		int cod_op = recibir_operacion(fd_kernel_interrupt); 
		switch (cod_op) {
		case MENSAJE:
			//
			break;
		case PAQUETE:
            //
			break;
		case AVISO_DESALOJO:
		    un_buffer = recibir_todo_el_buffer(fd_kernel_interrupt);
			int pid = extraer_int_del_buffer(un_buffer);
			cambioContexto = true;
			printf("FUI DESALOJADOOOOOOOOOOOOOOOOOOOOOOOOOOOO F\n");
		    break;
		case -1:
			log_trace(cpu_log_debug, "Desconexion de KERNEL - Interrupt");
			control_key = 0;
			break;
		default:
			log_warning(logger,"Operacion desconocida de KERNEL - Interrupt");
			break;
		}
	}
}