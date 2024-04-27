#include "../include/cpu_memoria.h"

void atender_cpu_kernel_dispatch(){
    bool control_key = 1;
	while (control_key) {
		int cod_op = recibir_operacion(fd_memoria); 
		switch (cod_op) {
		case MENSAJE:
			//
			break;
		case PAQUETE:
            //
			break;
		case -1:
			log_error(logger, "Desconexion de CPU - MEMORIA");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida de CPU - MEMORIA");
			break;
		}
	}
}