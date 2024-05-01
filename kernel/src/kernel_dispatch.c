#include "../include/kernel_dispatch.h"
#include <utils/shared.h>

void atender_kernel_dispatch(){
    bool control_key = 1;
	while (control_key) {
		int cod_op = recibir_operacion(fd_cpu_dispatch); 
		switch (cod_op) {
		case MENSAJE:
			//
			break;
		case PAQUETE:
            //
			break;
		case -1:
			log_trace(kernel_log_debug, "Desconexion de KERNEL - Dispatch");
			control_key = 0;
			break;
		default:
			log_warning(logger,"Operacion desconocida de KERNEL - Dispatch");
			break;
		}
	}
}