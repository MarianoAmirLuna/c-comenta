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

    //CLIENTE SIN HILO KERNEL ==> CPU

	datos_conexion info_conexion;

	info_conexion.ip = "IP_CPU";
	info_conexion.puerto = "PUERTO_CPU_DISPATCH";
	info_conexion.ruta_interna = "/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/kernel/kernel.config";	
	
	iniciar_conexion((void*) &info_conexion);
	
	//CLIENTE SIN HILO Kernel ==> Memoria
	
	datos_conexion info_conexion2;

	info_conexion2.ip = "IP_MEMORIA";
	info_conexion2.puerto = "PUERTO_MEMORIA";
	info_conexion2.ruta_interna = "/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/kernel/kernel.config";	


	iniciar_conexion((void*) &info_conexion2);

    //SERVIDOR SIN HILO I/O ==>Kernel

	datos_conexion_server info_conexion_server;
	t_config* config = iniciar_config("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/kernel/kernel.config");

	info_conexion_server.nombreCliente = "Entrada-salida";
	info_conexion_server.ip = config_get_string_value(config,"IP_IO");
	info_conexion_server.puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
	
	crear_servidor((void*) &info_conexion_server);

	return 0;
}

