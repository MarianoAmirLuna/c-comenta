#include <../include/memoria.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <pthread.h>
#include "utils/shared.h"
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv) {

	/*if (argc < 2) {
        fprintf(stderr, "Uso: %s <ruta_al_archivo_de_configuracion>\n", argv[0]);
        return EXIT_FAILURE;
    }*/

	inicializar_memoria(argv[1]);
	
	fd_memoria = iniciar_servidor(PUERTO_ESCUCHA);
   
	log_trace(memoria_log_debug, "listo para escuchar al CPU");
	
	fd_cpu = esperar_cliente(fd_memoria);  

	log_trace(memoria_log_debug, "listo para escuchar al KERNEL");
	fd_kernel = esperar_cliente(fd_memoria);

	//Atender los mensajes del kernel

	pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_memoria_kernel, NULL);
	pthread_detach(hilo_kernel);	
	
	//Atender los mensajes del CPU
	pthread_t hilo_cpu;
	pthread_create(&hilo_cpu, NULL, (void*)atender_memoria_cpu, NULL);
	pthread_detach(hilo_cpu);

	while(1) {

        printf("entre al while\n");

        int* socket_cliente = malloc(sizeof(int));

        if (socket_cliente == NULL) {
            perror("Error en malloc()");
            return EXIT_FAILURE;
        }

		printf("un instante antes del accept\n");
        *socket_cliente = accept(fd_memoria, NULL, NULL); //en el momento que recibe una nueva conexion por parte de io pasa el accept y crea un nuevo hilo
		printf("un instante despues del accept\n");

        printf("el socket del cliente es: %d\n",*socket_cliente);

        if (*socket_cliente < 0) {
            perror("Error en accept()");
            free(socket_cliente); // Libera la memoria en caso de error
            continue;
        }

        pthread_t thread;
        pthread_create(&thread, NULL, (void*)atender_creacion_interfaz, socket_cliente);
        pthread_detach(thread);
    }

	return 0;

}

