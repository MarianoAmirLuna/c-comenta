#ifndef CPU_H_
#define CPU_H_

#include "i_gestor.h"
#include "io_memoria.h"
#include "io_kernel.h"

//Variables GLOBALES

t_log* io_logger;
t_log* io_log_debug;
t_config* io_config;
t_bitarray* bitarray_bloques;

int fd_memoria;
int fd_kernel;
char *nombreInterACrear;
char *direccionConfigInterCrear;


char* TIPO_INTERFAZ; //Los nombres de las variables estan repetidos generando problemas, por eso los comento, capaz haya que eliminar la creacion de un config inicial y estas variables
char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* IP_KERNEL;
char* PUERTO_KERNEL;
int TIEMPO_UNIDAD_TRABAJO;
char* PATH_BASE_DIALFS;
int BLOCK_SIZE;
int BLOCK_COUNT;
int RETRASO_COMPACTACION;
sem_t termino_ejecutar;
sem_t esperar_palabra_memoria;
char* palabraIOWrite;


#endif