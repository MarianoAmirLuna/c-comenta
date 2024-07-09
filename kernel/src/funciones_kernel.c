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

pidConQ *nuevoPidConQ(int pid){
  pidConQ* ret = malloc(sizeof(pidConQ));
  ret->pid=pid;
  ret->qPrima=quantum;
  return ret;
}

bool pidIgualAlGlobal(void* pid){
  int *aux = (int*)pid;
  return *aux==pidGlobal?true:false; //redundancia que puede ser al pedo
}

bool pidDeStructIgualAlGlobal(void* pq){
  pidConQ *aux = (pidConQ*)pq;
  return aux->pid==pidGlobal?true:false; //redundancia que puede ser al pedo
}

int buscarQPrima(int pid){
  pidGlobal=pid;
  pidConQ *pidConQEncontrado = list_find(listQPrimas, pidDeStructIgualAlGlobal);
  return pidConQEncontrado->qPrima;
}

pidConQ *buscarPidConQ(int pid){
  pidGlobal=pid;
  return (pidConQ *)list_find(listQPrimas, pidIgualAlGlobal);
}

void restaurarQPrima(int pid){
  pidConQ *aux = buscarPidConQ(pid);
  aux->qPrima=quantum;
}

void modificarQPrima(int pid, int nuevaQPrima){
  pidConQ *aux = buscarPidConQ(pid);
  aux->qPrima=nuevaQPrima;
}

void suspenderProceso(){
  pidConQ *pidConQEXEC = buscarPidConQ(procesoEXEC);
  int qPrimaNueva = quantum-tiempoTranscurrido;
  pidConQEXEC->qPrima=qPrimaNueva==0?quantum:qPrimaNueva; 
  int *aux = malloc(sizeof(int));
  *aux=procesoEXEC;
  list_add(procesosSuspendidos, aux);
  procesoEXEC=0;
  //printf("proceso suspendido: %d, qprima: %d \n", *aux, pidConQEXEC->qPrima);
}

void sacarDeSuspension(){ //saca el primer suspendido
  //int *aux = list_get(procesosSuspendidos, 0);
  int *aux = list_remove(procesosSuspendidos, 0);
  //if(aux==NULL) printf("no anda el get");
  //else printf("el get devuelve %d", *aux);
  list_add(procesosREADY, aux);
  //printf("proceso sacado de suspension: %d \n", *aux);
}

/*
void listaPidQ(t_list *l, t_list *pids){
  for(int i=0;i<list_size(pids);i++){
    list_add(l, pidConQ(list_get(pids, i)));
  }
}


void imprimirListaPidQ(t_list *l){
  for(int i=0;i<list_size(l);i++){
    pidConQ *pq = list_get(l, i);
    printf("pid: %d, QP: %d", pq->pid, pq->qPrima);
  }
}*/

void agregarADict(int *pid){
  int value = 3;
  //char key = *pid;
  dictionary_put(dictQPrimas, pid, &value);
}

int* qPrima(int *pid){
  char c;
  c = (char)*pid;
  return dictionary_get(dictQPrimas, &c);
}

