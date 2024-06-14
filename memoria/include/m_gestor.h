#ifndef M_GESTOR_H_
#define M_GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include<commons/bitarray.h>

extern t_log* memoria_logger;
extern t_log* memoria_log_debug;
extern t_config* memoria_config;

extern int fd_memoria;
extern int fd_kernel;
extern int fd_io;
extern int fd_cpu;

extern char* PUERTO_ESCUCHA;
extern int TAM_MEMORIA;
extern int TAM_PAGINA;
extern char* PATH_INSTRUCCIONES;
extern int RETARDO_RESPUESTA;

extern int socket_cliente_CPU;
extern int socket_cliente_KERNEL;
extern int socket_cliente_IO;

extern int id_lamba;
extern t_list* list_path_id;
extern t_list* list_pcb;
extern t_list* listaPaginas;
extern t_list* listaTablaPaginas;
extern int id_global;
extern int id_global_pcb;

extern void* memoriaPrincipal;
extern int cant_frames_ppal;
extern char* data2;
extern t_bitarray* frames_ocupados_ppal;

#endif