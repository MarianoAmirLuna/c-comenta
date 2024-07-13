#include "../include/kernel_dispatch.h"
#include <utils/shared.h>
#include "../include/funciones_kernel.h"

void atender_kernel_dispatch(){
    bool control_key = 1;
	t_buffer *un_buffer;
	while (control_key) {
		int cod_op = recibir_operacion(fd_cpu_dispatch); 
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
			estaCPULibre = true;
			sem_post(&esperar_devolucion_pcb);

			if(codigo == 1){// si hay cambio de contexto envio un 1 osea fue desalojado => le faltan instrucciones por ejecutar
				list_add(listaPCBs,pcb_devuelto);
			    list_add(procesosREADY,&(pcb_devuelto->pid));
			}
            else{ //ejecuto todas las instrucciones
				//significa que termino todas sus intrucciones y tengo que liberar los recursos
				finalizarProceso(pcb_devuelto->pid);
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