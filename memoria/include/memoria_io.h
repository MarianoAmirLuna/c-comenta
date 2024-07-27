#ifndef MEMORIA_IO_H_
#define MEMORIA_IO_H_

#include "m_gestor.h"

void atender_creacion_interfaz();
void leerMemoria(t_buffer *un_buffer);
int encontrar_fd_interfaz(char *nombre_buscado);
void aviso_finalizacion_a_IO(int fd);
void leerMemoriaFS(t_buffer *un_buffer);

#endif