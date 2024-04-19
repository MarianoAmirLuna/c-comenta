#include "utils/utils_server.h"
#include "utils/utils_cliente.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>


int main(void) {

	t_config* config = iniciar_config("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/cpu/cpu.config");
	char* ip_kernel = config_get_string_value(config,"IP_KERNEL");
	char* puerto_kernel = config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");

	crear_servidor("kernel",ip_kernel,puerto_kernel);

	return 0;
}
