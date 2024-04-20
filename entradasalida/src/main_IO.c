#include <stdlib.h>
#include <stdio.h>
#include "utils/utils_cliente.h"
#include <pthread.h>

void *crear_servidor(void *ptr);


int main(){

//CLIENTE SIN HILO I/O ==> MEMORIA

	datos_conexion info_conexion;

	info_conexion.ip = "IP_MEMORIA";
	info_conexion.puerto = "PUERTO_MEMORIA";
	info_conexion.ruta_interna = "/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/entradasalida/entradasalida.config";	
	
	iniciar_conexion((void*) &info_conexion);


//CLIENTE SIN HILO I/O ==> KERNEL
	
	datos_conexion info_conexion2;

	info_conexion2.ip = "IP_KERNEL";
	info_conexion2.puerto = "PUERTO_KERNEL";
	info_conexion2.ruta_interna = "/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/entradasalida/entradasalida.config";	
	
	iniciar_conexion((void*) &info_conexion2);

	return 0;
}




