// #include <stdlib.h>
// #include <stdio.h>
// #include <commons/config.h>
// #include <pthread.h>
#include "utils/shared.h"
// #include <commons/log.h>
#include "../include/funciones_kernel.h"
#include "../include/servicios_kernel.h"

PCB *iniciar_PCB()
{ // revisar si anda o hay que poner struct adelante

  t_config *config = iniciar_configuracion("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/kernel/kernel.config");

  PCB *pcb = malloc(sizeof(PCB));

  pcb->pid = asignar_pid();
  pcb->program_counter = 0;
  pcb->quantum = config_get_int_value(config, "QUANTUM");
  // p->b.pathTXT = path;
  pcb->registros_cpu.AX = 0;
  pcb->registros_cpu.BX = 0;
  pcb->registros_cpu.CX = 0;
  pcb->registros_cpu.DX = 0;
  pcb->registros_cpu.EAX = 0;
  pcb->registros_cpu.EBX = 0;
  pcb->registros_cpu.ECX = 0;
  pcb->registros_cpu.EDX = 0;
  pcb->registros_cpu.SI = 0;
  pcb->registros_cpu.DI = 0;

  // printf("El numero del pcb es: ");
  // printf("%d\n", pcb.pid);

  return pcb;
}

void finalizarProceso(int pid)
{
  // Le paso el pid a memoria, para que borre lo asociado a ese pid
  t_buffer *a_enviar = crear_buffer();
  a_enviar->size = 0;
  a_enviar->stream = NULL;

  cargar_int_al_buffer(a_enviar, pid);

  t_paquete *un_paquete = crear_super_paquete(ELIMINAR_PROCESO, a_enviar);
  enviar_paquete(un_paquete, fd_memoria);
  destruir_paquete(un_paquete);
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

void enviar_pcb_con_codop(PCB pcb, op_code codop, int socket_enviar)
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

  t_paquete *un_paquete = crear_super_paquete(codop, a_enviar);
  enviar_paquete(un_paquete, socket_enviar);
  destruir_paquete(un_paquete);
}

void enviar_pid_a_cpu(int pid)
{
  t_buffer *buffer_pid = crear_buffer();
  buffer_pid->size = 0;
  buffer_pid->stream = NULL;
  cargar_int_al_buffer(buffer_pid, pid);
  t_paquete *paquete_pid = crear_super_paquete(ENVIAR_PID, buffer_pid);
  enviar_paquete(paquete_pid, fd_cpu_dispatch);
  destruir_paquete(paquete_pid);
}

void planificacion()
{
  while (seguirPlanificando)
  {
    sem_wait(&sem_cpu_libre);
    ciclo_planificacion();
    if (ejecutandoProceso)
    {
      enviar_pid_a_cpu(procesoEXEC);
    }
    else
      sem_post(&sem_cpu_libre);
  }
}



pidConQ *nuevoPidConQ(int pid)
{
  pidConQ *ret = malloc(sizeof(pidConQ));
  ret->pid = pid;
  ret->qPrima = quantum;
  return ret;
}

bool pidIgualAlGlobal(void *pid)
{
  int *aux = (int *)pid;
  return *aux == pidGlobal ? true : false; // redundancia que puede ser al pedo
}

bool pidDeStructIgualAlGlobal(void *pq)
{
  pidConQ *aux = (pidConQ *)pq;
  return aux->pid == pidGlobal ? true : false; // redundancia que puede ser al pedo
}

int buscarQPrima(int pid)
{
  pidGlobal = pid;
  pidConQ *pidConQEncontrado = list_find(listQPrimas, pidDeStructIgualAlGlobal);

  if (pidConQEncontrado == NULL)
  {
    // Maneja el caso en el que no se encuentra nada
    return 1000000; // O cualquier valor que elijas para indicar que no se encontró nada
  }

  return pidConQEncontrado->qPrima;
}

pidConQ *buscarPidConQ(int pid)
{
  pidGlobal = pid;
  return (pidConQ *)list_find(listQPrimas, pidIgualAlGlobal);
}

void restaurarQPrima(int pid)
{
  pidConQ *aux = buscarPidConQ(pid);
  aux->qPrima = quantum;
}

