#ifndef MEMORIA_CPU_H_
#define MEMORIA_CPU_H_

#include "m_gestor.h"
#include "utils/shared.h"
#include "../include/funciones_memoria.h"

void atender_memoria_cpu();
void devolver_instruccion(t_buffer *un_buffer);
char *obtener_instruccion_lista(int pid, int program_counter);
bool condition_id_igual_n(void *elemento);
char* obtenerInstruccion(char* path, int programCounter);
//char* obtener_tabla_pagina(int pid, int program_counter);
bool condition_tabla_pagina(void *elemento);
tablaPaginas* obtener_tabla_pagina(int pid);
int contarBitsValidez(tablaPaginas* tabla);
void imprimirBitsValidez(tablaPaginas p);

#endif