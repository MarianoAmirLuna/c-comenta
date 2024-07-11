#include "../include/kernel_dispatch.h"
#include <utils/shared.h>

void atender_kernel_dispatch(){
    bool control_key = 1;
	t_buffer *un_buffer;
	while (control_key) {
		int cod_op = recibir_operacion(fd_cpu_dispatch); 
		printf("codigo de operacion: %d\n",cod_op);
		switch (cod_op) {
		case MENSAJE:
			//
			break;
		case PAQUETE:
            //
			break;
		case CONSULTA_PLANIFICACION:
			ciclo_planificacion();
			t_paquete *paquete_pid = crear_paquete();
			agregar_a_paquete(paquete_pid, procesoEXEC, sizeof(int));
			enviar_paquete(paquete_pid, fd_cpu_dispatch);
			destruir_paquete(paquete_pid);
			break;
		case RECIBIR_PCB:
			un_buffer = recibir_todo_el_buffer(fd_cpu_dispatch);
			PCB *pcb_devuelto = atender_recibir_pcb(un_buffer);
			list_add(listaPCBs, pcb_devuelto);
			int codigo = extraer_int_del_buffer(un_buffer);

            if(codigo == 2){ //le faltan instrucciones por ejecutar
				//significa que termino todas sus intrucciones y tengo que liberar los recursos
			}

			if(procesoEXEC != 0){
				mandarNuevoPCB();
			}
			else{
				primeraVezEjecuta = true;
			}

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