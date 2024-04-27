#include "../include/cpu_kernel_dispatch.h"
#include <utils/shared.h>

void atender_cpu_kernel_dispatch(){
    bool control_key = 1;
	while (control_key) {
		int cod_op = recibir_operacion(fd_kernel_dispatch); 
		switch (cod_op) {
		case MENSAJE:
			//
			break;
		case PAQUETE:
            //
			break;
		case -1:
			log_error(logger, "Desconexion de KERNEL - Dispatch");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida de KERNEL - Dispatch");
			break;
		}
	}
}