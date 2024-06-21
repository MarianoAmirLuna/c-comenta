
#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include "../include/funciones_cpu.h"
#include <semaphore.h>

bool es4bytes(char *registro)
{

    if (strncmp(registro, "E", 1) == 0 ||
        strncmp(registro, "SI", 2) == 0 ||
        strncmp(registro, "DI", 2) == 0 ||
        strncmp(registro, "PC", 2) == 0)
    {
        return true;
    }

    return false;
}

uint32_t *get_registry(char *registro)
{

    if (strcmp(registro, "AX") == 0)
        return &pcb_ejecucion.registros_cpu.AX;
    else if (strcmp(registro, "BX") == 0)
        return &pcb_ejecucion.registros_cpu.BX;
    else if (strcmp(registro, "CX") == 0)
        return &pcb_ejecucion.registros_cpu.CX;
    else if (strcmp(registro, "DX") == 0)
        return &pcb_ejecucion.registros_cpu.DX;
    if (strcmp(registro, "EAX") == 0)
        return &pcb_ejecucion.registros_cpu.EAX;
    else if (strcmp(registro, "EBX") == 0)
        return &pcb_ejecucion.registros_cpu.EBX;
    else if (strcmp(registro, "ECX") == 0)
        return &pcb_ejecucion.registros_cpu.ECX;
    else if (strcmp(registro, "EDX") == 0)
        return &pcb_ejecucion.registros_cpu.EDX;
    else if (strcmp(registro, "PC") == 0)
        return &pcb_ejecucion.program_counter;
    else if (strcmp(registro, "SI") == 0)
        return &pcb_ejecucion.registros_cpu.SI;
    else if (strcmp(registro, "DI") == 0)
        return &pcb_ejecucion.registros_cpu.DI;
    else
    {
        return NULL;
    }
}

void _set(char *registro, char *valor)
{
    uint32_t *destino = get_registry(registro);
    *destino = atoi(valor);
}

void _mov_in(char *registroDatos, char *registroDireccion)
{
}

void _mov_out(char *registroDireccion, char *registroDatos)
{
}

void _sum(char *registroDestino, char *registroOrigen)
{
    uint32_t *destino = get_registry(registroDestino);
    uint32_t *origen = get_registry(registroOrigen);

    *destino = *destino + *origen;
}

void _sub(char *registroDestino, char *registroOrigen)
{
    uint32_t *destino = get_registry(registroDestino);
    uint32_t *origen = get_registry(registroOrigen);

    *destino = *destino - *origen;
}

void _jnz(char *registro, char *instruccion)
{
}

void _resize(char *tamanio)
{
    int tamanio2 = atoi(tamanio);

   t_buffer *buffer = crear_buffer();
   buffer->size = 0;
   buffer->stream = NULL;

   cargar_int_al_buffer(buffer, pcb_ejecucion.pid);
   cargar_int_al_buffer(buffer, tamanio2);

   t_paquete *paquete = crear_super_paquete(EJECUTAR_RESIZE,buffer);
   enviar_paquete(paquete, fd_memoria);
   destruir_paquete(paquete);

}

void _copy_string(char *tamanio)
{
}

// faltan las demas

