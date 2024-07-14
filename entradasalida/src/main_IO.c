#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <../include/IO.h>
#include <../include/inicializar_io.h>
#include <readline/readline.h>
#include <string.h>

t_config*  crearConfig(char* direccion){

	t_config* configDevolver = config_create(direccion); //esto te pide la ruta del config

	if ( configDevolver == NULL)	
	{
		perror("Hay un error al iniciar el config.");
		exit(EXIT_FAILURE);
	}

	return configDevolver;
}


void ejecutarInterfazGenerica(char* nombre,t_config* config_interface){
	log_info(io_logger,"Iniciando interfaz Generica"); 
	int TIEMPO_UNIDAD_TRABAJOO = config_get_int_value(config_interface, "TIEMPO_UNIDAD_TRABAJO"); //por default es 250, osea, 4 unidades de tiempo equivalen a un 1 segundo
	int Unidades_De_Trabajo = 4;

	log_info(io_logger,"Iniciando sleep");
	usleep(Unidades_De_Trabajo*(TIEMPO_UNIDAD_TRABAJOO*1000));//esta funcion anda con microsegundos, por eso se multiplican por 1000, asi cada unidad de trabajo equivale a un milisegundo
	log_info(io_logger,"Fin de sleep");

	log_trace(io_log_debug, "Fin de Interfaz Generica");


}

void escribirEnMemoria(int direccionLogica, char* texto, int tamanio){

	t_buffer *buffer = crear_buffer();
    buffer->size = 0;
    buffer->stream = NULL;

	cargar_int_al_buffer(buffer,direccionLogica);
	cargar_int_al_buffer(buffer,tamanio);
	cargar_string_al_buffer(buffer,texto);

    t_paquete *paquete = crear_super_paquete(ESCRIBIR_IO_MEMORIA, buffer);
    enviar_paquete(paquete, fd_memoria);
    destruir_paquete(paquete);
}

void ejecutarInterfazSTDIN(char* nombre,t_config* config_interface){
	log_info(io_logger,"Iniciando interfaz STDIN");

		char*  textoAEscribir;
		log_info(io_logger,"Escriba el texto que desea guardar");
		textoAEscribir = readline(">");

	log_info(io_logger,"El texto a guardar es %s",textoAEscribir);
	
	free(textoAEscribir);
}


void ejecutarInterfazSTDOUT(char* nombre,t_config* config_interface){

	log_info(io_logger,"Iniciando interfaz STDOUT");

	char* textoLeido;

	textoLeido="Texto de prueba";
	
	log_info(io_logger,"El texto leido es %s",textoLeido);

}



void iniciarInterfaz(char* nombre_Interface,char* direccion_Config){

	log_info(io_logger,"Incializando interfaz"); 

	t_config* config_interface = crearConfig(direccion_Config);

	char* TIPO_INTERFAZZ = config_get_string_value(config_interface, "TIPO_INTERFAZ");

	if (strcmp(TIPO_INTERFAZZ, "GENERICA")==0) {
		log_info(io_logger,"La interfaz a iniciar es del tipo Generica"); 
		ejecutarInterfazGenerica(nombre_Interface, config_interface);
	}else if (strcmp(TIPO_INTERFAZZ, "STDIN")==0){
		log_info(io_logger,"La interfaz a iniciar es del tipo STDIN"); 
		ejecutarInterfazSTDIN(nombre_Interface, config_interface);
	}else if (strcmp(TIPO_INTERFAZZ, "STDOUT")==0){
		log_info(io_logger,"La interfaz a iniciar es del tipo STDOUT");  
		ejecutarInterfazSTDOUT(nombre_Interface, config_interface);
	}else if (strcmp(TIPO_INTERFAZZ, "DialFS")==0){
		log_info(io_logger,"La interfaz a iniciar es del tipo DialFS"); 
		log_error(io_log_debug,"Esta interfaz no se encuentra implementada");
	}else{
		log_error(io_log_debug, "No se pudo identificar correctamente el tipo de interfaz");
	}

}



int main(){

	inicializar_io();


	log_info(io_logger,"Inicializando Entrada/Salida");


	
	fd_memoria = iniciar_conexion(PUERTO_MEMORIA, "MEMORIA",io_log_debug);
	
	fd_kernel = iniciar_conexion(PUERTO_KERNEL, "KERNEL",io_log_debug);


	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_io_kernel, NULL);
	pthread_detach(hilo_kernel);

	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_io_memoria, NULL);
	pthread_join(hilo_memoria, NULL);
	


	char* nombreInterACrear;

	do{


		log_info(io_logger,"Escriba el nombre de la interfaz");
		nombreInterACrear = readline(">");

		if(string_is_empty(nombreInterACrear)){
			log_warning(io_log_debug,"Una interfaz no puede tener el nombre vacio");
		}
	}while(string_is_empty(nombreInterACrear));
	
	log_info(io_logger,"El nombre elegido es %s",nombreInterACrear);


	char* direccionConfigInterCrear;

	do{

		log_info(io_logger,"Escriba la direccion del archivo de configuracion");
		direccionConfigInterCrear = readline(">");

		if(string_is_empty(direccionConfigInterCrear)){
			log_warning(io_log_debug,"Una direccion no puede ser vacia");
		}

	}while(string_is_empty(direccionConfigInterCrear));

	log_info(io_logger,"La direccion elegida es %s",nombreInterACrear); //TODO: ¿Se deberia considerar la posibilidad de que en esa direccion no haya archivo de configuracion?
	
	free(direccionConfigInterCrear);

	direccionConfigInterCrear="/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/entradasalida/entradasalida.config"; //TODO: borrar este hardcodeo de la direccion del config
	
	iniciarInterfaz(direccionConfigInterCrear, direccionConfigInterCrear);
	
	//free(direccionConfigInterCrear);
	free(nombreInterACrear);


	log_info(io_logger,"Fin de Entrada/Salida");
	
	return 0;
}