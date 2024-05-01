#include "../include/memoria_io.h"
#include <utils/shared.h>

void atender_memoria_io(){
    bool control_key = 1;
	while (control_key) {
		int cod_op = recibir_operacion(fd_io); 
		switch (cod_op) {
		case MENSAJE:
			//
			break;
		case PAQUETE:
            //
			break;
		case -1:
			log_trace(memoria_log_debug, "Desconexion de IO - MEMORIA");
			control_key = 0;
			break;
		default:
			log_warning(logger,"Operacion desconocida de IO - MEMORIA");
			break;
		}
	}
}