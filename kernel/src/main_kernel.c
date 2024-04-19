#include "utils/utils_server.h"
#include "utils/utils_cliente.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <pthread.h>
void *iniciar_conexion(void *ptr);

typedef struct {
	char *ip;
	char *puerto;
	char *ruta_interna;
} datos_conexion;

int main(void) {

	//SERVIDOR 
	
	t_config* config = iniciar_config("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/kernel/kernel.config");
	char* ip_IO = config_get_string_value(config,"IP_IO");
	char* puerto_IO = config_get_string_value(config,"PUERTO_ESCUCHA");

	crear_servidor("entrada-salida",ip_IO,puerto_IO);
	


	//CLIENTE

    pthread_t hilo1;
	int iret1;

	datos_conexion info_conexion;

	info_conexion.ip = "IP_CPU";
	info_conexion.puerto = "PUERTO_CPU_DISPATCH";
	info_conexion.ruta_interna = "/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/kernel/kernel.config";

    iret1 = pthread_create(&hilo1, NULL,iniciar_conexion, (void*) &info_conexion);
	pthread_join(hilo1,NULL);

	


	return 0;
}

