
#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include "../include/funciones_cpu.h"
#include <semaphore.h>

void solicitar_instruccion(int pid, int program_counter)
{
    t_buffer *a_enviar = crear_buffer();

    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_int_al_buffer(a_enviar, pid);
    cargar_int_al_buffer(a_enviar, program_counter);

    t_paquete *un_paquete = crear_super_paquete(SOLICITUD_INSTRUCCION, a_enviar);
    enviar_paquete(un_paquete, fd_memoria);
    destruir_paquete(un_paquete);
}

void procesamiento_cpu()
{

    printf("toy esperando\n");
    sem_wait(&arrancar_cpu);
    printf("pase el wait\n");

    //printf("cantidad lineas txt %d\n",cantidad_lineas_txt_ejecutando);
    //printf("program counter ejecutando %d\n",pcb_ejecucion.program_counter);

    //while (cantidad_lineas_txt_ejecutando >= pcb_ejecucion.program_counter){

    solicitar_instruccion(pcb_ejecucion.pid, pcb_ejecucion.program_counter);
        //sleep(2);
    //}
}
