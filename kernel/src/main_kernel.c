#include "utils/utils_server.h"
#include "utils/utils_cliente.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>


int main(void) {

	t_config* config = iniciar_config("/home/utnso/Desktop/tp_operativos1/tp-2024-1c-Granizado/kernel/kernel.config");
	char* ip_IO = config_get_string_value(config,"IP_IO");
	char* puerto_IO = config_get_string_value(config,"PUERTO_ESCUCHA");

	crear_servidor("entrada-salida",ip_IO,puerto_IO);

	//iniciar_conexion("IP_CPU","PUERTO_CPU_DISPATCH","/home/utnso/Desktop/tp_operativos1/tp-2024-1c-Granizado/kernel/kernel.config");

	return 0;
}

