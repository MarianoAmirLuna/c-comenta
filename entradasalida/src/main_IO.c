#include <stdlib.h>
#include <stdio.h>
#include "utils/utils_cliente.h"

int main(){

	iniciar_conexion("IP_KERNEL","PUERTO_KERNEL","/home/utnso/Desktop/tp_operativos/tp-2024-1c-Granizado/entradasalida/entradasalida.config");
	iniciar_conexion("IP_MEMORIA","PUERTO_MEMORIA","/home/utnso/Desktop/tp_operativos/tp-2024-1c-Granizado/entradasalida/entradasalida.config");

	return 0;
}




