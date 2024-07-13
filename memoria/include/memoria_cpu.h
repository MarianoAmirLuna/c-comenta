#ifndef MEMORIA_CPU_H_
#define MEMORIA_CPU_H_

#include "m_gestor.h"
#include "utils/shared.h"
#include "../include/funciones_memoria.h"

void atender_memoria_cpu();
void devolver_instruccion(t_buffer *un_buffer);
void devolverTamanioPagina(t_buffer *un_buffer);
void buscarMarco(t_buffer* un_buffer);
char *obtener_instruccion_lista(int pid, int program_counter);
bool condition_id_igual_n(void *elemento);
char* obtenerInstruccion(char* path, int programCounter);
//char* obtener_tabla_pagina(int pid, int program_counter);
bool condition_tabla_pagina(void *elemento);
tablaPaginas* obtener_tabla_pagina(int pid);
void imprimirBitsValidez(tablaPaginas p);
void escribirDato(t_buffer *un_buffer);
void leerDato(t_buffer *un_buffer);
void obtenerCantInstrucciones(int pid);
void escribirCPYSTRING(t_buffer *un_buffer);
bool necesitoNuevaDF(t_list* cortesPagina, int cantIteraciones);
void obtenerCortesDePagina(t_list* lista,int tamanio_a_escribir, int restante_pagina);
void imprimirBitmapMemoriaPrincipal();
void terminoInstruccionMemoria();


#endif