void imprimirLista(t_list* l){
  //int i=0;
  for(int i=0;i<list_size(l);i++){
    int *elem = list_get(l, i);
    if(elem==NULL)
    {
      printf("cagaste\n");
      return;
    }
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
  int pidGlobal=0;
  //sleep(2);
  //printf("llega adentro de iniciarPlani\n");
  procesosNEW=list_create();
  //printf("entrando a ciclo plani\n");
  procesosREADY=list_create();
  listQPrimas=list_create();
  procesosSuspendidos=list_create();
  //dictQPrimas=dictionary_create();
  procesoEXEC=0;
  int uno=1, dos=2, tres=3;


  list_add(procesosNEW, &uno);
  list_add(procesosNEW, &dos);
  list_add(procesosNEW, &tres);
  //printf("llega aca\n");

  ciclo_planificacion();
  estadoPlani();
  suspenderProceso();
  ciclo_planificacion();
  estadoPlani();
  sacarDeSuspension();
  ciclo_planificacion();
  estadoPlani();
  for(int i=0;i<8;i++)
  {
    ciclo_planificacion();
    estadoPlani();
  }
  printf("segunda suspension-------------------------\n");
  suspenderProceso();
  ciclo_planificacion();
  estadoPlani();
  sacarDeSuspension();
  tipoPlanificacion=FIFO;
  for(int i=0;i<13;i++)
  {
    ciclo_planificacion();
    estadoPlani();
  }
  suspenderProceso();
  ciclo_planificacion();
  estadoPlani();
  ciclo_planificacion();
  estadoPlani();

  /*pidConQ *qPrimaUno = nuevoPidConQ(uno);
  list_add(listQPrimas, qPrimaUno);
  //printf("Q prima del uno: %d\n", ((pidConQ*)list_get(listQPrimas, 0))->qPrima);
  printf("Q prima del uno: %d\n",buscarQPrima(1));
  procesoEXEC=1;
  suspenderProceso();
  sacarDeSuspension();
  imprimirLista(procesosREADY);
  modificarQPrima(1, 9);
  printf("Q prima del uno: %d\n",buscarQPrima(1));*/


}



void ciclo_plani_FIFO(){
  while(!list_is_empty(procesosNEW) && list_size(procesosREADY)<GRADO_MULTIPROGRAMACION){ //si entró un nuevo proceso y todavia no tengo el ready al maximo, lo mando
    //list_add(procesosREADY, list_remove(procesosNEW, 0));
    int *pidNuevo = list_remove(procesosNEW, 0);
    pidConQ *pqNuevo = nuevoPidConQ(*pidNuevo);
    list_add(listQPrimas, pqNuevo);
    list_add(procesosREADY, pidNuevo);
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
  if(tiempoTranscurrido>=quantum && procesoEXEC!=0){
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
    //list_add(procesosREADY, list_remove(procesosNEW, 0));
    int *pidNuevo = list_remove(procesosNEW, 0);
    pidConQ *pqNuevo = nuevoPidConQ(*pidNuevo);
    list_add(listQPrimas, pqNuevo);
    list_add(procesosREADY, pidNuevo);
  } 
  if(procesoEXEC==0){
    pthread_mutex_lock(&mutexExec);
    int* exec = list_remove(procesosREADY,0);
    procesoEXEC= *exec; 
    tiempoTranscurrido=0;
    pthread_mutex_unlock(&mutexExec);
  }
  if(procesoEXEC==0) ejecutandoProceso=0;
  else ejecutandoProceso=1;
  tiempoTranscurrido++;
}
void desalojarProceso(){
  
}
void bloquearProceso(){

}

void ciclo_plani_VRR(){
  /*if(tiempoTranscurrido>=quantum && procesoEXEC!=0){
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
    int *pidTrasladado = list_remove(procesosNEW, 0);
    list_add(procesosREADY, pidTrasladado);
    agregarADict(dictQPrimas, pidTrasladado);
  } */

  if(procesoEXEC!=0 && tiempoTranscurrido >=buscarQPrima(procesoEXEC) ){
    printf("FIN DE QPRIMA\n");
    tiempoTranscurrido=0;
    int *procesoDesalojado = malloc(sizeof(int)); 
    *procesoDesalojado = procesoEXEC;
    restaurarQPrima(procesoEXEC);
    list_add(procesosREADY, procesoDesalojado);
    pthread_mutex_lock(&mutexExec);
    procesoEXEC=0;
    pthread_mutex_unlock(&mutexExec);
  }
  while(!list_is_empty(procesosNEW) && list_size(procesosREADY)<GRADO_MULTIPROGRAMACION){ //si entró un nuevo proceso y todavia no tengo el ready al maximo, lo mando
    int *pidNuevo = list_remove(procesosNEW, 0);
    pidConQ *pqNuevo = nuevoPidConQ(*pidNuevo);
    list_add(listQPrimas, pqNuevo);
    list_add(procesosREADY, pidNuevo);
  } 
  if(procesoEXEC==0){
    pthread_mutex_lock(&mutexExec);
    int* exec = list_remove(procesosREADY,0);
    procesoEXEC= *exec; 
    tiempoTranscurrido=0;
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
    case VRR:
      ciclo_plani_VRR();
      break;
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
