//#include <stdlib.h>
//#include <stdio.h>
//#include <commons/config.h>
//#include <pthread.h>
//#include "utils/shared.h"
//#include <commons/log.h>
#include "../include/funciones_kernel.h"
#include "../include/servicios_kernel.h"


PCB iniciar_PCB()
{ // revisar si anda o hay que poner struct adelante

  t_config *config = iniciar_configuracion("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/kernel/kernel.config");

  PCB pcb;

  pcb.pid = asignar_pid();
  pcb.program_counter = 1;
  pcb.quantum = config_get_int_value(config, "QUANTUM");
  //pcb.pathTXT = path;
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

  //printf("El numero del pcb es: ");
  //printf("%d\n", pcb.pid);

  return pcb;
}

void enviar_path_memoria(char *path, int pid)
{
  t_buffer *a_enviar = crear_buffer();

  a_enviar->size = 0;
  a_enviar->stream = NULL;

  cargar_int_al_buffer(a_enviar, pid);
  cargar_string_al_buffer(a_enviar, path);

  t_paquete *un_paquete = crear_super_paquete(CREAR_PROCESO_KM, a_enviar);
  enviar_paquete(un_paquete, fd_memoria);
  destruir_paquete(un_paquete);
}

void enviar_pcb(PCB pcb, int socket_enviar)
{
  t_buffer *a_enviar = crear_buffer();

  a_enviar->size = 0;
  a_enviar->stream = NULL;

  cargar_int_al_buffer(a_enviar, pcb.pid);
  cargar_int_al_buffer(a_enviar, pcb.program_counter);
  cargar_int_al_buffer(a_enviar, pcb.quantum);
  cargar_uint8_al_buffer(a_enviar, pcb.registros_cpu.AX);
  cargar_uint8_al_buffer(a_enviar, pcb.registros_cpu.BX);
  cargar_uint8_al_buffer(a_enviar, pcb.registros_cpu.CX);
  cargar_uint8_al_buffer(a_enviar, pcb.registros_cpu.DX);
  cargar_uint32_al_buffer(a_enviar, pcb.registros_cpu.EAX);
  cargar_uint32_al_buffer(a_enviar, pcb.registros_cpu.EBX);
  cargar_uint32_al_buffer(a_enviar, pcb.registros_cpu.ECX);
  cargar_uint32_al_buffer(a_enviar, pcb.registros_cpu.EDX);
  cargar_uint32_al_buffer(a_enviar, pcb.registros_cpu.SI);
  cargar_uint32_al_buffer(a_enviar, pcb.registros_cpu.DI);

  t_paquete *un_paquete = crear_super_paquete(RECIBIR_PCB, a_enviar);
  enviar_paquete(un_paquete, socket_enviar);
  destruir_paquete(un_paquete);
}


void enviar_pid_a_cpu(int pid){
  t_buffer *buffer_pid = crear_buffer();
  buffer_pid->size = 0;
  buffer_pid->stream = NULL;
  cargar_int_al_buffer(buffer_pid, pid);
  t_paquete *paquete_pid = crear_super_paquete(ENVIAR_PID, buffer_pid);
  enviar_paquete(paquete_pid, fd_cpu_dispatch);
  destruir_paquete(paquete_pid);
}

void planificacion(){
  while(seguirPlanificando){
    sem_wait(&sem_cpu_libre);
    ciclo_planificacion();
    if(ejecutandoProceso){
      enviar_pid_a_cpu(procesoEXEC);
    }
    else sem_post(&sem_cpu_libre);
  }
}

void imprimirLista(t_list* l){
  int i=0;
  for(int i=0;i<list_size(l);i++){
    int *elem = list_get(l, i);
    printf("%d, ", *elem);
  }
}

void estadoPlani(){
  //int *new0=list_get(procesosREADY, 0);
  //void *ready0=list_get(procesosREADY, 1);
  //printf("New: %d, Ready:%d, exec:%d", new0, ready0, procesoEXEC);
  //printf("ready: %d, exec:%d\n",*new0, procesoEXEC);
  printf("ready: ");
  imprimirLista(procesosREADY);
  printf(" exec: %d \n", procesoEXEC);

}

