#include <../include/memoria.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <pthread.h>
#include "utils/shared.h"
#include <commons/log.h>



int main() {

	inicializar_memoria();	
	
	fd_memoria = iniciar_servidor(PUERTO_ESCUCHA);
   
	log_trace(memoria_log_debug, "listo para escuchar al CPU");
	
	fd_cpu = esperar_cliente(fd_memoria);  

	log_trace(memoria_log_debug, "listo para escuchar al KERNEL");
	fd_kernel = esperar_cliente(fd_memoria);

	log_trace(memoria_log_debug, "listo para escuchar al IO");
	fd_io = esperar_cliente(fd_memoria);

	//Atender los mensajes del kernel
	atender_memoria_kernel();
	
	//Atender los mensajes del CPU
	atender_memoria_cpu();

	//Atender los mensajes del IO
	atender_memoria_io();

	return 0;
}