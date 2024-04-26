#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_


#include "k_gestor.h"

#include "inicializar_kernel.h"
t_log* kernel_logger;
t_log* kernel_log_debug;
t_config* kernel_config;

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

#endif