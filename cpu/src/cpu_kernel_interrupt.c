#include "../include/cpu_kernel_interrupt.h"

void atender_cpu_kernel_dispatch(){
    bool control_key = 1;
	while (control_key) {
		int cod_op = recibir_operacion(fd_kernel_interrupt); 
		switch (cod_op) {
		case MENSAJE:
			//
			break;
		case PAQUETE:
            //
			break;
		case -1:
			log_error(logger, "Desconexion de KERNEL - Interrupt");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida de KERNEL - Interrupt);
			break;
		}
	}
}