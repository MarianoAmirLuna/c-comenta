#include "utils/utils_cliente.h"
#include "utils/utils_server.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h> 
#include <pthread.h>

int *crear_servidor(void *ptr);

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
	pthread_join(hilo1,NULL);
	


//SERVIDOR CON HILO Kernel ==> Memoria
    pthread_t hilo2;
	int iret2;

	datos_conexion_server info_conexion_server2;
	config = iniciar_config("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/memoria/memoria.config");

	info_conexion_server2.nombreCliente = "KERNEL";
	info_conexion_server2.ip = config_get_string_value(config,"IP_KERNEL");
	info_conexion_server2.puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
	

    iret2 = pthread_create(&hilo2, NULL,crear_servidor, (void*) &info_conexion_server2);
	pthread_join(hilo2,NULL);
	
//SERVIDOR CON HILO I/O ==> Memoria
	pthread_t hilo3;
	int iret3;

	datos_conexion_server info_conexion_server3;
	config = iniciar_config("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/memoria/memoria.config");

	info_conexion_server3.nombreCliente = "Entrada-salida";
	info_conexion_server3.ip = config_get_string_value(config,"IP_IO");
	info_conexion_server3.puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
	

    iret3 = pthread_create(&hilo3, NULL,crear_servidor, (void*) &info_conexion_server3);
	pthread_join(hilo3,NULL);
	


	return 0;
}

