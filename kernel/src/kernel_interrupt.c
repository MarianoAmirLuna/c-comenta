#include "../include/kernel_interrupt.h"
#include <utils/shared.h>

void atender_kernel_interrupt(){
	t_buffer *un_buffer;
    bool control_key = 1;
	while (control_key) {
		int cod_op = recibir_operacion(fd_cpu_interrupt); 
		switch (cod_op) {
		case MENSAJE:
			//
			break;
		case PAQUETE:
            //
			break;
		case -1:
			log_trace(kernel_log_debug, "Desconexion de KERNEL - Interrupt");
			control_key = 0;
			break;
		default:
			log_warning(logger,"Operacion desconocida de KERNEL - Interrupt");
			break;
		}
	}
}