#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <../include/IO.h>

int main(){

	inicializar_io();

	iniciar_conexion(PUERTO_MEMORIA, "MEMORIA");

	iniciar_conexion(PUERTO_KERNEL, "KERNEL");
	
	return 0;
}




