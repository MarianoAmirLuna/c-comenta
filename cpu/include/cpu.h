#ifndef CPU_H_
#define CPU_H_

#include "c_gestor.h"
#include "inicializar_cpu.h"

//Variables GLOBALES

t_log* cpu_logger;
t_log* cpu_log_debug;
t_config* cpu_config;

char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* PUERTO_ESCUCHA_DISPATCH;
char* PUERTO_ESCUCHA_INTERRUPT;

#endif