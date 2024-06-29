#ifndef FUNCIONES_CPU_H_
#define FUNCIONES_CPU_H_

#include "c_gestor.h"

void procesamiento_cpu();
void ejecutar_instruccion(char* instruccion, PCB* pcb);
bool huboCambioContexto(int pidAEjecutar);
void procesar_instruccion(int pidAEjecutar);
void actualizarPCB();
nombre_instruccion str_to_instruction(const char* instr) ;
bool es4bytes(char* instruccion);
void _set(char* registro,char* valor);
void hacerMovIn(int dirLogicaDelDato, int tamanioDatoALeer);
void hacerMovOut(int direccionLogica, void *dato, int tamanio_dato);
void _mov_in(char* registroDatos,char* registroDireccion);
void _mov_out(char* registroDireccion, char* registroDatos);
void _sum(char* registroDestino, char* registroOrigen);
void _sub(char* registroDestino,char* registroOrigen);
void _jnz(char* registro,char* instruccion);
void _resize(char* tamanio);
void _copy_string(char* tamanio);
int traducir_dl(int direccionLogica);
uint32_t *get_registry(char *registro);
void mandarDatoAEscribir(int direccion_logica,int direccion_fisica,int segundaDF ,void *queEscribir, int bytes_a_escribir,int seEscribe2paginas, int tamanioRestantePagina);
void mandarDatoALeer(int dirFisicaDelDato,int segundaDF ,int bytesALeer,int seEscribe2paginas,int tamanioRestantePagina);
void mandarSegundaDireccionALeer(int direccion_fisica,int bytes_restantes_en_pagina);
int conocerTamanioDeLosRegistros(char *registro);

#endif