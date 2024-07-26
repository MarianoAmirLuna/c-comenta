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

  //t_config *config = iniciar_configuracion("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/kernel/deadlock.config");

  PCB *pcb = malloc(sizeof(PCB));

  pcb->pid = asignar_pid();
  pcb->program_counter = 0;
  pcb->quantum = QUANTUM;
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

  // sem_wait(&esperar_carga_path_memoria);
}

void enviar_pcb(PCB pcb, int socket_enviar, int numerillo)
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
  cargar_int_al_buffer(a_enviar, numerillo);

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

/*
void planificacion()
{
  while (seguirPlanificando)
  {
    sem_wait(&sem_cpu_libre);
    ciclo_planificacion();
    if (ejecutandoProceso)
    {
      enviar_pid_a_cpu(procesoEXEC); //CREO QUE ESTA AL PEDO
    }
    else
      sem_post(&sem_cpu_libre);
  }
}*/

pidConQ *nuevoPidConQ(int pid)
{
  pidConQ *ret = malloc(sizeof(pidConQ));
  ret->pid = pid;
  ret->qPrima = QUANTUM;
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
    return 1000; // O cualquier valor que elijas para indicar que no se encontró nada
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
  aux->qPrima = QUANTUM;
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
  pidConQEXEC->qPrima = qPrimaNueva == 0 ? QUANTUM : qPrimaNueva;
  int *aux = malloc(sizeof(int));
  *aux = procesoEXEC;
  list_add(procesosSuspendidos, aux);
  procesoEXEC = 0;
  // printf("proceso suspendido: %d, qprima: %d \n", *aux, pidConQEXEC->qPrima);
}

void actualizarQPrimaProceso(int pid, int tiempo)
{ // le resta el tiempo al quantum del proceso

  pidConQ *pidConQEXEC = buscarPidConQ(pid);                      // obtener el pid_qprima
  int qPrimaNueva = pidConQEXEC->qPrima - tiempo;                 // obtengo el nuevo quantum
  pidConQEXEC->qPrima = qPrimaNueva <= 0 ? QUANTUM : qPrimaNueva; // en el caso de que sea 0, lo pongo en QUANTUM
  printf("el nuevo tiempo es: \n", pidConQEXEC->qPrima);
}

void bloquearPorRecurso(char *nombre) // FALTA PROBAR
{
  pidConQ *pidConQEXEC = buscarPidConQ(estaEJecutando);           // obtener el pid_qprima
  int qPrimaNueva = pidConQEXEC->qPrima - tiempoTranscurrido;     // obtengo el nuevo quantum
  pidConQEXEC->qPrima = qPrimaNueva == 0 ? QUANTUM : qPrimaNueva; // en el caso de que sea 0, lo pongo en QUANTUM

  int i = 0;
  for (i = 0; strcmp(nombre, nombresRecursos[i]) != 0; i++)
    ; // PREGUNTAR LUCA, antes estaba en ==
  t_list *lista_donde_agregar = list_get(lista_recursos_y_bloqueados, i);

  printf("lo agrege a la lista nro: %d\n", i);

  int *aux = malloc(sizeof(int));
  *aux = estaEJecutando;

  list_add(lista_donde_agregar, aux);
  // procesoEXEC=0;
}

void estado_instancias()
{
  for (int i = 0; nombresRecursos[i] != NULL; i++)
  {
    int *aux = list_get(instanciasRecursos, i);
    printf("recurso: %s, instancias: %d \n", nombresRecursos[i], *aux);
  }
}

