
#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include "../include/funciones_cpu.h"
#include <semaphore.h>
#include <inttypes.h>

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

void mandarDatoAEscribir(int direccion_logica,int direccion_fisica, void *queEscribir, int bytes_a_escribir,int seEscribe2paginas, int tamanioRestantePagina)
{
    t_buffer *a_enviar = crear_buffer();
    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_int_al_buffer(a_enviar, direccion_logica);
    cargar_int_al_buffer(a_enviar, direccion_fisica);
    cargar_int_al_buffer(a_enviar, bytes_a_escribir);
    
    if(bytes_a_escribir == 1){
        uint8_t* dato = (uint8_t*)queEscribir;
        cargar_uint8_al_buffer(a_enviar,*dato);
    }
    else{
        uint32_t* dato = (uint32_t*)queEscribir;
        cargar_uint32_al_buffer(a_enviar,*dato);
    }

    cargar_int_al_buffer(a_enviar, seEscribe2paginas);
    cargar_int_al_buffer(a_enviar, tamanioRestantePagina);

    t_paquete *un_paquete = crear_super_paquete(MANDAR_DATO_A_ESCRIBIR, a_enviar);
    enviar_paquete(un_paquete, fd_memoria);
    destruir_paquete(un_paquete);

    sem_wait(&esperarEscrituraDeMemoria);
}

void hacerMovOut(int direccionLogica, void *dato, int tamanio_dato)
{
    // No tengo el tam_pag
    if (primeraSolicitudTamanioDePagina)
    {
        solicitarTamanioPagina();
        primeraSolicitudTamanioDePagina = false;
    }

    int desplazamiento_en_pagina = direccionLogica % tamanio_pagina;           // offset
    int bytes_restantes_en_pagina = tamanio_pagina - desplazamiento_en_pagina; // cuanto queda en la pagina
    int direccion_fisica = traducir_dl(direccionLogica);

    if(tamanio_dato == 1){ //si es un u8
       
       mandarDatoAEscribir(direccionLogica,direccion_fisica,dato,1,0,bytes_restantes_en_pagina);
        
    }
    else{ //si es un u32
       
       if(bytes_restantes_en_pagina < 4){ //se tiene que escribir en 2 paginas diferentes
           mandarDatoAEscribir(direccionLogica,direccion_fisica,dato,4,1,bytes_restantes_en_pagina);

       }
       else{ //no se tiene que escribir en 2 paginas diferentes
           mandarDatoAEscribir(direccionLogica,direccion_fisica,dato,4,0,bytes_restantes_en_pagina);
       }

    }
}

int conocerTamanioDeLosRegistros(char *registro)
{

    if (strcmp(registro, "AX") == 0 || strcmp(registro, "BX") == 0 || strcmp(registro, "CX") == 0 || strcmp(registro, "DX") == 0)
    {
        return 1;
    }
    else
    {
        return 4;
    }
}

void _mov_out(char *registroDireccion, char *registroDatos)
{

    // obtengo el valor de los registros y se los paso a hacerMovOut
    void *direccionLogica = (void *)get_registry(registroDireccion);
    void *dato = (void *)get_registry(registroDatos);

    uint8_t *ptr_dato = (uint8_t *)dato;
    printf("el valor de 'dato' es: %" PRIu8 "\n", *ptr_dato);

    // Suponiendo que 'direccionLogica' apunta a un uint32_t
    uint32_t *ptr_direccionLogica = (uint32_t *)direccionLogica;
    printf("el valor de 'direccionLogica' es: %" PRIu32 "\n", *ptr_direccionLogica);

    int tamanioDato = conocerTamanioDeLosRegistros(registroDatos);

    printf("el tamanio del dato: %d\n", tamanioDato);

    int *dirLogic = (int *)direccionLogica;

    hacerMovOut(*dirLogic, dato, tamanioDato);
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

    t_paquete *paquete = crear_super_paquete(EJECUTAR_RESIZE, buffer);
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

void solicitarTamanioPagina()
{
    t_buffer *a_enviar = crear_buffer();

    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_int_al_buffer(a_enviar, 1);

    t_paquete *un_paquete = crear_super_paquete(DEVOLVER_TAMANIO_PAGINA, a_enviar);
    enviar_paquete(un_paquete, fd_memoria);
    destruir_paquete(un_paquete);

    sem_wait(&esperarTamanioDePagina);
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

void enviar_pedido_marco(int num_pag, int pid)
{

    t_buffer *a_enviar = crear_buffer();

    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_int_al_buffer(a_enviar, num_pag);
    cargar_int_al_buffer(a_enviar, pid);

    t_paquete *un_paquete = crear_super_paquete(DEVOLVER_MARCO, a_enviar);
    enviar_paquete(un_paquete, fd_memoria);
    destruir_paquete(un_paquete);

    printf("Antes del semanforo\n");

    sem_wait(&esperarMarco);

    printf("Pase el semanforo\n");
}

int traducir_dl(int direccionLogica)
{
    if (primeraSolicitudTamanioDePagina)
    {
        solicitarTamanioPagina();
        primeraSolicitudTamanioDePagina = false;
    }

    int num_pag = direccionLogica / tamanio_pagina;

    int desplazamiento = direccionLogica - num_pag * tamanio_pagina;

    enviar_pedido_marco(num_pag, pcb_ejecucion.pid);

    printf("llega el marco, falta calcular\n");

    int direccionFisica = marco * tamanio_pagina + desplazamiento;
    return direccionFisica;
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

    for (int i = 0; i < 6; i++) // temporal para las pruebas nada mas
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
