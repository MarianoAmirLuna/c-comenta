#include "../include/inicializar_memoria.h"

void iniciar_listas(){
	list_path_id = list_create();
	list_pcb = list_create();
	listaTablaPaginas = list_create();
	listaPaginas = list_create();
	lista_interfaces = list_create();
}

void iniciar_logs(){
    
	memoria_logger = log_create("memoria.log", "", 1, LOG_LEVEL_INFO);

	if (memoria_logger == NULL)
	{
		perror("Hay un error al iniciar el log.");
		exit(EXIT_FAILURE);
	}

	memoria_log_debug = log_create("memoria.log", "", 1, LOG_LEVEL_TRACE);

	if (memoria_log_debug == NULL)
	{
		perror("Hay un error al iniciar el log.");
		exit(EXIT_FAILURE);
	}

}

void iniciar_config(char* rutaconfig){
	//memoria_config = config_create(rutaconfig);

	memoria_config = config_create("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/memoria/fs.config");

	if (memoria_config == NULL)
	{
		perror("Hay un error al iniciar el config.");
		exit(EXIT_FAILURE);
	}

	PUERTO_ESCUCHA = config_get_string_value(memoria_config,"PUERTO_ESCUCHA");
    TAM_MEMORIA = config_get_int_value(memoria_config,"TAM_MEMORIA");
    TAM_PAGINA = config_get_int_value(memoria_config,"TAM_PAGINA");
    PATH_INSTRUCCIONES = config_get_string_value(memoria_config,"PATH_INSTRUCCIONES");
    RETARDO_RESPUESTA = config_get_int_value(memoria_config,"RETARDO_RESPUESTA");
}

void imprimir_config(){    	
	log_info(memoria_log_debug, "PUERTO_ESCUCHA: %s", PUERTO_ESCUCHA);
	log_info(memoria_log_debug, "TAM_MEMORIA: %d", TAM_MEMORIA);
	log_info(memoria_log_debug, "TAM_PAGINA: %d", TAM_PAGINA);
	log_info(memoria_log_debug, "RETARDO_RESPUESTA: %d", RETARDO_RESPUESTA);
}

void iniciar_semaforos(){
    sem_init(&esperar_df,1,0);
}

void inicializar_memoria(char* rutaconfig){
	iniciar_listas();
    iniciar_logs();
    iniciar_config(rutaconfig);
    imprimir_config();
	iniciarPaginacion();
	iniciar_semaforos();
}