void modificarQPrima(int pid, int nuevaQPrima)
{
  pidConQ *aux = buscarPidConQ(pid);
  aux->qPrima = nuevaQPrima;
}

void suspenderProceso()
{
  pidConQ *pidConQEXEC = buscarPidConQ(procesoEXEC);
  int qPrimaNueva = pidConQEXEC->qPrima - tiempoTranscurrido;
  pidConQEXEC->qPrima = qPrimaNueva == 0 ? quantum : qPrimaNueva;
  int *aux = malloc(sizeof(int));
  *aux = procesoEXEC;
  list_add(procesosSuspendidos, aux);
  procesoEXEC = 0;
  // printf("proceso suspendido: %d, qprima: %d \n", *aux, pidConQEXEC->qPrima);
}

void bloquearPorRecurso(char* nombre) //FALTA PROBAR
{
  pidConQ *pidConQEXEC = buscarPidConQ(estaEJecutando);
  int qPrimaNueva = pidConQEXEC->qPrima - tiempoTranscurrido;
  pidConQEXEC->qPrima = qPrimaNueva == 0 ? quantum : qPrimaNueva;

  int i=0;
  for(i=0;strcmp(nombre, nombresRecursos[i])!=0;i++); //PREGUNTAR LUCA, antes estaba en ==
  t_list *lista_donde_agregar = list_get(lista_recursos_y_bloqueados, i);

  printf("lo agrege a la lista nro: %d\n",i);

  int *aux = malloc(sizeof(int));
  *aux = estaEJecutando;
  
  list_add(lista_donde_agregar, aux);
  //procesoEXEC=0;
}

void estado_instancias()
{
  for(int i=0;nombresRecursos[i] != NULL;i++)
  {
    int *aux = list_get(instanciasRecursos, i);
    printf("recurso: %s, instancias: %d \n", nombresRecursos[i],*aux);
  }
}

pidConRecursos_t *encontrarPidRecursos(int pid)
{
  pidConRecursos_t *pidConRec=NULL;
  for(int i=0;i<list_size(listaPidsRecursos);i++) //me rehuso a usar list_find
  {
    pidConRec=list_get(listaPidsRecursos, i);
    if(pidConRec->pid == pid)
    {
      break;
    }
  }
  return pidConRec;
}

void mostrarInstanciasTomadas(int pid)
{
  pidConRecursos_t *pidConRec = encontrarPidRecursos(pid);
  /*for(int i=0;i<list_size(listaPidsRecursos);i++) //me rehuso a usar list_find
  {
    pidConRec=list_get(listaPidsRecursos, i);
    if(pidConRec->pid == pid)
    {
      break;
    }
  }*/
  for(int j=0;nombresRecursos[j]!=NULL;j++)
  {
    printf("recurso: %s, instancias usadas: %d\n", nombresRecursos[j], *(int*)list_get(pidConRec->recursosTomados,j));
  }
}


void atender_wait(char* recurso, int *pid) //FALTA PROBAR
{
  int i=0;
  for(i=0;strcmp(recurso, nombresRecursos[i])!=0;i++);

  int *instancias = list_get(instanciasRecursos, i);

  pidGlobal = *pid;

  pidConRecursos_t *pidRec = encontrarPidRecursos(*pid);

  int *instanciasPedidasRecurso = list_get(pidRec->recursosTomados, i); 

  //printf("recurso pedido: %s \n", recurso);

  if(*instancias>0)
  {
    *instancias = (*instancias) - 1;
    //printf("hay instancias disponibles \n");
    list_add(procesosREADY, pid);
    //mostrarInstanciasTomadas(pid);
    *instanciasPedidasRecurso = (*instanciasPedidasRecurso) +1;
    //printf("Instancias disponibles, se tomo el recurso\n");
  }
  else
  {
    bloquearPorRecurso(recurso);
    //printf("NO hay instancias disponibles, se bloqueo el proceso \n");
  }
  //estado_instancias();
  //printf("fin atender_wait\n");
  //mostrarInstanciasTomadas(*pid);
}

