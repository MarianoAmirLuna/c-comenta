#ifndef FUNCIONES_MEMORIA_H_
#define FUNCIONES_MEMORIA_H_
#define MAX_LEN 100

#include "m_gestor.h"
#include <utils/shared.h>

int bitsToBytes(int bits);
char* asignarMemoriaBits(int bits);
void iniciarPaginacion();
t_list *buscarFramesSinOcupar();
char* asignarMemoriaBits(int bits);
int bitsToBytes(int bits);
t_list* reservarFrames(tablaPaginas *tablaPags, int cantidadPaginasNecesarias);
tablaPaginas inicializarTablaPaginas(int pid);
int calcularPaginasNecesarias(int tamanio);
bool puedeCargarloCompleto(int tamanioAcumulado, int tamanioQuiereCargar);
char* obtenerInstruccion(char* path, int programCounter);
char* dividirStringIzquierda( char* instruccion, int tamanio);
char* dividirStringDerecha(char *instruccion, int tamanio);
void cargarRegistro(int frame,int tamanioAcumulado,char* instruccion);
int contarBitValidez(tablaPaginas *tabla);
void liberarFrames(tablaPaginas* tablaPags, int cantidadPaginasBorrar);


#endif