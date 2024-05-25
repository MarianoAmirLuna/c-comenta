#ifndef FUNCIONES_KERNEL_H_
#define FUNCIONES_KERNEL_H_

#include "k_gestor.h"


void iniciar_proceso(char* path);
PCB iniciar_PCB();
void enviar_path_memoria(char*, int);
void iniciar_proceso(char*);
void iniciar_planificacion();
void ciclo_plani_FIFO();
void ciclo_plani_RR();
void ciclo_planificacion();

#endif

