#ifndef CPU_H_
#define CPU_H_

#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <pthread.h>
#include <commons/log.h>

//Variables GLOBALES

t_log* io_logger;
t_log* io_log_debug;
t_config* io_config;

char* TIPO_INTERFAZ;
char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* IP_KERNEL;
char* PUERTO_KERNEL;
int TIEMPO_UNIDAD_TRABAJO;
char* PATH_BASE_DIALFS;
int BLOCK_SIZE;
int BLOCK_COUNT;
int RETRASO_COMPACTACION;

#endif