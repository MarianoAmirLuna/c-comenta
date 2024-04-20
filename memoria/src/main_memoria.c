#include "utils/utils_cliente.h"
#include "utils/utils_server.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h> 
#include <pthread.h>

void *crear_servidor(void *ptr);

typedef struct {
	char *nombreCliente;
	char *ip;
	char *puerto;
} datos_conexion_server;


int main(void) {
	//Levantar servidor sin usar hilos
/*
	t_config* config = iniciar_config("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/memoria/memoria.config");
	char* ip_IO = config_get_string_value(config,"IP_IO");
	char* puerto_IO = config_get_string_value(config,"PUERTO_ESCUCHA");

	crear_servidor("entrada-salida",ip_IO,puerto_IO);
*/


//SERVER CON HILO CPU ==> MEMORIA

    pthread_t hilo1;
	int iret1;

	datos_conexion_server info_conexion_server;
	t_config* config = iniciar_config("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/memoria/memoria.config");

	info_conexion_server.nombreCliente = "CPU";
	info_conexion_server.ip = config_get_string_value(config,"IP_CPU");
	info_conexion_server.puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
	

    iret1 = pthread_create(&hilo1, NULL,crear_servidor, (void*) &info_conexion_server);
	pthread_detach(hilo1);
	

//SERVIDOR CON HILO Kernel ==> Memoria

    pthread_t hilo2;
	int iret2;

	datos_conexion_server info_conexion_server2;
	config = iniciar_config("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/memoria/memoria.config");

	info_conexion_server2.nombreCliente = "KERNEL";
	info_conexion_server2.ip = config_get_string_value(config,"IP_KERNEL");
	info_conexion_server2.puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
	

    iret2 = pthread_create(&hilo2, NULL,crear_servidor, (void*) &info_conexion_server2);
	pthread_detach(hilo2);
	
//SERVIDOR SIN HILO I/O ==> Memoria

	datos_conexion_server info_conexion_server3;
	config = iniciar_config("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/memoria/memoria.config");

	info_conexion_server3.nombreCliente = "Entrada-salida";
	info_conexion_server3.ip = config_get_string_value(config,"IP_IO");
	info_conexion_server3.puerto = config_get_string_value(config,"PUERTO_ESCUCHA");

    crear_servidor((void*) &info_conexion_server3);

	return 0;
}