nombre_instruccion str_to_instruction(const char *instr)
{
    if (strcmp(instr, "SET") == 0)
        return SET;
    if (strcmp(instr, "MOV_IN") == 0)
        return MOV_IN;
    if (strcmp(instr, "MOV_OUT") == 0)
        return MOV_OUT;
    if (strcmp(instr, "SUM") == 0)
        return SUM;
    if (strcmp(instr, "SUB") == 0)
        return SUB;
    if (strcmp(instr, "JNZ") == 0)
        return JNZ;
    if (strcmp(instr, "RESIZE") == 0)
        return RESIZE;
    if (strcmp(instr, "COPY_STRING") == 0)
        return COPY_STRING;
    if (strcmp(instr, "WAIT") == 0)
        return WAIT;
    if (strcmp(instr, "SIGNAL") == 0)
        return SIGNAL;
    if (strcmp(instr, "IO_GEN_SLEEP") == 0)
        return IO_GEN_SLEEP;
    if (strcmp(instr, "IO_STDIN_READ") == 0)
        return IO_STDIN_READ;
    if (strcmp(instr, "IO_STDOUT_WRITE") == 0)
        return IO_STDOUT_WRITE;
    if (strcmp(instr, "IO_FS_CREATE") == 0)
        return IO_FS_CREATE;
    if (strcmp(instr, "IO_FS_DELETE") == 0)
        return IO_FS_DELETE;
    if (strcmp(instr, "IO_FS_TRUNCATE") == 0)
        return IO_FS_TRUNCATE;
    if (strcmp(instr, "IO_FS_WRITE") == 0)
        return IO_FS_WRITE;
    if (strcmp(instr, "IO_FS_READ") == 0)
        return IO_FS_READ;
    if (strcmp(instr, "EXIT") == 0)
        return EXIT;
    return INVALID_INSTRUCTION; // Si la instrucción no es válida
}

void ejecutar_instruccion(char *instruccion, PCB *pcb)
{
    char instr[20], param1[20], param2[20];

    sscanf(instruccion, "%s %s %s", instr, param1, param2);

    nombre_instruccion instruction = str_to_instruction(instr);

    switch (instruction)
    {
    case SET:
        _set(param1, param2);
        break;
    case MOV_IN:
        _mov_in(param1, param2);
        break;
    case MOV_OUT:
        _mov_out(param1, param2);
        break;
    case SUM:
        _sum(param1, param2);
        break;
    case SUB:
        _sub(param1, param2);
        break;
    case JNZ:
        _jnz(param1, param2);
        break;
    case RESIZE:
        _resize(param1);
        break;
    case COPY_STRING:
        _copy_string(param1);
        break;
    case WAIT:

        break;
    case SIGNAL:

        break;
    case IO_GEN_SLEEP:

        break;
    case IO_STDIN_READ:

        break;
    case IO_STDOUT_WRITE:

        break;
    case IO_FS_CREATE:

        break;
    case IO_FS_DELETE:

        break;
    case IO_FS_TRUNCATE:

        break;
    case IO_FS_WRITE:

        break;
    case IO_FS_READ:

        break;
    case EXIT:

        break;
    case INVALID_INSTRUCTION:

        break;
    }
}

void solicitar_instruccion(int pid, int program_counter)
{
    t_buffer *a_enviar = crear_buffer();

    a_enviar->size = 0;
    a_enviar->stream = NULL;

    printf("el pid en solicitar_instruccion es: %d\n", pid);
    printf("el program counter en solicitar instrucciones: %d\n", program_counter);

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

    cargar_int_al_buffer(a_enviar, pidAEjecutar);

    t_paquete *un_paquete = crear_super_paquete(SOLICITUD_PCB, a_enviar);
    enviar_paquete(un_paquete, fd_memoria);
    destruir_paquete(un_paquete);

    sem_wait(&pcb_actualizado); // hacer el signal en cpu_memoria al recibir pcb
}

void termino_ejecutar()
{
    t_buffer *a_enviar = crear_buffer();
    a_enviar->size = 0;
    a_enviar->stream = NULL;

    printf("envie el mensaje de CPU LISTA\n");

    cargar_string_al_buffer(a_enviar, "mariano es fachero");

    t_paquete *un_paquete = crear_super_paquete(CPU_LISTA, a_enviar);
    enviar_paquete(un_paquete, fd_kernel_dispatch);
    destruir_paquete(un_paquete);
}

void procesar_instruccion(int pidAEjecutar)
{
    // cambioContexto = huboCambioContexto(pidAEjecutar);

    // if (cambioContexto || pcb_ejecucion.pid == -1)
    //{
    //    actualizarPCB(pidAEjecutar);
    //    printf("se actualizo el pcb\n");
    //}

    // hasta aca anda
    // printf("se solicito la instruccion\n");
    sleep(1);

    for (int i = 0; i < 3; i++) // temporal para las pruebas nada mas
    {
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
    }

    // error: Desconexion de CPU - MEMORIA

    // termino_ejecutar();
}
