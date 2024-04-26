#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include "../include/cpu.h"


void *iniciar_conexion(void *ptr);
void *crear_servidor(void *ptr);




int main(void) {

	inicializar_cpu();

	return 0;
}
