#include "../include/memoria_io.h"
#include <utils/shared.h>

void atender_memoria_io(){
    bool control_key = 1;
	t_buffer *un_buffer;

	while (control_key) {
		int cod_op = recibir_operacion(fd_io); 
		switch (cod_op) {
		case MENSAJE:
			//
			break;
		case PAQUETE:
            //
			break;
		case ESCRIBIR_IO_MEMORIA:
		    un_buffer = recibir_todo_el_buffer(fd_io);
	        int direccionLogica = extraer_int_del_buffer(un_buffer);
			int tamanio = extraer_int_del_buffer(un_buffer);
			char* texto = extraer_string_del_buffer(un_buffer);

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