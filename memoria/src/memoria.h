#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdlib.h>
#include <stdio.h>

#include <commons/log.h>
#include <commons/config.h>

t_log* memoria_logger;
t_log* memoria_log_debug;
t_config* memoria_config;

char* PUERTO_ESCUCHA;
char* TAM_MEMORIA;
char* TAM_PAGINA;
char* PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;

#endif