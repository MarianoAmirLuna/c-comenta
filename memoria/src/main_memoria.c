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

	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_memoria_kernel, NULL);
	pthread_detach(hilo_kernel);	
	
	//Atender los mensajes del CPU
	pthread_t hilo_cpu;
	pthread_create(&hilo_cpu, NULL, (void*)atender_memoria_cpu, NULL);
	pthread_detach(hilo_cpu);
	

	//Atender los mensajes del IO
	pthread_t hilo_io;
	pthread_create(&hilo_io, NULL, (void*)atender_memoria_io, NULL);
	pthread_join(hilo_io, NULL);
	

	return 0;
}

/*
void atender_proceso(t_buffer* un_buffer){
  int pid = extraer_int_del_buffer(un_buffer);
  char* path = extraer_string_del_buffer(un_buffer);

  free(path);
}
*/