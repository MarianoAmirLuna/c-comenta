#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <../include/IO.h>
#include <../include/inicializar_io.h>

void ejecutarInterfazGenerica(char* nombre,t_config* config_interface){
	log_info(io_logger,"Iniciando interfaz Generica"); 
	int TIEMPO_UNIDAD_TRABAJOO = config_get_int_value(config_interface, "TIEMPO_UNIDAD_TRABAJO"); //por default es 250, osea, 4 unidades de tiempo equivalen a un 1 segundo
	int Unidades_De_Trabajo = 4;

	log_info(io_logger,"Iniciando sleep");
	usleep(Unidades_De_Trabajo*(TIEMPO_UNIDAD_TRABAJOO*1000));//esta funcion anda con microsegundos, por eso se multiplican por 1000, asi cada unidad de trabajo equivale a un milisegundo
	log_info(io_logger,"Fin de sleep");

	log_trace(io_log_debug, "Fin de Interfaz Generica");


}

void iniciarInterfaz(char* nombre_Interface,t_config* config_interface){
	log_info(io_logger,"Incializando interfaz"); 
	char* TIPO_INTERFAZZ = config_get_string_value(config_interface, "TIPO_INTERFAZ");
	if (strcmp(TIPO_INTERFAZZ, "GENERICA")==0) {
		log_info(io_logger,"La interfaz a iniciar es del tipo Generica"); 
		ejecutarInterfazGenerica(nombre_Interface, config_interface);
	}else if (strcmp(TIPO_INTERFAZZ, "STDIN")==0){
		log_info(io_logger,"La interfaz a iniciar es del tipo STDIN"); 
		log_error(io_log_debug,"Esta interfaz no se encuentra implementada");
	}else if (strcmp(TIPO_INTERFAZZ, "STDOUT")==0){
		log_info(io_logger,"La interfaz a iniciar es del tipo STDOUT"); 
		log_error(io_log_debug,"Esta interfaz no se encuentra implementada");
	}else if (strcmp(TIPO_INTERFAZZ, "DialFS")==0){
		log_info(io_logger,"La interfaz a iniciar es del tipo DialFS"); 
		log_error(io_log_debug,"Esta interfaz no se encuentra implementada");
	}else{
		log_error(io_log_debug, "No se pudo identificar correctamente el tipo de interfaz");
	}

}



int main(){

	inicializar_io();

	
	fd_memoria = iniciar_conexion(PUERTO_MEMORIA, "MEMORIA",io_log_debug);
	
	fd_kernel = iniciar_conexion(PUERTO_KERNEL, "KERNEL",io_log_debug);


	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_io_kernel, NULL);
	pthread_detach(hilo_kernel);

	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_io_memoria, NULL);
	pthread_join(hilo_memoria, NULL);
	

	log_info(io_logger,"Eligiendo interfaz a iniciar");

	char* nombreInterACrear = "int1";

	iniciarInterfaz(nombreInterACrear, io_config);



	log_info(io_logger,"Fin de Entrada/Salida");
	
	return 0;
}

//iniciar servidor => retorna socket servidor
//esperar cliente => retorna socket cliente
//iniciar conexion / crear conexion => retorna socket cliente




