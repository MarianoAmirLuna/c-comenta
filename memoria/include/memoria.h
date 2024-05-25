#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "m_gestor.h"
#include "inicializar_memoria.h"
#include "memoria_kernel.h"
#include "memoria_io.h"
#include "memoria_cpu.h"

t_log* memoria_logger;
t_log* memoria_log_debug;
t_config* memoria_config;

int fd_memoria;
int fd_kernel;
int fd_io;
int fd_cpu;

char* PUERTO_ESCUCHA;
char* TAM_MEMORIA;
char* TAM_PAGINA;
char* PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;
int socket_cliente_CPU;
int socket_cliente_KERNEL;
int socket_cliente_IO;

int id_lamba;
t_list* list_path_id;
int id_global;


#endif