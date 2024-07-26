#include "../include/kernel.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <pthread.h>
#include "utils/shared.h"
#include <commons/log.h>
#include "../include/funciones_kernel.h"
#include "../include/consola.h"

int main(int argc, char** argv)
{
	//recibe mensajes del interrupt / dispatch / io / memoria
	lista_interfaces = list_create();

	if (argc < 2) {
        fprintf(stderr, "Uso: %s <ruta_al_archivo_de_configuracion>\n", argv[0]);
        return EXIT_FAILURE;
    }

	inicializar_kernel(argv[1]);
	fd_kernel = iniciar_servidor(PUERTO_ESCUCHA);

    fd_memoria = iniciar_conexion(IP_MEMORIA, PUERTO_MEMORIA, "MEMORIA",kernel_log_debug);

	fd_cpu_interrupt = iniciar_conexion(IP_CPU,PUERTO_CPU_INTERRUPT, "CPU-INTERRUPT",kernel_log_debug);
	fd_cpu_dispatch = iniciar_conexion(IP_CPU, PUERTO_CPU_DISPATCH, "CPU-DISPATCH",kernel_log_debug);

	pthread_t hilo_planificacion;
	pthread_create(&hilo_planificacion, NULL, (void*)iniciar_planificacion, NULL);
	pthread_detach(hilo_planificacion);

    pthread_t hilo_planificacion_io;
	pthread_create(&hilo_planificacion_io, NULL, (void*)iniciar_planificacion_io, NULL);
	pthread_detach(hilo_planificacion_io);

	/*pthread_t hilo_temporizador;
	pthread_create(&hilo_temporizador, NULL, (void*)temporizadorQuantum, NULL);
	pthread_detach(hilo_temporizador);*/

	pthread_t hilo_cpu_interrupt;
	pthread_create(&hilo_cpu_interrupt, NULL, (void*)atender_kernel_interrupt, NULL);
	pthread_detach(hilo_cpu_interrupt);

	pthread_t hilo_cpu_dispatch;
	pthread_create(&hilo_cpu_dispatch, NULL, (void*)atender_kernel_dispatch, NULL);
	pthread_detach(hilo_cpu_dispatch);
	
	pthread_t hilo_consola;
	pthread_create(&hilo_consola, NULL, (void*)iniciar_consola_interactiva, NULL);
	pthread_detach(hilo_consola);

	while(1) {

        printf("entre al while\n");

        int* socket_cliente = malloc(sizeof(int));

        if (socket_cliente == NULL) {
            perror("Error en malloc()");
            return EXIT_FAILURE;
        }

		printf("un instante antes del accept\n");
        *socket_cliente = accept(fd_kernel, NULL, NULL); //en el momento que recibe una nueva conexion por parte de io pasa el accept y crea un nuevo hilo
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

//iniciar servidor => retorna socket servidor
//esperar cliente => retorna socket cliente
//iniciar conexion / crear conexion => retorna socket cliente

