#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <../include/IO.h>
#include <../include/inicializar_io.h>

int main(){

	inicializar_io();

	iniciar_conexion(PUERTO_MEMORIA, "MEMORIA",io_log_debug);

	iniciar_conexion(PUERTO_KERNEL, "KERNEL",io_log_debug);
	
	return 0;
}




