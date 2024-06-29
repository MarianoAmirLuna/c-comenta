#ifndef CPU_H_
#define CPU_H_

#include "c_gestor.h"
#include "inicializar_cpu.h"
#include "cpu_memoria.h"
#include "cpu_kernel_dispatch.h"
#include "cpu_kernel_interrupt.h"
#include "funciones_cpu.h"

//Variables GLOBALES

t_log* cpu_logger;
t_log* cpu_log_debug;
t_config* cpu_config;

int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_kernel_dispatch;
int fd_kernel_interrupt;
int fd_memoria;

char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* PUERTO_ESCUCHA_DISPATCH;
char* PUERTO_ESCUCHA_INTERRUPT;

PCB pcb_ejecucion;
sem_t arrancar_cpu;
sem_t wait_instruccion;
sem_t pcb_actualizado;
sem_t esperarTamanioDePagina;
sem_t esperarMarco;
sem_t esperarEscrituraDeMemoria;
sem_t esperarLecturaDeMemoria;

char* instruccion_actual;
bool cambioContexto;
bool hayPcbEjecucion;
bool primeraSolicitudTamanioDePagina;
int tamanio_pagina;
int marco;

#endif