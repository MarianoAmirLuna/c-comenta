#ifndef I_GESTOR_H_
#define I_GESTOR_H_


#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <pthread.h>
#include <commons/log.h>

//Variables GLOBALES

extern t_log* io_logger;
extern t_log* io_log_debug;
extern t_config* io_config;

extern int fd_memoria;
extern int fd_kernel;

extern char* TIPO_INTERFAZ;
extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* IP_KERNEL;
extern char* PUERTO_KERNEL;
extern int TIEMPO_UNIDAD_TRABAJO;
extern char* PATH_BASE_DIALFS;
extern int BLOCK_SIZE;
extern int BLOCK_COUNT;
extern int RETRASO_COMPACTACION;


#endif