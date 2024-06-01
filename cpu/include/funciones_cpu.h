#ifndef FUNCIONES_CPU_H_
#define FUNCIONES_CPU_H_

#include "c_gestor.h"

void procesamiento_cpu();
void ejecutar_instruccion(char* instruccion, PCB* pcb);
bool huboCambioContexto(int pidAEjecutar);
void procesar_instruccion(int pidAEjecutar);
void actualizarPCB();

#endif