#ifndef MEMORIA_KERNEL_H_
#define MEMORIA_KERNEL_H_

#include "m_gestor.h"
#include <utils/shared.h>

void atender_memoria_kernel();
path_conID* deserializar(t_buffer* buffer);

#endif