void atender_signal(char* recurso, int *pid) //FALTA PROBAR
{
  int i=0;
  for(i=0;strcmp(recurso, nombresRecursos[i])!=0;i++);

  t_list *bloqueados_por_este_recurso = list_get(lista_recursos_y_bloqueados, i);

  pidConRecursos_t *pidRec = encontrarPidRecursos(*pid);
  int* instanciasPedidasRecurso = list_get(pidRec->recursosTomados, i);

  if(*instanciasPedidasRecurso > 0)
  {
    /*printf("Se esta intentando liberar un recurso que nunca se pidio\n");
    return; //flaco para que carajo liberas un recurso que no pediste*/
    *instanciasPedidasRecurso = (*instanciasPedidasRecurso)-1;
  }

  
  //printf("Se liberó el recurso\n");

  //printf("recurso liberado: %s\n", recurso);
  if(list_size(bloqueados_por_este_recurso) == 0)
  {
    int * instancias_de_este_recurso = list_get(instanciasRecursos, i);
    *instancias_de_este_recurso = (*instancias_de_este_recurso) + 1;
  }
  else
  {
    list_add(procesosREADY, list_remove(bloqueados_por_este_recurso, 0));
  }
  //estado_instancias();
  //mostrarInstanciasTomadas(*pid);
  //printf("fin atender_signal\n");
}

