#include "utils/utils_server.h"
#include "utils/utils_cliente.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <pthread.h>


void *iniciar_conexion(void *ptr);
void *crear_servidor(void *ptr);

typedef struct {
	char *ip;
	char *puerto;
	char *ruta_interna;
} datos_conexion;


int main(void) {


//CLIENTE SIN HILO CPU ==> Memoria 

	datos_conexion info_conexion;

	info_conexion.ip = "IP_MEMORIA";
	info_conexion.puerto = "PUERTO_MEMORIA";
	info_conexion.ruta_interna = "/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/cpu/cpu.config";	
	
	iniciar_conexion((void*) &info_conexion);

//SERVER SIN HILO KERNEL ==> CPU

	datos_conexion_server info_conexion_server;
	t_config* config = iniciar_config("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/cpu/cpu.config");

	info_conexion_server.nombreCliente = "KERNEL";
	info_conexion_server.ip = config_get_string_value(config,"IP_KERNEL");
	info_conexion_server.puerto = config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");

	crear_servidor((void*) &info_conexion_server);

	return 0;
}
