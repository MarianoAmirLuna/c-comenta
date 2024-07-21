#ifndef FUNCIONES_CPU_H_
#define FUNCIONES_CPU_H_

#include "c_gestor.h"

void procesamiento_cpu();
void ejecutar_instruccion(char* instruccion, PCB* pcb);
bool huboCambioContexto(int pidAEjecutar);
void procesar_instruccion();
void actualizarPCB();
nombre_instruccion str_to_instruction(const char* instr);
bool es4bytes(char* instruccion);
void _set(char* registro,char* valor);
void hacerMovIn( int dirLogicaDelDato, int tamanioDatoALeer,char* registroDatos);
void hacerMovOut(int direccionLogica, void *dato, int tamanio_dato);
void _mov_in(char* registroDatos,char* registroDireccion);
void _mov_out(char* registroDireccion, char* registroDatos);
void _sum(char* registroDestino, char* registroOrigen);
void _sub(char* registroDestino,char* registroOrigen);
void _jnz(char* registro,char* instruccion);
void _resize(char* tamanio);
void _copy_string(char* tamanio);
void ioGenSleep(char *nombreInterfaz, char *unidadesTrabajo);
int traducir_dl(int direccionLogica);
uint32_t *get_registry(char *registro);
void mandarDatoAEscribir(int direccion_logica,int direccion_fisica,int segundaDF ,void *queEscribir, int bytes_a_escribir,int seEscribe2paginas, int tamanioRestantePagina);
void mandarDatoALeer(int dirFisicaDelDato,int segundaDF ,int bytesALeer,int seEscribe2paginas,int tamanioRestantePagina,char* registroDatos);
void mandarSegundaDireccionALeer(int direccion_fisica,int bytes_restantes_en_pagina);
int conocerTamanioDeLosRegistros(char *registro);
int buscarMarcoTLB(int pid, int pagina);
bool condicion_id_pagina(void *elemento);
void agregarPaginaTLB(int pid,int pagina,int marco);
void actualizarPrioridadesTLB(lineaTLB lineaTL);
lineaTLB* inicializarLineaTLB(int pid, int pagina, int marco);
void concat_uint8_to_string(char* str, uint8_t ch);
int obtener_cant_direcciones(int direccionLogica,int tamanioAEscribir, int bytes_restantes_en_pagina);
void escribir_string_memoria(char* datoEscribir, int direccionLogica);
void _wait(char* recurso);
void _signal(char* recurso);
void devolverPCBKernel();
void devolverPCBKernelSenial();
void io_stdout_write(char* interfaz,char* direccionLogica,char* tamanio);
void leerStringMemoria(int direccionLogica, int tamanio);
int is_8bit_register(char *registro);
void leerCaracterMemoria(int direccionLogica);
void devolverPCBKernel_exit_o_bloqueado();
void establecerVariablesNecesarias(char* tipoInterfaz,char* nombreInterfaz);
void ioSTDINRead(char* interfaz,char* registro_direccion,char* registro_tamanio);
void solicitarTamanioPagina();
bool instruccion_es_tipo_io(char *instruccion_actual);
int obtenerValorRegistro(char* registro);

#endif