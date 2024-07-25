#ifndef INICIALIZAR_IO_H_
#define INICIALIZAR_IO_H_

#include "i_gestor.h"

void inicializar_io(char* rutaconfig);
void iniciar_logs();
void iniciar_config_io(char* rutaconfig); //capaz haya que eliminar esto de iniciar config de io nada mas arrancar
void imprimir_config();
void iniciar_semaforos();

#endif