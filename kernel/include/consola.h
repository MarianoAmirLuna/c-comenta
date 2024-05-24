#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "k_gestor.h"
#include "servicios_kernel.h"

void iniciar_consola_interactiva();
bool _validacion_de_instruccion_de_consola(char* leido);
void _atender_instruccion_validada(char* leido);
void _f_iniciar_proceso(t_buffer* un_buffer);
void iniciar_cpu();

#endif /* CONSOLA_H_ */
