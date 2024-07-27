#include "../include/inicializar_kernel.h"
#include "../include/funciones_kernel.h"

#include <stdlib.h>
#include <stdio.h>
#include "utils/shared.h"
#include <commons/log.h>
#include <commons/config.h>

void inicializar_logs()
{
	kernel_logger = log_create("kernel.log", "", 1, LOG_LEVEL_INFO);

	if (kernel_logger == NULL)
	{
		perror("Hay un error al iniciar el log.");
		exit(EXIT_FAILURE);
	}

	kernel_log_debug = log_create("kernel.log", "", 1, LOG_LEVEL_TRACE);

	if (kernel_log_debug == NULL)
	{
		perror("Hay un error al iniciar el log.");
		exit(EXIT_FAILURE);
	}
}

void inicializar_configs(char* rutaconfig)
{
	//kernel_config = config_create(rutaconfig); //DES-COMENTAR PARA ENTREGA
	kernel_config = config_create("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/kernel/deadlock.config");

	if (kernel_config == NULL)
	{
		perror("Hay un error al iniciar el config.");
		exit(EXIT_FAILURE);
	}
	

	PUERTO_ESCUCHA = config_get_string_value(kernel_config, "PUERTO_ESCUCHA");
	IP_MEMORIA = config_get_string_value(kernel_config, "IP_MEMORIA");
	PUERTO_MEMORIA = config_get_string_value(kernel_config, "PUERTO_MEMORIA");
	IP_CPU = config_get_string_value(kernel_config, "IP_CPU");
	PUERTO_CPU_DISPATCH = config_get_string_value(kernel_config, "PUERTO_CPU_DISPATCH");
	PUERTO_CPU_INTERRUPT = config_get_string_value(kernel_config, "PUERTO_CPU_INTERRUPT");
	ALGORITMO_PLANIFICACION = config_get_string_value(kernel_config, "ALGORITMO_PLANIFICACION");
	QUANTUM = config_get_int_value(kernel_config, "QUANTUM");
	RECURSOS = config_get_string_value(kernel_config, "RECURSOS");
	INSTANCIAS_RECURSOS = config_get_string_value(kernel_config, "INSTANCIAS_RECURSOS");
	GRADO_MULTIPROGRAMACION = config_get_int_value(kernel_config, "GRADO_MULTIPROGRAMACION");
}

void imprimir_configs()
{
	log_debug(kernel_log_debug, "PUERTO_ESCUCHA: %s", PUERTO_ESCUCHA);
	log_debug(kernel_log_debug, "PUERTO_CPU_DISPATCH: %s", PUERTO_CPU_DISPATCH);
	log_debug(kernel_log_debug, "RECURSOS: %s", RECURSOS);
	log_debug(kernel_log_debug, "QUANTUM: %d", QUANTUM);
}

void iniciar_semaforos()
{
	sem_init(&sem_cpu_libre, 1, 1);
	sem_init(&esperar_devolucion_pcb, 1, 0);
	sem_init(&sem_seguir_planificando, 1, 1);
	sem_init(&esperar_carga_path_memoria, 1, 0);
	sem_init(&nuevo_bucle,1,0);
	sem_init(&contador_q,1,0);
	sem_init(&ciclo_instruccion_io,1,0);
	sem_init(&esperar_vuelva,1,0);
	sem_init(&esperar_termine_ejecutar_pcb_cpu,1,1);

	listaPCBs = list_create();
	listaPidsRecursos = list_create();
	int flagCambioProceso = 0;
	procesosNEW = list_create();
	procesosREADY = list_create();
	listQPrimas = list_create();
	procesosSuspendidos = list_create();
	procesosEXIT = list_create();
    lista_id_hilos = list_create();
	procesos_READY_priori = list_create();

	
}

void iniciar_recursos()
{
	nombresRecursos = config_get_array_value(kernel_config, "RECURSOS");
	char **STR_InstanciasRecursos = config_get_array_value(kernel_config, "INSTANCIAS_RECURSOS");
	instanciasRecursos = list_create();
	for (int i = 0; STR_InstanciasRecursos[i] != NULL; i++) // mira el primer caracter de cada string
	{
		cantidad_de_recursos++;
		int *instancia = malloc(sizeof(int));
		*instancia = atoi(STR_InstanciasRecursos[i]);
		list_add(instanciasRecursos, instancia);
		// printf("Recurso: %s, cantidad: %d \n", nombresRecursos[i], *instancia);
	}
	lista_recursos_y_bloqueados = list_create();

	for (int i = 0; nombresRecursos[i]!=NULL; i++)
	{
		t_list *new_list = list_create();
		list_add(lista_recursos_y_bloqueados, new_list);
	}
	estado_instancias();
}

void inicializar_kernel(char* rutaConfig)
{
	inicializar_logs();
	inicializar_configs(rutaConfig);
	imprimir_configs();
	// iniciar_planificacion();
	iniciar_semaforos();
	iniciar_recursos();
}