void iniciar_planificacion(){
  //sleep(2);
  //printf("llega adentro de iniciarPlani\n");
  procesosNEW=list_create();
  //printf("entrando a ciclo plani\n");
  procesosREADY=list_create();
  procesoEXEC=0;
  int uno=1, dos=2, tres=3;
  //list_add(procesosNEW, &uno);
  //list_add(procesosNEW, &dos);
  //printf("entran 1 y 2 a new en ese orden------------------------------------\n");
  
  //sleep(1);
  /*ciclo_planificacion();
  estadoPlani();
  list_add(procesosNEW, &tres);
  printf("entra 3 a new-----------------------------------------\n");
  ciclo_planificacion();
  estadoPlani();

  procesoEXEC=0;
  printf("termina el proceso 1--------------------------------------\n");
  ciclo_planificacion();
  estadoPlani();
  printf("termina el proceso 2------------------------------\n");
  procesoEXEC=0;
  ciclo_planificacion();
  estadoPlani();*/
  list_add(procesosNEW, &uno);
  list_add(procesosNEW, &dos);
  list_add(procesosNEW, &tres);
  ciclo_planificacion();
  estadoPlani();
  ciclo_planificacion();
  estadoPlani();
  ciclo_planificacion();
  estadoPlani();
  ciclo_planificacion();
  estadoPlani();
  ciclo_planificacion();
  estadoPlani();
  ciclo_planificacion();
  estadoPlani();
  ciclo_planificacion();
  estadoPlani();
  ciclo_planificacion();
  estadoPlani();
  ciclo_planificacion();
  estadoPlani();
  ciclo_planificacion();
  estadoPlani();


  /*
  pthread_t hilo_ciclo_planificacion;
  pthread_create(&hilo_ciclo_planificacion, NULL, (void*)planificacion, NULL);
  pthread_detach(hilo_ciclo_planificacion);*/
}



void ciclo_plani_FIFO(){
  while(!list_is_empty(procesosNEW) && list_size(procesosREADY)<GRADO_MULTIPROGRAMACION){ //si entró un nuevo proceso y todavia no tengo el ready al maximo, lo mando
    list_add(procesosREADY, list_remove(procesosNEW, 0));
  } 
  //printf("limpiada lista de new\n");
  if (procesoEXEC==0 && !list_is_empty(procesosREADY)) //si no hay ningun proceso en ejecucion, pone el primero de READY
  {
    pthread_mutex_lock(&mutexExec);
    int* exec = list_remove(procesosREADY,0);
    procesoEXEC= *exec; 
    pthread_mutex_unlock(&mutexExec); 
  }
  if(procesoEXEC==0) ejecutandoProceso=0;
  else ejecutandoProceso=1;
}

void ciclo_plani_RR(){
  //quantum++;
  if(tiempoTranscurrido>=quantum){
    printf("FIN DE QUANTUM\n");
    tiempoTranscurrido=0;
    int *procesoDesalojado = malloc(sizeof(int)); 
    *procesoDesalojado = procesoEXEC;
    list_add(procesosREADY, procesoDesalojado);
    pthread_mutex_lock(&mutexExec);
    procesoEXEC=0;
    pthread_mutex_unlock(&mutexExec);
  }
  while(!list_is_empty(procesosNEW) && list_size(procesosREADY)<GRADO_MULTIPROGRAMACION){ //si entró un nuevo proceso y todavia no tengo el ready al maximo, lo mando
    list_add(procesosREADY, list_remove(procesosNEW, 0));
  } 
  if(procesoEXEC==0){
    pthread_mutex_lock(&mutexExec);
    int* exec = list_remove(procesosREADY,0);
    procesoEXEC= *exec; 
    pthread_mutex_unlock(&mutexExec);
  }
  if(procesoEXEC==0) ejecutandoProceso=0;
  else ejecutandoProceso=1;
  tiempoTranscurrido++;
}

void ciclo_planificacion(){
  switch(tipoPlanificacion){
    case FIFO:
    printf("CICLO FIFO\n");
      ciclo_plani_FIFO();
      break;
    case RR:
      ciclo_plani_RR();
    default:
      break;
  }
}


void iniciar_proceso(char *path)
{
  PCB pcb = iniciar_PCB(path);
  enviar_path_memoria(path, pcb.pid);
  enviar_pcb(pcb, fd_cpu_dispatch);
  list_add(procesosNEW, &(pcb.pid));
}


void iniciar_cpu()
{
  t_buffer *a_enviar = crear_buffer();


  a_enviar->size = 0;
  a_enviar->stream = NULL;

  cargar_string_al_buffer(a_enviar, "iniciar_cpu");

  t_paquete *un_paquete = crear_super_paquete(INICIAR_CPU, a_enviar);
  enviar_paquete(un_paquete, fd_cpu_dispatch);
  destruir_paquete(un_paquete);
}
