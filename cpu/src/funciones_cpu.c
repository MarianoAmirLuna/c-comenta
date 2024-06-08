
#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include "../include/funciones_cpu.h"
#include <semaphore.h>

void ejecutar_instruccion(char *instruccion, PCB *pcb)
{
    char operacion[20];
    char reg1[5], reg2[5];
    int valor, sleep_time;

    sscanf(instruccion, "%s", operacion);

    printf("Operación: %s\n", operacion); // Debug

    if (strcmp(operacion, "SET") == 0)
    {
        sscanf(instruccion, "SET %s %d", reg1, &valor);
        if (strcmp(reg1, "AX") == 0)
            pcb->registros_cpu.AX = valor;
        else if (strcmp(reg1, "BX") == 0)
            pcb->registros_cpu.BX = valor;
        else if (strcmp(reg1, "CX") == 0)
            pcb->registros_cpu.CX = valor;
        else if (strcmp(reg1, "DX") == 0)
            pcb->registros_cpu.DX = valor;
        else if (strcmp(reg1, "PC") == 0)
            pcb->program_counter = valor;
        printf("SET %s to %d\n", reg1, valor); // Debug
    }
    else if (strcmp(operacion, "SUM") == 0)
    {
        sscanf(instruccion, "SUM %s %s", reg1, reg2);
        if (strcmp(reg1, "AX") == 0 && strcmp(reg2, "BX") == 0)
            pcb->registros_cpu.AX += pcb->registros_cpu.BX;
        else if (strcmp(reg1, "AX") == 0 && strcmp(reg2, "CX") == 0)
            pcb->registros_cpu.AX += pcb->registros_cpu.CX;
        else if (strcmp(reg1, "AX") == 0 && strcmp(reg2, "DX") == 0)
            pcb->registros_cpu.AX += pcb->registros_cpu.DX;
        printf("SUM %s %s = %d\n", reg1, reg2, pcb->registros_cpu.AX); // Debug
    }
    else if (strcmp(operacion, "SUB") == 0)
    {
        sscanf(instruccion, "SUB %s %s", reg1, reg2);
        if (strcmp(reg1, "AX") == 0 && strcmp(reg2, "BX") == 0)
            pcb->registros_cpu.AX -= pcb->registros_cpu.BX;
        else if (strcmp(reg1, "AX") == 0 && strcmp(reg2, "CX") == 0)
            pcb->registros_cpu.AX -= pcb->registros_cpu.CX;
        else if (strcmp(reg1, "AX") == 0 && strcmp(reg2, "DX") == 0)
            pcb->registros_cpu.AX -= pcb->registros_cpu.DX;
        printf("SUB %s %s = %d\n", reg1, reg2, pcb->registros_cpu.AX); // Debug
    }
    else if (strcmp(operacion, "JNZ") == 0)
    {
        sscanf(instruccion, "JNZ %s %d", reg1, &valor);
        if (strcmp(reg1, "AX") == 0 && pcb->registros_cpu.AX != 0)
            pcb->program_counter = valor;
        else if (strcmp(reg1, "BX") == 0 && pcb->registros_cpu.BX != 0)
            pcb->program_counter = valor;
        else if (strcmp(reg1, "CX") == 0 && pcb->registros_cpu.CX != 0)
            pcb->program_counter = valor;
        else if (strcmp(reg1, "DX") == 0 && pcb->registros_cpu.DX != 0)
            pcb->program_counter = valor;
        printf("JNZ %s %d, nuevo PC = %d\n", reg1, valor, pcb->program_counter); // Debug
    }
    else if (strcmp(operacion, "IO_GEN_SLEEP") == 0)
    {
        sscanf(instruccion, "IO_GEN_SLEEP Int%d %d", &valor, &sleep_time);
        printf("Durmiendo por %d segundos...\n", sleep_time);
        sleep(sleep_time);
    }
}

void solicitar_instruccion(int pid, int program_counter)
{
    t_buffer *a_enviar = crear_buffer();

    a_enviar->size = 0;
    a_enviar->stream = NULL;

    printf("el pid en solicitar_instruccion es: %d\n",pid);
    printf("el program counter en solicitar instrucciones: %d\n",program_counter);

    cargar_int_al_buffer(a_enviar, pid);
    cargar_int_al_buffer(a_enviar, program_counter);

    t_paquete *un_paquete = crear_super_paquete(SOLICITUD_INSTRUCCION, a_enviar);
    enviar_paquete(un_paquete, fd_memoria);
    destruir_paquete(un_paquete);
}

bool huboCambioContexto(int pidAEjecutar)
{
    return pcb_ejecucion.pid != pidAEjecutar;
}

void actualizarPCB(int pidAEjecutar)
{
    t_buffer *a_enviar = crear_buffer();
    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_int_al_buffer(a_enviar,pidAEjecutar);

    t_paquete *un_paquete = crear_super_paquete(SOLICITUD_PCB, a_enviar);
    enviar_paquete(un_paquete, fd_memoria);
    destruir_paquete(un_paquete);

    sem_wait(&pcb_actualizado); //hacer el signal en cpu_memoria al recibir pcb
}

void termino_ejecutar(){
    t_buffer *a_enviar = crear_buffer();
    a_enviar->size = 0;
    a_enviar->stream = NULL;

    printf("envie el mensaje de CPU LISTA\n");

    cargar_string_al_buffer(a_enviar,"mariano es fachero");

    t_paquete *un_paquete = crear_super_paquete(CPU_LISTA, a_enviar);
    enviar_paquete(un_paquete, fd_kernel_dispatch);
    destruir_paquete(un_paquete);
}

void procesar_instruccion(int pidAEjecutar)
{
    cambioContexto = huboCambioContexto(pidAEjecutar);

    if (cambioContexto || pcb_ejecucion.pid == -1)
    {
        actualizarPCB(pidAEjecutar);
        printf("se actualizo el pcb\n");
    }

    //hasta aca anda
    printf("se solicito la instruccion\n");
    solicitar_instruccion(pcb_ejecucion.pid, pcb_ejecucion.program_counter); 

    sem_wait(&wait_instruccion);

    printf("se ejecuto la instruccion\n");
    ejecutar_instruccion(instruccion_actual, &pcb_ejecucion);

    printf("Estado de los registros:\n");
    printf("AX: %d, BX: %d, CX: %d, DX: %d\n", pcb_ejecucion.registros_cpu.AX, pcb_ejecucion.registros_cpu.BX, pcb_ejecucion.registros_cpu.CX, pcb_ejecucion.registros_cpu.DX);
    printf("EAX: %u, EBX: %u, ECX: %u, EDX: %u\n", pcb_ejecucion.registros_cpu.EAX, pcb_ejecucion.registros_cpu.EBX, pcb_ejecucion.registros_cpu.ECX, pcb_ejecucion.registros_cpu.EDX);
    printf("PC: %d\n\n", pcb_ejecucion.program_counter);
    printf("--------------------------------\n\n");

    pcb_ejecucion.program_counter++;

    //error: Desconexion de CPU - MEMORIA

    termino_ejecutar();

}
