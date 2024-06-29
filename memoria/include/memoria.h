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
int TAM_MEMORIA;
int TAM_PAGINA;
char* PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;
int socket_cliente_CPU;
int socket_cliente_KERNEL;
int socket_cliente_IO;

int id_lamba;
t_list* list_path_id;
t_list* list_pcb;
t_list* listaPaginas;
t_list* listaTablaPaginas;
int id_global;
int id_global_pcb;
void* memoriaPrincipal;
int cant_frames_ppal;
t_bitarray* frames_ocupados_ppal;
sem_t esperar_df;
int dir_fisica_global;
void* dataParte2Global;
int cuantoFaltabaEscribir;
int dirFisicaDondeHayQueAlmacenarGlobal;
int cuantoFaltaLeer;


#endif