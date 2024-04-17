#include <stdlib.h>
#include <stdio.h>
//#include <utils/hello.h>
#include "main.h"

int main(){

	iniciar_conexion("IP_KERNEL","PUERTO_KERNEL","/home/utnso/Desktop/tp_operativos/tp-2024-1c-Granizado/entradasalida/entradasalida.config");

	return 0;
}


void iniciar_conexion(char *nombreIp, char *puertoIp,char *rutaConexion){
	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

    logger = iniciar_logger();

	log_info(logger,"Soy un Log");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config(rutaConexion);

	valor = config_get_string_value(config, "CLAVE");
	ip = config_get_string_value(config, nombreIp);
	puerto = config_get_string_value(config, puertoIp);

	//log_info(logger,ip_kernel);
	//log_info(logger, puerto_kernel);

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

    
	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip,puerto); //crear conexion te retorna el socket

	enviar_mensaje(valor,conexion);
}

t_log* iniciar_logger(void)
{
	
	t_log* nuevo_logger = log_create("cliente.log","", 1 ,LOG_LEVEL_INFO);

	if(nuevo_logger == NULL){
		perror ("Hay un error al iniciar el log.");
	}

	return nuevo_logger;
}

t_config* iniciar_config(char *rutaConexion)
{
	t_config* nuevo_config = config_create(rutaConexion); //esto te pide la ruta del config

	if ( nuevo_config == NULL)	
	{
		perror("Hay un error al iniciar el config.");
	}
	
	return nuevo_config;

}

void leer_consola(t_log* logger)
{
	// La primera te la dejo de yapa
	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
    char *linea;
	linea = readline(">");
	log_info(logger, ">> %s", linea);

    while (strcmp(linea,"") != 0) {
		free(linea);
        linea = readline(">");
		log_info(logger, ">> %s", linea);
    }
	// ¡No te olvides de liberar las lineas antes de regresar!

}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido = NULL;
	t_paquete* paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete

	leido = readline(">");
	while (strcmp(leido ,"") != 0) {
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		free(leido);
		leido = readline("> ");
    }

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	free(leido);

	enviar_paquete(paquete,conexion);
	
	eliminar_paquete(paquete);
	
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
}
