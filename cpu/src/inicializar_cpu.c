#include "../include/inicializar_cpu.h"
#include <utils/shared.h>

void inicializar_cpu(){
    iniciar_logs();
    iniciar_config_cpu();
    imprimir_config();
	iniciar_semaforos();
	iniciar_variables();
}

void iniciar_logs(){
	cpu_logger = log_create("cliente.log","", 1 ,LOG_LEVEL_INFO);

	if(cpu_logger == NULL){
		perror ("Hay un error al iniciar el log.");
		exit(EXIT_FAILURE);
	}


	cpu_log_debug = log_create("cliente.log","", 1 ,LOG_LEVEL_TRACE);

	if(cpu_log_debug == NULL){
		perror ("Hay un error al iniciar el log.");
		exit(EXIT_FAILURE);
	}
}
void iniciar_config_cpu(){
    cpu_config = config_create("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/cpu/cpu.config"); //esto te pide la ruta del config

	if ( cpu_config == NULL)	
	{
		perror("Hay un error al iniciar el config.");
		exit(EXIT_FAILURE);
	}

	IP_MEMORIA = config_get_string_value(cpu_config, "IP_MEMORIA");
	PUERTO_MEMORIA = config_get_string_value(cpu_config, "PUERTO_MEMORIA");
	PUERTO_ESCUCHA_DISPATCH = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_DISPATCH");
	PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(cpu_config, "PUERTO_ESCUCHA_INTERRUPT");

}
void imprimir_config(){
    log_info(cpu_logger, "IP_MEMORIA: %s", IP_MEMORIA);
	log_warning(cpu_log_debug, "PUERTO_MEMORIA: %s", PUERTO_MEMORIA);
	log_debug(cpu_log_debug, "PUERTO_ESCUCHA_DISPATCH: %s", PUERTO_ESCUCHA_DISPATCH);
	log_trace(cpu_log_debug, "PUERTO_ESCUCHA_INTERRUPT: %s", PUERTO_ESCUCHA_INTERRUPT);
}

void iniciar_semaforos(){
	sem_init(&arrancar_cpu, 1, 0);
	sem_init(&wait_instruccion, 1, 0);
	sem_init(&pcb_actualizado,1,0);
	sem_init(&esperarTamanioDePagina,1,0);
	sem_init(&esperarMarco,1,0);
	sem_init(&esperarEscrituraDeMemoria,1,0);
}

PCB iniciar_PCB()
{ 
  PCB pcb;

  pcb.pid = 0;
  pcb.program_counter = 1;
  pcb.quantum = 3;
  pcb.registros_cpu.AX = 0;
  pcb.registros_cpu.BX = 0;
  pcb.registros_cpu.CX = 0;
  pcb.registros_cpu.DX = 0;
  pcb.registros_cpu.EAX = 0;
  pcb.registros_cpu.EBX = 0;
  pcb.registros_cpu.ECX = 0;
  pcb.registros_cpu.EDX = 0;
  pcb.registros_cpu.SI = 0;
  pcb.registros_cpu.DI = 0;

  return pcb;
}


void iniciar_variables(){
	hayPcbEjecucion = false;
	pcb_ejecucion.pid = -1; //el -1 significa que aun no llego ni una instruccion a cpu
	pcb_ejecucion = iniciar_PCB();
	primeraSolicitudTamanioDePagina = true;
}
