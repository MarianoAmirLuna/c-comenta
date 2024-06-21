#ifndef MEMORIA_KERNEL_H_
#define MEMORIA_KERNEL_H_

#include "m_gestor.h"
#include <utils/shared.h>
#include "../include/funciones_memoria.h"

void atender_memoria_kernel();
void atender_crear_proceso(t_buffer* un_buffer);
path_conID *iniciar_path_id(int id, char *path);

#endif