pidConRecursos_t *encontrarPidRecursos(int pid)
{
  pidConRecursos_t *pidConRec = NULL;
  for (int i = 0; i < list_size(listaPidsRecursos); i++) // me rehuso a usar list_find
  {
    pidConRec = list_get(listaPidsRecursos, i);
    if (pidConRec->pid == pid)
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
  for (int j = 0; nombresRecursos[j] != NULL; j++)
  {
    printf("recurso: %s, instancias usadas: %d\n", nombresRecursos[j], *(int *)list_get(pidConRec->recursosTomados, j));
  }
}

void atender_wait(char *recurso, int *pid) // FALTA PROBAR
{
  int i = 0;
  for (i = 0; strcmp(recurso, nombresRecursos[i]) != 0; i++)
    ;

  int *instancias = list_get(instanciasRecursos, i);

  pidGlobal = *pid;

  pidConRecursos_t *pidRec = encontrarPidRecursos(*pid);

  int *instanciasPedidasRecurso = list_get(pidRec->recursosTomados, i);

  // printf("recurso pedido: %s \n", recurso);

  if (*instancias > 0)
  {
    *instancias = (*instancias) - 1;
    // printf("hay instancias disponibles \n");
    pthread_mutex_lock(&proteger_lista_ready);
    list_add(procesosREADY, pid);
    pthread_mutex_unlock(&proteger_lista_ready);
    // mostrarInstanciasTomadas(pid);
    *instanciasPedidasRecurso = (*instanciasPedidasRecurso) + 1;
    // printf("Instancias disponibles, se tomo el recurso\n");
  }
  else
  {
    bloquearPorRecurso(recurso);
    // printf("NO hay instancias disponibles, se bloqueo el proceso \n");
  }
  // estado_instancias();
  // printf("fin atender_wait\n");
  // mostrarInstanciasTomadas(*pid);
}

void atender_signal(char *recurso, int *pid) // FALTA PROBAR
{
  int i = 0;
  for (i = 0; strcmp(recurso, nombresRecursos[i]) != 0; i++)
    ;

  t_list *bloqueados_por_este_recurso = list_get(lista_recursos_y_bloqueados, i);

  pidConRecursos_t *pidRec = encontrarPidRecursos(*pid);
  int *instanciasPedidasRecurso = list_get(pidRec->recursosTomados, i);

  if (*instanciasPedidasRecurso > 0)
  {
    /*printf("Se esta intentando liberar un recurso que nunca se pidio\n");
    return; //flaco para que carajo liberas un recurso que no pediste*/
    *instanciasPedidasRecurso = (*instanciasPedidasRecurso) - 1;
  }

  // printf("Se liberó el recurso\n");

  // printf("recurso liberado: %s\n", recurso);
  if (list_size(bloqueados_por_este_recurso) == 0)
  {
    int *instancias_de_este_recurso = list_get(instanciasRecursos, i);
    *instancias_de_este_recurso = (*instancias_de_este_recurso) + 1;
  }
  else
  {
    pthread_mutex_lock(&proteger_lista_ready);
    list_add(procesosREADY, list_remove(bloqueados_por_este_recurso, 0));
    pthread_mutex_unlock(&proteger_lista_ready);
  }
  // estado_instancias();
  // mostrarInstanciasTomadas(*pid);
  // printf("fin atender_signal\n");
}

void sacarDeSuspension()
{ // saca el primer suspendido
  // int *aux = list_get(procesosSuspendidos, 0);
  int *aux = list_remove(procesosSuspendidos, 0);
  // if(aux==NULL) printf("no anda el get");
  // else printf("el get devuelve %d", *aux);
  pthread_mutex_lock(&proteger_lista_ready);
  list_add(procesosREADY, aux);
  pthread_mutex_unlock(&proteger_lista_ready);
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
/*
void iniciar_bucle()
{
  while (1)
  {
    sem_wait(&sem_seguir_planificando);
    ciclo_planificacion();
    sem_post(&sem_seguir_planificando);
    usleep(100000);
  }
}*/

void iniciar_planificacion()
{
  while (1)
  {
    sem_wait(&nuevo_bucle);
    // printf("se hizo un bucle!!");
    sem_wait(&sem_seguir_planificando);
    ciclo_planificacion();
    sem_post(&sem_seguir_planificando);
  }
}

int tiempo_transcurrido_milisegundos(struct timespec start, struct timespec end)
{
  return (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
}

void iniciar_tiempo()
{
  clock_gettime(CLOCK_MONOTONIC, &start_time);
}

void detener_tiempo()
{
  clock_gettime(CLOCK_MONOTONIC, &end_time);
}

/*void temporizadorQuantum(int quantum)
{
  while (1)
  {
    sem_wait(&contador_q);
    usleep(quantum_global_reloj * 1000);
    tiempoTranscurrido = quantum_global_reloj;
    sem_post(&nuevo_bucle);
  }
}*/

void ejectuar_siguiente_instruccion_io(interfaces_io interfaz)
{
  t_buffer *buffer = crear_buffer();
  buffer->size = 0;
  buffer->stream = NULL;

  instruccion *instruccionXD = queue_pop(interfaz.instrucciones_ejecutar); // saco la instruccion de la queue y la ejecuto
  int* pid = list_get(interfaz.procesos_bloqueados->elements,0); //obtengo el pid
  cargar_int_al_buffer(buffer,*pid);

  if (strcmp(instruccionXD->nombre_instruccion, "IO_GEN_SLEEP") == 0)
  {
    printf("voy a ejecutar un gen_sleep\n");

    int *unidades_trabajo = list_get(instruccionXD->lista_enteros, 0);
    
    cargar_int_al_buffer(buffer, *unidades_trabajo);
    t_paquete *paquete = crear_super_paquete(ENVIAR_IO_GEN_SLEEP, buffer);
    enviar_paquete(paquete, interfaz.fd_interfaz);
    destruir_paquete(paquete);
  }

  else if (strcmp(instruccionXD->nombre_instruccion, "IO_STDIN_READ") == 0)
  {
    for (int i = 0; i < list_size(instruccionXD->lista_enteros); i++)
    {
      int *numerin = list_get(instruccionXD->lista_enteros, i);
      cargar_int_al_buffer(buffer, *numerin);
    }
    t_paquete *paquete = crear_super_paquete(ENVIAR_IO_STDIN_READ, buffer);
    enviar_paquete(paquete, interfaz.fd_interfaz);
    destruir_paquete(paquete);
  }

  else if (strcmp(instruccionXD->nombre_instruccion, "IO_STDOUT_WRITE") == 0)
  {
    for (int i = 0; i < list_size(instruccionXD->lista_enteros); i++)
    {
      int *numerito = list_get(instruccionXD->lista_enteros, i);
      cargar_int_al_buffer(buffer, *numerito);
    }

    t_paquete *paquete = crear_super_paquete(ENVIAR_IO_STDOUT_WRITE, buffer);
    enviar_paquete(paquete, interfaz.fd_interfaz);
    destruir_paquete(paquete);

    printf("mande el paquete a kernel en write\n");
  }

  else if (strcmp(instruccionXD->nombre_instruccion, "IO_FS_CREATE") == 0)
  {
    cargar_string_al_buffer(buffer,instruccionXD->nombre_archivo);

    t_paquete *paquete = crear_super_paquete(ENVIAR_IO_FS_CREATE, buffer);
    enviar_paquete(paquete, interfaz.fd_interfaz);
    destruir_paquete(paquete);
  }

  else if (strcmp(instruccionXD->nombre_instruccion, "IO_FS_DELETE") == 0)
  {
    cargar_string_al_buffer(buffer,instruccionXD->nombre_archivo);

    t_paquete *paquete = crear_super_paquete(ENVIAR_IO_FS_DELETE, buffer);
    enviar_paquete(paquete, interfaz.fd_interfaz);
    destruir_paquete(paquete);
  }

  else if (strcmp(instruccionXD->nombre_instruccion, "IO_FS_TRUNCATE") == 0)
  {
    cargar_string_al_buffer(buffer,instruccionXD->nombre_archivo);
    int* numerito = list_get(instruccionXD->lista_enteros,0);
    cargar_int_al_buffer(buffer,*numerito);

    t_paquete *paquete = crear_super_paquete(ENVIAR_IO_FS_TRUNCATE, buffer);
    enviar_paquete(paquete, interfaz.fd_interfaz);
    destruir_paquete(paquete);
  }

  else if (strcmp(instruccionXD->nombre_instruccion, "IO_FS_WRITE") == 0)
  {
    cargar_string_al_buffer(buffer,instruccionXD->nombre_archivo);

    for (int i = 0; i < list_size(instruccionXD->lista_enteros); i++)
    {
      int *numerito = list_get(instruccionXD->lista_enteros, i);
      cargar_int_al_buffer(buffer, *numerito);
    }

    t_paquete *paquete = crear_super_paquete(ENVIAR_IO_FS_WRITE, buffer);
    enviar_paquete(paquete, interfaz.fd_interfaz);
    destruir_paquete(paquete);
  }

  else if (strcmp(instruccionXD->nombre_instruccion, "IO_FS_READ") == 0)
  {
    cargar_string_al_buffer(buffer,instruccionXD->nombre_archivo);

    for (int i = 0; i < list_size(instruccionXD->lista_enteros); i++)
    {
      int *numerito = list_get(instruccionXD->lista_enteros, i);
      cargar_int_al_buffer(buffer, *numerito);
    }

    t_paquete *paquete = crear_super_paquete(ENVIAR_IO_FS_READ, buffer);
    enviar_paquete(paquete, interfaz.fd_interfaz);
    destruir_paquete(paquete);
  }
}

void iniciar_planificacion_io()
{

  while (1)
  {

    for (int i = 0; i < list_size(lista_interfaces); i++)
    {

      interfaces_io *interfaz = list_get(lista_interfaces, i);

      if (interfaz->estaLibre && queue_size(interfaz->instrucciones_ejecutar) > 0)
      { // si esta libre la interfaz y tenes instrucciones para ejecutar

        printf("ejecute una instruccion de tipo io\n");

        ejectuar_siguiente_instruccion_io(*interfaz);
        interfaz->estaLibre = false;
      }
    }

    sem_wait(&ciclo_instruccion_io);
  }
}

void avisarDesalojo(int pid)
{
  log_trace(kernel_log_debug, "PID: %d - Desalojado por fin de Quantum", pid);
  t_buffer *buffer = crear_buffer();
  buffer->size = 0;
  buffer->stream = NULL;
  cargar_int_al_buffer(buffer, 0);
  t_paquete *paquete_pid = crear_super_paquete(AVISO_DESALOJO, buffer);
  enviar_paquete(paquete_pid, fd_cpu_interrupt);
  destruir_paquete(paquete_pid);
}

void desalojoFinProceso()
{
  log_trace(kernel_log_debug, "PID: %d - Desalojado por finalizacion de proceso", estaEJecutando);
  t_buffer *buffer = crear_buffer();
  buffer->size = 0;
  buffer->stream = NULL;
  cargar_int_al_buffer(buffer, 0);
  t_paquete *paquete_pid = crear_super_paquete(AVISO_DESALOJO_FIN_PROCESO, buffer);
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
    pthread_mutex_lock(&proteger_lista_ready);
    list_add(procesosREADY, pidNuevo);
    pthread_mutex_unlock(&proteger_lista_ready);
    // mientras que haya cosas en new y el grado de multiprogramacion me lo permita, lo paso a ready
  }
  // printf("limpiada lista de new\n");
  pthread_mutex_lock(&mutexExec);
  if (procesoEXEC == 0 && !list_is_empty(procesosREADY) && estaCPULibre) // si no hay ningun proceso en ejecucion, pone el primero de READY
  {

    int *exec = list_remove(procesosREADY, 0);
    procesoEXEC = *exec;
    // estaEJecutando = procesoEXEC;
    // avisarDesalojo();
  }
  pthread_mutex_unlock(&mutexExec);
  // if(procesoEXEC==0) ejecutandoProceso=0;
  // else ejecutandoProceso=1;

  if (procesoEXEC != 0) // FINALIZAR_PROCESO 4
  {
    if (estaCPULibre)
    {
      mandarNuevoPCB();
    }
  }
}

void ciclo_plani_RR()
{
  // printf("entre al rr\n");
  //  quantum++;
  /*if (tiempoTranscurrido * 100 >= QUANTUM && !estaCPULibre) // FIN DE QUANTUM
  {
    tiempoTranscurrido = 0;
    avisarDesalojo(estaEJecutando);
    sem_wait(&esperar_devolucion_pcb);
  }*/

  while (!list_is_empty(procesosNEW) && list_size(procesosREADY) < GRADO_MULTIPROGRAMACION) // si hay procesos en new y los podes pasar a ready pasalos
  {                                                                                         // si entró un nuevo proceso y todavia no tengo el ready al maximo, lo mando
    int *pidNuevo = list_remove(procesosNEW, 0);
    pidConQ *pqNuevo = nuevoPidConQ(*pidNuevo);
    list_add(listQPrimas, pqNuevo);
    pthread_mutex_lock(&proteger_lista_ready);
    list_add(procesosREADY, pidNuevo);
    pthread_mutex_unlock(&proteger_lista_ready);
  }
  pthread_mutex_lock(&mutexExec);
  if (procesoEXEC == 0 && !list_is_empty(procesosREADY) && estaCPULibre)
  {
    int *exec = list_remove(procesosREADY, 0);
    procesoEXEC = *exec;
    // estaEJecutando = procesoEXEC;
  }
  pthread_mutex_unlock(&mutexExec);

  if (procesoEXEC != 0)
  {
    if (estaCPULibre)
    {
      mandarNuevoPCB();
    }
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

  /*if (tiempoTranscurrido * 100 >= buscarQPrima(estaEJecutando) && !estaCPULibre) // si el tiempo transcurrido es mayor a lo que le queda
  {                                                                              // cuando se termino su qprima
    // printf("FIN DE QPRIMA\n");
    restaurarQPrima(estaEJecutando);
    tiempoTranscurrido = 0;
    avisarDesalojo(estaEJecutando);
    sem_wait(&esperar_devolucion_pcb);
  }*/

  while (!list_is_empty(procesosNEW) && list_size(procesosREADY) < GRADO_MULTIPROGRAMACION)
  { // si entrÃ³ un nuevo proceso y todavia no tengo el ready al maximo, lo mando
    int *pidNuevo = list_remove(procesosNEW, 0);
    pidConQ *pqNuevo = nuevoPidConQ(*pidNuevo);
    list_add(listQPrimas, pqNuevo);
    pthread_mutex_lock(&proteger_lista_ready);
    list_add(procesosREADY, pidNuevo);
    pthread_mutex_unlock(&proteger_lista_ready);
  }

  pthread_mutex_lock(&mutexExec);
  if (procesoEXEC == 0 && (!list_is_empty(procesosREADY) || !list_is_empty(procesos_READY_priori)) && estaCPULibre)
  {
    int *exec;
    if(!list_is_empty(procesos_READY_priori))
    {
      exec = list_remove(procesos_READY_priori, 0);
    }
    else
    {
      exec = list_remove(procesosREADY, 0);
    }
    procesoEXEC = *exec;
    // estaEJecutando = procesoEXEC;
  }
  pthread_mutex_unlock(&mutexExec);

  if (procesoEXEC != 0)
  {
    if (estaCPULibre)
    {
      mandarNuevoPCB();
    }
  }
}

void ciclo_planificacion()
{
  if (strcmp(ALGORITMO_PLANIFICACION, "FIFO") == 0)
  {
    ciclo_plani_FIFO();
  }
  else if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0)
  {
    ciclo_plani_RR();
  }
  else if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0)
  {
    ciclo_plani_VRR();
  }
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

void *contador_tiempos(void *arg)
{

  thread_args *args = (thread_args *)arg;

  usleep(args->tiempo * 1000);

  printf("la id es: %d\n", args->id);

  if (contiene_numero(lista_id_hilos, args->id)) // si la lista contiene el numero mando la interrupcion
  {
    avisarDesalojo(args->pid);
    printf("aviso un desalojo\n");
  }
  else
  {
    printf("NOOO aviso un desalojo\n");
  }
}

void mandarNuevoPCB()
{
  // printf("mande un PCB\n");
  sem_wait(&esperar_termine_ejecutar_pcb_cpu);

  pthread_mutex_lock(&proteger_mandar_pcb);
  PCB *pcb_a_enviar = buscarPCB(procesoEXEC);                 // Busco el pcb que le toca ejecutar en la cola
  enviar_pcb(*pcb_a_enviar, fd_cpu_dispatch, contador_hilos); // si rompe es casi seguro porque busca un pcb que no coincide con el pid
  estaEJecutando = pcb_a_enviar->pid;
  pthread_mutex_unlock(&proteger_mandar_pcb);

  procesoEXEC = 0;
  estaCPULibre = false;

  // quantum_global_reloj = QUANTUM; FINALIZAR_PROCESO 4
  // sem_post(&contador_q);
  iniciar_tiempo(); // empiezo a contar por el tema de los q primas de VRR

  if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0 || strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0)
  {
    thread_args *args = malloc(sizeof(thread_args));
    args->id = contador_hilos;
    args->pid = pcb_a_enviar->pid;
    if(strcmp(ALGORITMO_PLANIFICACION, "RR") == 0)
    {
      args->tiempo = QUANTUM; // ADAPTAR A VRR
    }
    else //es VRR
    {
      args->tiempo = buscarQPrima(pcb_a_enviar->pid);
      //log_trace(kernel_log_debug, "PID: %d - EL Q PRIMA QUE ASIGNE ES: %d", args->pid,args->tiempo);
    }

    int *numerillo = malloc(sizeof(int));
    *numerillo = contador_hilos;

    list_add(lista_id_hilos, numerillo);

    pthread_t threadXD;
    pthread_create(&threadXD, NULL, contador_tiempos, args);
    pthread_detach(threadXD);
  }

  // pthread_mutex_lock(&modificarLista);
  list_remove_element(procesosREADY, (void *)pcb_a_enviar->pid);
  list_remove_element(listaPCBs, (void *)pcb_a_enviar);
  // pthread_mutex_unlock(&modificarLista);
  contador_hilos++;
}

void nuevaListaRecursos(int pid)
{
  pidConRecursos_t *pidRecAux = malloc(sizeof(pidConRecursos_t));
  pidRecAux->pid = pid;
  pidRecAux->recursosTomados = list_create();
  for (int i = 0; i < nombresRecursos[i] != NULL; i++)
  {
    int *recurso_i = malloc(sizeof(int));
    *recurso_i = 0;
    list_add(pidRecAux->recursosTomados, recurso_i);
    // printf("recurso: %s, instancias usadas: %d\n", nombresRecursos[i], *recurso_i);
  }
  list_add(listaPidsRecursos, pidRecAux);
}

void iniciar_proceso(char *path)
{
  PCB *pcb = iniciar_PCB(path);
  printf("el pid es %d\n", pcb->pid);

  pthread_mutex_lock(&lista_pcb_mutex);
  list_add(listaPCBs, pcb);
  pthread_mutex_unlock(&lista_pcb_mutex);

  enviar_path_memoria(path, pcb->pid);

  list_add(procesosNEW, &(pcb->pid)); // agrego el pcb al planificador de pids
  nuevaListaRecursos(pcb->pid);

  sem_post(&nuevo_bucle);
}

void liberarRecursosProceso(int *pid)
{
  pidConRecursos_t *pidRec = encontrarPidRecursos(*pid);
  for (int i = 0; nombresRecursos[i] != NULL; i++)
  {
    int *recurso_i = list_get(pidRec->recursosTomados, i);
    while (*recurso_i > 0) // mientras haya instancias pedidas, simulo un signal para no repetir logica
    {
      atender_signal(nombresRecursos[i], pid);
      recurso_i = list_get(pidRec->recursosTomados, i); // actualizo instancias_i
    }
  }
}

interfaces_io *encontrar_interfaz(char *nombre_buscado)
{
  for (int i = 0; i < list_size(lista_interfaces); i++)
  {
    interfaces_io *elemento = list_get(lista_interfaces, i);

    // printf("###### nombre de la interfaz: %s\n", elemento->nombre_interfaz);
    // printf("###### tipo de la interfaz: %s\n", elemento->tipo_interfaz);

    if (strcmp(elemento->nombre_interfaz, nombre_buscado) == 0)
    {
      return elemento;
    }
  }
  return NULL; // No se encontró la interfaz con el nombre buscado
}

int obtener_fd_interfaz(char *nombre_interfaz)
{
  interfaces_io *interfazADS = encontrar_interfaz(nombre_interfaz);

  if (interfazADS == NULL)
  {
    printf("Interfaz no encontrada: %s\n", nombre_interfaz);
    return -1; // o algún valor de error apropiado
  }

  printf("nombre de la interfaz: ######%s\n", interfazADS->nombre_interfaz);
  printf("tipo de la interfaz:###### %s\n", interfazADS->tipo_interfaz);
  return interfazADS->fd_interfaz;
}

bool admiteOperacionInterfaz(char *nombre_interfaz, char *tipo_instruccion)
{ // nombre = pepe, tipo_instruccion = gen_sleep, tipo_interfaz = GENERICA

  interfaces_io *interfazEncontrada = encontrar_interfaz(nombre_interfaz);

  char *tipoInterfaz = interfazEncontrada->tipo_interfaz;

  if (strcmp(tipoInterfaz, "GENERICA") == 0)
  {

    if (strcmp(tipo_instruccion, "IO_GEN_SLEEP") == 0)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  if (strcmp(tipoInterfaz, "STDIN") == 0)
  {

    if (strcmp(tipo_instruccion, "IO_STDIN_READ") == 0)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  if (strcmp(tipoInterfaz, "STDOUT") == 0)
  {

    if (strcmp(tipo_instruccion, "IO_STDOUT_WRITE") == 0)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  if (strcmp(tipoInterfaz, "DIALFS") == 0)
  {

    if (strcmp(tipo_instruccion, "IO_FS_CREATE") == 0 || strcmp(tipo_instruccion, "IO_FS_DELETE") == 0 || strcmp(tipo_instruccion, "IO_FS_TRUNCATE") == 0 || strcmp(tipo_instruccion, "IO_FS_WRITE") == 0 || strcmp(tipo_instruccion, "IO_FS_READ") == 0)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  return false;
}

int list_index_of(t_list *self, void *data, bool (*comp)(void *, void *))
{
  int index = 0;
  t_link_element *current = self->head;
  while (current != NULL)
  {
    if (comp(current->data, data))
    {
      return index;
    }
    current = current->next;
    index++;
  }
  return -1; // Elemento no encontrado
}

bool comparar_enteros(void *a, void *b)
{
  int *intA = (int *)a;
  int *intB = (int *)b;
  return (*intA == *intB);
}

void consultar_pid_cpu()
{
  t_buffer *a_enviar = crear_buffer();

  a_enviar->size = 0;
  a_enviar->stream = NULL;

  cargar_int_al_buffer(a_enviar, 0);

  t_paquete *un_paquete = crear_super_paquete(CONSULTA_PID, a_enviar);
  enviar_paquete(un_paquete, fd_cpu_dispatch);
  destruir_paquete(un_paquete);

  printf("toy esperando\n");
  // sem_wait(&esperar_consulta_pid);
  printf("pase el semaforo\n");
}

void mandar_a_exit(int *pid_finalizado)
{

  // consultar_pid_cpu();
  printf("esta ejecutando: %d\n", estaEJecutando);
  printf("pid finalizado: %d\n", *pid_finalizado);

  if (estaEJecutando == *pid_finalizado)
  {
    // estaEJecutando = 0;
    printf("mande a desalojar el que esta ejecutando\n");
    desalojoFinProceso();
  }

  bool seEncontroElPid = false;
  int *numero_desalojado = malloc(sizeof(int));
  numero_desalojado = -1;
  int *numerito;

  for (int i = 0; i < list_size(procesosNEW); i++)
  {

    numerito = list_get(procesosNEW, i);

    if (*numerito == *pid_finalizado)
    {

      numero_desalojado = list_remove(procesosNEW, i);
      printf("elimine a un wachin de new\n");
    }
  }

  for (int i = 0; i < list_size(procesosREADY); i++)
  {

    numerito = list_get(procesosREADY, i);

    if (*numerito == *pid_finalizado)
    {
      pthread_mutex_lock(&proteger_lista_ready);
      numero_desalojado = list_remove(procesosREADY, i);
      pthread_mutex_unlock(&proteger_lista_ready);
      printf("elimine a un wachin de ready\n");
    }
  }

  for (int i = 0; i < list_size(procesosSuspendidos); i++)
  {

    numerito = list_get(procesosSuspendidos, i);

    if (*numerito == *pid_finalizado)
    {

      numero_desalojado = list_remove(procesosSuspendidos, i);
      printf("elimine a un wachin de suspended\n");
    }
  }

  for(int i=0;nombresRecursos[i]!=NULL;i++) //mira los bloqueados por recursos
  {
    printf("el pid: %d\n",*pid_finalizado);
    if(i < list_size(lista_recursos_y_bloqueados))
    {
      t_list *lista = list_get(lista_recursos_y_bloqueados, i); // PREGUNTAR A LUCA ACA HAY UN SEGMENTATION

      for (int j = 0; j < list_size(lista); j++)
      {

        int *numeroSuerte = list_get(lista, j);

        if (*numeroSuerte == *pid_finalizado)
        {

          numero_desalojado = list_remove(lista, j);
          printf("elimine a un wachin del recurso\n");
        }
      }
    }
    
  }
  /*
    for(int i = 0; nombresRecursos[i] != NULL ; i++){

          t_list *lista_donde_agregar = list_get(lista_recursos_y_bloqueados, i);

          log_debug(kernel_log_debug, "Esta bloqueado por el recurso: %s", nombresRecursos[i]);

          for(int j = 0; j < list_size(lista_donde_agregar); j++){

              int* numeroXD = list_get(lista_donde_agregar,j);
              log_debug(kernel_log_debug, "PID: %d", *numeroXD);
          }
      }*/

  for (int i = 0; i < list_size(lista_interfaces); i++) // mira los bloqueados por IO
  {
    interfaces_io *interfaz = list_get(lista_interfaces, i);
    int numero_a_buscar = *pid_finalizado;
    int index = -1;
    index = list_index_of(interfaz->procesos_bloqueados->elements, &numero_a_buscar, comparar_enteros);

    if (index > -1 && queue_size(interfaz->instrucciones_ejecutar) >= index)
    {
      list_remove(interfaz->instrucciones_ejecutar->elements, index);
    }

    seEncontroElPid = list_remove_element(interfaz->procesos_bloqueados->elements, pid_finalizado);
  }

  if (numero_desalojado != -1)
  {
    list_add(procesosEXIT, pid_finalizado);
  }
}
