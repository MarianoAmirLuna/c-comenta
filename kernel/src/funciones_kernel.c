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

  printf("El numero del pcb es: ");
  printf("%d\n",pcb.pid);

  return pcb;
}


void enviar_path_memoria(char* path, int pid){
  t_buffer* a_enviar = crear_buffer();

  a_enviar->size = 0;
  a_enviar->stream = NULL;
  
  cargar_int_al_buffer(a_enviar, pid);
  cargar_string_al_buffer(a_enviar, path);

  t_paquete* un_paquete = crear_super_paquete(CREAR_PROCESO_KM, a_enviar);
  enviar_paquete(un_paquete, fd_memoria);
  destruir_paquete(un_paquete);

}

void iniciar_proceso(char* path){
  
  PCB pcb = iniciar_PCB();
  enviar_path_memoria(path,pcb.pid);
  list_add(procesosNEW, &(pcb.pid));
}

void iniciar_planificacion(){
  procesosNEW=list_create();
  procesosREADY=list_create();
  procesoEXEC=0;
}

void ciclo_planificacion(){
  switch(tipoPlanificacion){
    case FIFO:
      ciclo_plani_FIFO();
      break;
    default:
    break;
  }
}

void ciclo_plani_FIFO(){
  wait(sem_planificacion);
  while(!list_is_empty(procesosNEW) && list_size(procesosREADY)<GRADO_MULTIPROGRAMACION){ //si entró un nuevo proceso y todavia no tengo el ready al maximo, lo mando
    list_add(procesosREADY, list_remove(procesosNEW, 0));
  } 
  if (procesoEXEC==0) //si no hay ningun proceso en ejecucion, pone el primero de READY
  {
    pthread_mutex_t *mutexExec;
    pthread_mutex_lock(mutexExec);
    procesoEXEC=list_remove(procesosREADY, 0); 
    pthread_mutex_unlock(mutexExec); 
  }
  signal(sem_planificacion)
}


