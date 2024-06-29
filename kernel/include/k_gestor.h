#ifndef K_GESTOR_H_
#define K_GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <readline/readline.h>

#include "utils/shared.h"

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

typedef enum{
    FIFO,
    RR,
    VRR
} t_planificacion;

//Variables GLOBALES

extern t_log* kernel_logger;
extern t_log* kernel_log_debug;
extern t_config* kernel_config;

extern int fd_kernel;
extern int fd_memoria;
extern int fd_cpu_interrupt;
extern int fd_cpu_dispatch;
extern int fd_io;

extern char* PUERTO_ESCUCHA;
extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* IP_CPU;
extern char* PUERTO_CPU_DISPATCH;
extern char* PUERTO_CPU_INTERRUPT;
extern char* ALGORITMO_PLANIFICACION;
extern int QUANTUM;
extern char* RECURSOS;
extern char* INSTANCIAS_RECURSOS;
extern int GRADO_MULTIPROGRAMACION;

extern int identificador_PID;
extern int contador_pcbs;

extern pthread_mutex_t mutex_pid;
extern pthread_mutex_t mutexExec;

extern t_list *procesosNEW;
extern t_list *procesosREADY;
extern t_list *procesosReadyConQ;
extern t_list *procesosSuspendidos;

typedef struct{
    int pid;
    int qPrima;
} pidConQ;

extern int procesoEXEC;
//extern pidConQ pidQEXEC;
//extern pidConQ PidQREADY;

extern t_dictionary *dictQPrimas; //una verga, el troubleshooting me va a tomar 2 años

extern t_list *listQPrimas;
extern int pidGlobal; //la unica implementacion que se me ocurre a las 2:08 AM

extern int sem_planificacion;

extern int quantum;

extern int tiempoTranscurrido;

extern t_planificacion tipoPlanificacion;

extern int seguirPlanificando;
extern sem_t sem_cpu_libre;
extern int ejecutandoProceso;

#endif
