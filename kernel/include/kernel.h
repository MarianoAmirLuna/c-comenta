#ifndef KERNEL_H_
#define KERNEL_H_

#include "k_gestor.h"

#include "inicializar_kernel.h"
#include "kernel_dispatch.h"
#include "kernel_interrupt.h"
#include "kernel_io.h"
#include "kernel_memoria.h"

t_log* kernel_logger;
t_log* kernel_log_debug;
t_config* kernel_config;

int fd_kernel;
int fd_memoria;
int fd_cpu_interrupt;
int fd_cpu_dispatch;
int fd_io;

char* PUERTO_ESCUCHA;
char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* IP_CPU;
char* PUERTO_CPU_DISPATCH;
char* PUERTO_CPU_INTERRUPT;
char* ALGORITMO_PLANIFICACION;
int QUANTUM;
char* RECURSOS;
char* INSTANCIAS_RECURSOS;
int GRADO_MULTIPROGRAMACION;

int identificador_PID = 1;
int contador_pcbs = 1;

pthread_mutex_t mutex_pid;
//pthread_mutex_t modificarLista;
sem_t esta_cpu_libre;

#endif