#ifndef C_GESTOR_H_
#define C_GESTOR_H_

#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/collections/queue.h>

//Variables GLOBALES

extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;
extern int fd_kernel_dispatch;
extern int fd_kernel_interrupt;
extern int fd_memoria;

extern t_log* cpu_logger;
extern t_log* cpu_log_debug;
extern t_config* cpu_config;

extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* PUERTO_ESCUCHA_DISPATCH;
extern char* PUERTO_ESCUCHA_INTERRUPT;
extern sem_t arrancar_cpu;
extern sem_t wait_instruccion;
extern sem_t pcb_actualizado;
extern sem_t esperarTamanioDePagina;
extern sem_t esperarMarco;
extern sem_t esperarEscrituraDeMemoria;
extern sem_t esperarLecturaDeMemoria;
extern sem_t esperar_lectura_caracter;
extern sem_t esperarLecturaDeString;
//extern sem_t esperarCantInstrucciones;

extern PCB pcb_ejecucion;
extern char* instruccion_actual;
extern bool cambioContexto;
extern bool hayPcbEjecucion;
extern bool primeraSolicitudTamanioDePagina;
extern int tamanio_pagina;
extern int marco;
extern int cantInstucciones;
extern t_queue *cola_tlb;
extern char* ALGORITMO_TLB;
extern int CANTIDAD_ENTRADAS_TLB;
extern int id_global;
extern int pagina_global;
extern bool terminarPorExit;
extern bool terminaPorSenial;
extern int waitOSignal;
extern uint8_t caracterGlobal;
extern bool ejecute_instruccion_tipo_io;
extern bool terminePorOutOfMemory;
extern char* nombre_interfaz;
extern char* tipo_instruccion;
extern bool yaDevolvioPcb;
extern char* stringLeido;

extern t_list* procesosConPath;
extern int numeroID_hilo;


#endif