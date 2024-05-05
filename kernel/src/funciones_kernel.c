#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <pthread.h>
#include "utils/shared.h"
#include <commons/log.h>
#include "../include/funciones_kernel.h"
#include "../include/servicios_kernel.h"

PCB iniciar_PCB(){ //revisar si anda o hay que poner struct adelante

  t_config* config = iniciar_configuracion("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/kernel/kernel.config");

  PCB pcb;

  pcb.pid = asignar_pid();
  pcb.program_counter = 0;
  pcb.quantum = config_get_int_value(config,"QUANTUM");

  return pcb;
}

void enviar_path_memoria(char* path, int pid){
  t_buffer* a_enviar = crear_buffer();
  
  cargar_int_al_buffer(a_enviar, pid);
  cargar_string_al_buffer(a_enviar, path);

  t_paquete* un_paquete = crear_super_paquete(CREAR_PROCESO_KM, a_enviar);
  enviar_paquete(un_paquete, fd_memoria);
  destruir_paquete(un_paquete);

}

void iniciar_proceso(char* path){
  
  PCB pcb = iniciar_PCB();
  enviar_path_memoria(path,pcb.pid);
}



