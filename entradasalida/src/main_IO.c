#include <stdlib.h>
#include <stdio.h>
#include "utils/utils_cliente.h"
#include <pthread.h>

int main(){

	//iniciar_conexion("IP_KERNEL","PUERTO_KERNEL","/home/utnso/Desktop/tp_operativos/tp-2024-1c-Granizado/entradasalida/entradasalida.config");
	//iniciar_conexion("IP_MEMORIA","PUERTO_MEMORIA","/home/utnso/Desktop/tp_operativos/tp-2024-1c-Granizado/entradasalida/entradasalida.config");

    pthread_t hilo1;
	int iret1;

	datos_conexion info_conexion;

	info_conexion.ip = "IP_KERNEL";
	info_conexion.puerto = "PUERTO_KERNEL";
	info_conexion.ruta_interna = "/home/utnso/Desktop/tp_operativos/tp-2024-1c-Granizado/entradasalida/entradasalida.config";

    iret1 = pthread_create(&hilo1, NULL, iniciar_conexion, (void *) &info_conexion);
	pthread_join(hilo1,NULL);


	return 0;
}