void sacarDeSuspension()
{ // saca el primer suspendido
  // int *aux = list_get(procesosSuspendidos, 0);
  int *aux = list_remove(procesosSuspendidos, 0);
  // if(aux==NULL) printf("no anda el get");
  // else printf("el get devuelve %d", *aux);
  list_add(procesosREADY, aux);
  // printf("proceso sacado de suspension: %d \n", *aux);
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

void agregarADict(int *pid)
{
  int value = 3;
  // char key = *pid;
  dictionary_put(dictQPrimas, pid, &value);
}

int *qPrima(int *pid)
{
  char c;
  c = (char)*pid;
  return dictionary_get(dictQPrimas, &c);
}

void imprimirLista(t_list *l)
{
  // int i=0;
  for (int i = 0; i < list_size(l); i++)
  {
    int *elem = list_get(l, i);
    if (elem == NULL)
    {
      printf("cagaste\n");
      return;
    }
    printf("%d, ", *elem);
  }
}

void estadoPlani()
{
  // int *new0=list_get(procesosREADY, 0);
  // void *ready0=list_get(procesosREADY, 1);
  // printf("New: %d, Ready:%d, exec:%d", new0, ready0, procesoEXEC);
  // printf("ready: %d, exec:%d\n",*new0, procesoEXEC);
  printf("ready: ");
  imprimirLista(procesosREADY);
  printf(" exec: %d \n", procesoEXEC);
}

void iniciar_bucle()
{
  while (flagSeguirPlanificando)
  {
    ciclo_planificacion();
    usleep(100000);
  }
}

void iniciar_planificacion()
{
  listaPCBs = list_create();
  listaPidsRecursos=list_create();
  int flagCambioProceso = 0;

  // sleep(2);
  // printf("llega adentro de iniciarPlani\n");
  procesosNEW = list_create();
  
  // printf("entrando a ciclo plani\n");
  procesosREADY = list_create();
  listQPrimas = list_create();
  procesosSuspendidos = list_create();

  lista_recursos_y_bloqueados = list_create();

  // Crear N listas y aÃ±adirlas a la lista de listas
  for (int i = 0; i < cantidad_de_recursos; i++)
  {
    t_list *new_list = list_create();
    list_add(lista_recursos_y_bloqueados, new_list);
  }

  iniciar_bucle();

  // dictQPrimas=dictionary_create();

  /*
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

void avisarDesalojo()
{
  log_trace(kernel_log_debug, "PID: %d - Desalojado por fin de Quantum", estaEJecutando);
  t_buffer *buffer = crear_buffer();
  buffer->size = 0;
  buffer->stream = NULL;
  cargar_int_al_buffer(buffer, 0);
  t_paquete *paquete_pid = crear_super_paquete(AVISO_DESALOJO, buffer);
  enviar_paquete(paquete_pid, fd_cpu_interrupt);
  destruir_paquete(paquete_pid);
}

void ciclo_plani_FIFO()
{
  // printf("entre al fifo\n");
  while (!list_is_empty(procesosNEW) && list_size(procesosREADY) < GRADO_MULTIPROGRAMACION)
  { // si entró un nuevo proceso y todavia no tengo el ready al maximo, lo mandod
    int *pidNuevo = list_remove(procesosNEW, 0);
    pidConQ *pqNuevo = nuevoPidConQ(*pidNuevo);
    list_add(listQPrimas, pqNuevo);
    list_add(procesosREADY, pidNuevo);
    // mientras que haya cosas en new y el grado de multiprogramacion me lo permita, lo paso a ready
  }
  // printf("limpiada lista de new\n");
  if (procesoEXEC == 0 && !list_is_empty(procesosREADY) && estaCPULibre) // si no hay ningun proceso en ejecucion, pone el primero de READY
  {
    pthread_mutex_lock(&mutexExec);
    int *exec = list_remove(procesosREADY, 0);
    procesoEXEC = *exec;
    pthread_mutex_unlock(&mutexExec);
    // avisarDesalojo();
  }
  // if(procesoEXEC==0) ejecutandoProceso=0;
  // else ejecutandoProceso=1;

  if (procesoEXEC != 0)
  {
    mandarNuevoPCB();
  }
}

void ciclo_plani_RR()
{
  // printf("entre al rr\n");
  //  quantum++;
  if (tiempoTranscurrido >= quantum && !estaCPULibre) // FIN DE QUANTUM
  {
    tiempoTranscurrido = 0;
    avisarDesalojo();
    sem_wait(&esperar_devolucion_pcb);
  }
  while (!list_is_empty(procesosNEW) && list_size(procesosREADY) < GRADO_MULTIPROGRAMACION) // si hay procesos en new y los podes pasar a ready pasalos
  {                                                                                         // si entró un nuevo proceso y todavia no tengo el ready al maximo, lo mando
    int *pidNuevo = list_remove(procesosNEW, 0);
    pidConQ *pqNuevo = nuevoPidConQ(*pidNuevo);
    list_add(listQPrimas, pqNuevo);
    list_add(procesosREADY, pidNuevo);
  }
  if (procesoEXEC == 0 && !list_is_empty(procesosREADY) && estaCPULibre)
  {
    pthread_mutex_lock(&mutexExec);
    int *exec = list_remove(procesosREADY, 0);
    procesoEXEC = *exec;
    pthread_mutex_unlock(&mutexExec);
  }

  if (procesoEXEC != 0)
  {
    mandarNuevoPCB();
  }

  tiempoTranscurrido++;
}
void desalojarProceso()
{
}
void bloquearProceso()
{
}

void ciclo_plani_VRR()
{
  if (tiempoTranscurrido >= buscarQPrima(estaEJecutando) && !estaCPULibre) // si el tiempo transcurrido es mayor a lo que le queda
  {                                                                        // cuando se termino su qprima
    // printf("FIN DE QPRIMA\n");
    restaurarQPrima(estaEJecutando);
    tiempoTranscurrido = 0;
    avisarDesalojo();
    sem_wait(&esperar_devolucion_pcb);
  }

  while (!list_is_empty(procesosNEW) && list_size(procesosREADY) < GRADO_MULTIPROGRAMACION)
  { // si entró un nuevo proceso y todavia no tengo el ready al maximo, lo mando
    int *pidNuevo = list_remove(procesosNEW, 0);
    pidConQ *pqNuevo = nuevoPidConQ(*pidNuevo);
    list_add(listQPrimas, pqNuevo);
    list_add(procesosREADY, pidNuevo);
  }

  if (procesoEXEC == 0 && !list_is_empty(procesosREADY) && estaCPULibre)
  {
    pthread_mutex_lock(&mutexExec);
    int *exec = list_remove(procesosREADY, 0);
    procesoEXEC = *exec;
    pthread_mutex_unlock(&mutexExec);
  }

  if (procesoEXEC != 0)
  {
    mandarNuevoPCB();
  }

  tiempoTranscurrido++;
}

void ciclo_planificacion()
{
  switch (tipoPlanificacion)
  {
  case FIFO:
    // printf("CICLO FIFO\n");
    ciclo_plani_FIFO();
    break;
  case RR:
    ciclo_plani_RR();
    break;
  case VRR:
    ciclo_plani_VRR();
    break;
  default:
    break;
  }
  // if(flagCambioProceso)
  //{
  // mandarNuevoPCB();
  // flagCambioProceso=0;
  //}
}

bool condition_id_igual_n(void *elemento)
{
  PCB *dato = (PCB *)elemento;
  return (dato->pid == pidGlobal);
}

PCB *buscarPCB(int pid)
{
  pidGlobal = pid;
  PCB *PCBEncontrado = list_find(listaPCBs, condition_id_igual_n);
  return PCBEncontrado;
}

void mandarNuevoPCB()
{
  // printf("mande un PCB\n");
  PCB *pcb_a_enviar = buscarPCB(procesoEXEC); // Busco el pcb que le toca ejecutar en la cola
  enviar_pcb(*pcb_a_enviar, fd_cpu_dispatch); // si rompe es casi seguro porque busca un pcb que no coincide con el pid
  estaEJecutando = procesoEXEC;
  procesoEXEC = 0;
  estaCPULibre = false;

  // pthread_mutex_lock(&modificarLista);
  list_remove_element(procesosREADY, (void *)pcb_a_enviar->pid);
  list_remove_element(listaPCBs, (void *)pcb_a_enviar);
  // pthread_mutex_unlock(&modificarLista);

  // printf("mande un pcb\n");

  /*BORRAR DESPUES*/

  int fdXD = obtener_fd_interfaz("MONITOR");

  t_buffer *buffer_pid = crear_buffer();
  buffer_pid->size = 0;
  buffer_pid->stream = NULL;

  cargar_int_al_buffer(buffer_pid, 1);

  t_paquete *paquete_pid = crear_super_paquete(HABLAR_CON_IO, buffer_pid);
  enviar_paquete(paquete_pid, fdXD);
  destruir_paquete(paquete_pid);
}

void nuevaListaRecursos(int pid)
{
  pidConRecursos_t *pidRecAux = malloc(sizeof(pidConRecursos_t));
  pidRecAux->pid = pid;
  pidRecAux->recursosTomados = list_create();
  for(int i=0;i<nombresRecursos[i]!=NULL;i++)
  {
    int *recurso_i = malloc(sizeof(int));
    *recurso_i =0;
    list_add(pidRecAux->recursosTomados, recurso_i);
    //printf("recurso: %s, instancias usadas: %d\n", nombresRecursos[i], *recurso_i);
  }
  list_add(listaPidsRecursos, pidRecAux);
}

void iniciar_proceso(char *path)
{
  PCB *pcb = iniciar_PCB(path);
  printf("el pid es %d\n", pcb->pid);
  list_add(listaPCBs, pcb);
  enviar_path_memoria(path, pcb->pid);
  list_add(procesosNEW, &(pcb->pid)); // agrego el pcb al planificador de pids
  nuevaListaRecursos(pcb->pid);
  //mostrarInstanciasTomadas(pcb->pid);
}

void liberarRecursosProceso(int *pid)
{
  pidConRecursos_t *pidRec = encontrarPidRecursos(*pid);
  for(int i=0;nombresRecursos[i]!=NULL;i++)
  {
    int *recurso_i = list_get(pidRec->recursosTomados, i);
    while(*recurso_i>0) //mientras haya instancias pedidas, simulo un signal para no repetir logica
    {
      atender_signal(nombresRecursos[i], pid);
      recurso_i = list_get(pidRec->recursosTomados, i); //actualizo instancias_i
    }
    
  }
}

interfaces_io* encontrar_interfaz(char* nombre_buscado){
  
    for (int i = 0; i < list_size(lista_interfaces); i++) {

        interfaces_io* elemento = list_get(lista_interfaces, i);

        printf("nombre: %d\n",elemento->nombre_interfaz);

        //if (strcmp(elemento->nombre_interfaz, nombre_buscado) == 0) {
          //  return elemento;
        //}
    }
    return NULL; // No se encontró la interfaz con el nombre buscado
}

int obtener_fd_interfaz(char* nombre_interfaz){

  interfaces_io* interfaz = encontrar_interfaz(nombre_interfaz);

  return interfaz->fd_interfaz;
}