#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <../include/IO.h>
#include <../include/inicializar_io.h>


void EjecutarInterfazGenerica(char* nombre,t_config* config_interface){
	char* TIPO_INTERFAZZ = config_get_string_value(config_interface, "TIPO_INTERFAZ");
	int TIEMPO_UNIDAD_TRABAJOO = config_get_int_value(config_interface, "TIEMPO_UNIDAD_TRABAJO"); //por default es 250, osea, 4 unidades de tiempo equivalen a un 1 segundo

	log_info(io_logger,"Noto que estas intentando crear un interfaz del tipo %s",TIPO_INTERFAZZ); //deberia decir del tipo GENERICA

	log_info(io_logger,"Buenas noches es hora de dormirse");
	//sleep(20);//Aca se podria usar usar la variable TIEMPO_UNIDAD_TRABAJOO de alguna forma
	usleep(TIEMPO_UNIDAD_TRABAJOO*1000);//esta funcion anda con microsegundo, por eso se multiplican por 1000
	log_info(io_logger,"Ya desperte del sueño");


}

int main(){

	inicializar_io();

	log_info(io_logger,"Hora del rock! Decime que tipo de interfaz inicializa el I/O");

	int interfazACrear = 1; //cada numero vendria a representar una interfaz generica, esto me parece que conviene cambiarlo a futuro, tal vez un enum o algo

	switch (interfazACrear) {
		case 1:
			EjecutarInterfazGenerica("interfaz1",io_config);
			break;
		case 2:
            log_info(io_logger,"Por cual motivo pondrias un 2 aca? XD");
			break;
		case -1:
			log_trace(io_logger, "Mepa que ocurrio un bug o algo");
			break;
		default:
			log_warning(logger,"PARA EMOCION!!! esa interfaz todavia no la implemente");
			break;
		}

	log_info(io_logger,"Acabo de salir del switch");


	
	/*
	fd_memoria = iniciar_conexion(PUERTO_MEMORIA, "MEMORIA",io_log_debug);

	fd_kernel = iniciar_conexion(PUERTO_KERNEL, "KERNEL",io_log_debug);


	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_io_kernel, NULL);
	pthread_detach(hilo_kernel);

	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_io_memoria, NULL);
	pthread_join(hilo_memoria, NULL);
	*/
	
	return 0;
}

//iniciar servidor => retorna socket servidor
//esperar cliente => retorna socket cliente
//iniciar conexion / crear conexion => retorna socket cliente




