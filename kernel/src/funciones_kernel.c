// #include <stdlib.h>
// #include <stdio.h>
// #include <commons/config.h>
// #include <pthread.h>
// #include "utils/shared.h"
// #include <commons/log.h>
#include "../include/funciones_kernel.h"
#include "../include/servicios_kernel.h"

PCB iniciar_PCB()
{ // revisar si anda o hay que poner struct adelante

  t_config *config = iniciar_configuracion("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/kernel/kernel.config");

  PCB pcb;

  pcb.pid = asignar_pid();
  pcb.program_counter = 1;
  pcb.quantum = config_get_int_value(config, "QUANTUM");
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

void iniciar_planificacion()
{
  procesosNEW = list_create();
  procesosREADY = list_create();
  procesoEXEC = 0;
}

void ciclo_plani_FIFO()
{
  // wait(sem_planificacion);//CORREGIR
  while (!list_is_empty(procesosNEW) && list_size(procesosREADY) < GRADO_MULTIPROGRAMACION)
  { // si entró un nuevo proceso y todavia no tengo el ready al maximo, lo mando
    list_add(procesosREADY, list_remove(procesosNEW, 0));
  }
  if (procesoEXEC == 0) // si no hay ningun proceso en ejecucion, pone el primero de READY
  {
    pthread_mutex_lock(&mutexExec);
    procesoEXEC = list_remove(procesosREADY, 0);
    pthread_mutex_unlock(&mutexExec);
  }
  // signal(sem_planificacion)//CORREGIR
}

void ciclo_plani_RR()
{
  quantum++;
  if (tiempoTranscurrido >= quantum)
  {
    int procesoDesalojado = procesoEXEC;
    pthread_mutex_lock(&mutexExec);
    procesoEXEC = 0;
    pthread_mutex_unlock(&mutexExec);
  }
  while (!list_is_empty(procesosNEW) && list_size(procesosREADY) < GRADO_MULTIPROGRAMACION)
  { // si entró un nuevo proceso y todavia no tengo el ready al maximo, lo mando
    list_add(procesosREADY, list_remove(procesosNEW, 0));
  }
  if (procesoEXEC == 0)
  {
    pthread_mutex_lock(&mutexExec);
    procesoEXEC = list_remove(procesosREADY, 0);
    pthread_mutex_unlock(&mutexExec);
  }
}

void ciclo_planificacion()
{
  switch (tipoPlanificacion)
  {
  case FIFO:
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
  PCB pcb = iniciar_PCB();
  enviar_path_memoria(path, pcb.pid);
  enviar_pcb(pcb, fd_memoria);
  list_add(procesosNEW, &(pcb.pid));
}

void enviar_pid_cpu(int pid)
{
  t_buffer *a_enviar = crear_buffer();

  a_enviar->size = 0;
  a_enviar->stream = NULL;

  cargar_int_al_buffer(a_enviar,pid);

  t_paquete *un_paquete = crear_super_paquete(EJECUTAR_INSTRUCCION, a_enviar);
  enviar_paquete(un_paquete, fd_cpu_dispatch);
  destruir_paquete(un_paquete);
}

void iniciar_cpu()
{
  sem_wait(&esta_cpu_libre);
  enviar_pid_cpu(1);
  printf("se solicito el pid 1\n");
  sem_wait(&esta_cpu_libre);
  enviar_pid_cpu(3);
  printf("se solicito el pid 3\n");
  sem_wait(&esta_cpu_libre);
  enviar_pid_cpu(2);
  printf("se solicito el pid 2\n");
}
