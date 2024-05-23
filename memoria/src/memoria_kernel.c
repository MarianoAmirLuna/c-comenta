#include "../include/memoria_kernel.h"
#include <pthread.h>

path_conID* iniciar_path_id(int id, char* path){
	path_conID* estructura = malloc(sizeof(path_conID));
	estructura->id = id;
	estructura->path = path;

	return estructura;
}

void atender_memoria_kernel()
{
	bool control_key = 1;
	t_buffer *un_buffer;
	while (control_key)
	{
		int cod_op = recibir_operacion(fd_kernel); 
		switch (cod_op)
		{
		case MENSAJE:

			break;
		case PAQUETE:

			break;
		case CREAR_PROCESO_KM:
		    printf("llego\n");
			un_buffer = recibir_todo_el_buffer(fd_kernel);
			atender_crear_proceso(un_buffer);
			break;
		case -1:
			log_trace(memoria_log_debug, "Desconexion de KERNEL");
			control_key = 0;
			break;

		default:
			//log_warning(logger,"Operacion desconocida de KERNEL");
			break;
		}
	}
}

void atender_crear_proceso(t_buffer* un_buffer){
	int pid = extraer_int_del_buffer(un_buffer);
	char* path = extraer_string_del_buffer(un_buffer);

    path_conID* path_con_id = iniciar_path_id(pid, path);

	list_add(list_path_id,path_con_id);

	//cantidad_lineas_txt_ejecutando = contarLineas(path);

	//printf("cantidad de lineas txt: %d\n",cantidad_lineas_txt_ejecutando);
	
	//printf("justo antes del post");
	//sem_post(&sem_llego_instruccion);
	//printf("el valor del semaforo es: %d\n",sem_llego_instruccion.__align);
	//printf("pase el post\n");

	//    return 0;
}
	//printf("pase el post\n");



	//path_conID* xd = list_get(list_path_id,0);

	//printf("el recien cargado es: %s\n",xd->path);
    
	//ya confirmamos que la lista esta bien cargada por printf
	

