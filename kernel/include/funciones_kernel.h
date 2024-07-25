#ifndef FUNCIONES_KERNEL_H_
#define FUNCIONES_KERNEL_H_

#include "k_gestor.h"

void iniciar_proceso(char* path);
PCB* iniciar_PCB();
void enviar_path_memoria(char*, int);
void iniciar_proceso(char*);
void iniciar_planificacion();
void ciclo_plani_FIFO();
void ciclo_plani_RR();
void ciclo_planificacion();
void iniciar_cpu();
void finalizarProceso(int pid);
pidConQ *nuevoPidConQ(int);
PCB *buscarPCB(int pid);
void mandarNuevoPCB();
void atender_signal(char*, int*);
void atender_wait(char*, int*);
void liberarRecursosProceso(int *);
void estado_instancias();
interfaces_io* encontrar_interfaz(char* nombre_buscado);
int obtener_fd_interfaz(char* nombre_interfaz);
bool admiteOperacionInterfaz(char* nombre_interfaz,char* tipo_interfaz);
void iniciar_planificacion_io();
void mandar_a_exit(int*);
void temporizadorQuantum(int quantum);
int tiempo_transcurrido_milisegundos(struct timespec start, struct timespec end);
void actualizarQPrimaProceso(int pid,int tiempo);
void enviar_pcb(PCB pcb, int socket_enviar,int numerillo);
void avisarDesalojo(int pid);
int buscarQPrima(int pid);

//int seguirPlanificando;
//sem_t sem_cpu_libre;
//int ejecutandoProceso;

#endif

