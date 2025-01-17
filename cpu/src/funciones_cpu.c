
#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include "../include/funciones_cpu.h"
#include <semaphore.h>
#include <inttypes.h>
#include <string.h>


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
    else if (strcmp(registro, "EAX") == 0)
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
    else if (strcmp(registro, "AUX1") == 0)
        return &pcb_ejecucion.registros_cpu.AUX1;
    else if (strcmp(registro, "AUX2") == 0)
        return &pcb_ejecucion.registros_cpu.AUX2;
    else
    {
        return NULL;
    }
}

int is_8bit_register(char *registro)
{
    // Supongamos que los registros de 8 bits tienen un nombre específico que podemos identificar
    return strcmp(registro, "AX") == 0 || strcmp(registro, "BX") == 0 || strcmp(registro, "CX") == 0 || strcmp(registro, "DX") == 0;
}

void _set(char *registro, char *valor)
{
    /*uint32_t *destino = get_registry(registro);
     *destino = atoi(valor);*/
    if (is_8bit_register(registro))
    {
        // Obtener el puntero al registro de 8 bits
        uint8_t *destino8 = (uint8_t *)get_registry(registro);
        *destino8 = (uint8_t)atoi(valor);
    }
    else
    {
        // Obtener el puntero al registro de 32 bits
        uint32_t *destino32 = get_registry(registro);
        *destino32 = (uint32_t)atoi(valor);
    }
}

int conocerTamanioDeLosRegistros(char *registro)
{
    if (strcmp(registro, "AX") == 0 || strcmp(registro, "BX") == 0 || strcmp(registro, "CX") == 0 || strcmp(registro, "DX") == 0 || strcmp(registro, "AUX1") == 0)
    {
        return 1;
    }
    else
    {
        return 4;
    }
}

void mandarDatoALeer(int dirFisicaDelDato, int segundaDF, int bytesALeer, int seEscribe2paginas, int tamanioRestantePagina, char *registroDatos)
{
    t_buffer *a_enviar = crear_buffer();
    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_int_al_buffer(a_enviar, pcb_ejecucion.pid);
    cargar_int_al_buffer(a_enviar, dirFisicaDelDato);
    cargar_int_al_buffer(a_enviar, segundaDF);
    cargar_int_al_buffer(a_enviar, bytesALeer);
    cargar_int_al_buffer(a_enviar, seEscribe2paginas);
    cargar_int_al_buffer(a_enviar, tamanioRestantePagina);
    cargar_string_al_buffer(a_enviar, registroDatos);

    t_paquete *un_paquete = crear_super_paquete(MANDAR_DATO_A_LEER, a_enviar);
    enviar_paquete(un_paquete, fd_memoria);
    destruir_paquete(un_paquete);

    sem_wait(&esperarLecturaDeMemoria);
}

void hacerMovIn(int dirLogicaDelDato, int tamanioDatoALeer, char *registroDatos)
{
    // No tengo el tam_pag
    if (primeraSolicitudTamanioDePagina)
    {
        solicitarTamanioPagina();
        primeraSolicitudTamanioDePagina = false;
    }

    int desplazamiento_en_pagina = dirLogicaDelDato % tamanio_pagina;          // offset
    int bytes_restantes_en_pagina = tamanio_pagina - desplazamiento_en_pagina; // cuanto queda en la pagina
    int dirFisicaDelDato = traducir_dl(dirLogicaDelDato);

    if (tamanioDatoALeer == 1)
    { // si es un u8

        mandarDatoALeer(dirFisicaDelDato, 0, 1, 0, bytes_restantes_en_pagina, registroDatos);
        int valorReg = obtenerValorRegistro(registroDatos);
        log_info(cpu_logger, "PID: %d - Acción: LEER - Dirección Física: %d - Valor: %d\n", pcb_ejecucion.pid, dirFisicaDelDato, valorReg);
    }
    else
    { // si es un u32

        if (bytes_restantes_en_pagina < 4)
        { // se tiene que leer en 2 paginas diferentes

            dirLogicaDelDato = dirLogicaDelDato + bytes_restantes_en_pagina;
            int segundaDF = traducir_dl(dirLogicaDelDato);
            mandarDatoALeer(dirFisicaDelDato, segundaDF, 4, 1, bytes_restantes_en_pagina, registroDatos);
            int valorReg = obtenerValorRegistro(registroDatos);
            log_info(cpu_logger, "PID: %d - Acción: LEER - Direcciones Físicas: %d, %d - Valor: %d\n", pcb_ejecucion.pid, dirFisicaDelDato, segundaDF, valorReg);
        }
        else
        { // Entra entero, osea que no se tiene que escribir en 2 paginas diferentes
            mandarDatoALeer(dirFisicaDelDato, 0, 4, 0, bytes_restantes_en_pagina, registroDatos);
            int valorReg = obtenerValorRegistro(registroDatos);
            log_info(cpu_logger, "PID: %d - Acción: LEER - Dirección Física: %d - Valor: %d\n", pcb_ejecucion.pid, dirFisicaDelDato, valorReg);
        }
    }
}

void _mov_in(char *registroDatos, char *registroDireccion)
{
    // obtengo el valor de los registros y se los paso
    void *direccionLogicaDelDato = (void *)get_registry(registroDireccion);

    int tamanioDatoALeer = conocerTamanioDeLosRegistros(registroDatos); // para saber el tamaño de lo que voy a leer
    //printf("el tamanio del dato: %d\n", tamanioDatoALeer);

    int *dirLogicaDelDato = (int *)direccionLogicaDelDato;

    hacerMovIn(*dirLogicaDelDato, tamanioDatoALeer, registroDatos);
}

void mandarDatoAEscribir(int direccion_logica, int direccion_fisica, int segundaDF, void *queEscribir, int bytes_a_escribir, int seEscribe2paginas, int tamanioRestantePagina)
{
    t_buffer *a_enviar = crear_buffer();
    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_int_al_buffer(a_enviar, pcb_ejecucion.pid);
    cargar_int_al_buffer(a_enviar, direccion_logica);
    cargar_int_al_buffer(a_enviar, direccion_fisica);
    cargar_int_al_buffer(a_enviar, segundaDF);
    cargar_int_al_buffer(a_enviar, bytes_a_escribir);

    if (bytes_a_escribir == 1)
    {
        uint8_t *dato = (uint8_t *)queEscribir;
        cargar_uint8_al_buffer(a_enviar, *dato);
    }
    else
    {
        uint32_t *dato = (uint32_t *)queEscribir;
        cargar_uint32_al_buffer(a_enviar, *dato);
    }

    cargar_int_al_buffer(a_enviar, seEscribe2paginas);
    cargar_int_al_buffer(a_enviar, tamanioRestantePagina);

    t_paquete *un_paquete = crear_super_paquete(MANDAR_DATO_A_ESCRIBIR, a_enviar);
    enviar_paquete(un_paquete, fd_memoria);
    destruir_paquete(un_paquete);

    sem_wait(&esperarEscrituraDeMemoria);
}

void mandarSegundaDireccion(int direccion_fisica)
{
    t_buffer *a_enviar = crear_buffer();
    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_int_al_buffer(a_enviar, direccion_fisica);
    t_paquete *un_paquete = crear_super_paquete(SEGUNDA_DIRECCION, a_enviar);
    enviar_paquete(un_paquete, fd_memoria);
    destruir_paquete(un_paquete);
}

void escribir_string_memoria(char *datoEscribir, int direccionLogica)
{

    int desplazamiento_en_pagina = direccionLogica % tamanio_pagina;           // offset
    int bytes_restantes_en_pagina = tamanio_pagina - desplazamiento_en_pagina; // cuanto queda en la pagina

    int cantDireccionesNecesarias = obtener_cant_direcciones(direccionLogica, sizeof(datoEscribir), bytes_restantes_en_pagina);

    t_buffer *buffer = crear_buffer();
    buffer->size = 0;
    buffer->stream = NULL;

    cargar_string_al_buffer(buffer, datoEscribir);
    cargar_int_al_buffer(buffer, bytes_restantes_en_pagina);
    cargar_int_al_buffer(buffer, sizeof(datoEscribir));

    int flag = 0;

    for (int i = 0; i < cantDireccionesNecesarias; i++)
    {
        int df = traducir_dl(direccionLogica);
        cargar_int_al_buffer(buffer, df);

        if (flag == 0)
        {
            direccionLogica = direccionLogica + bytes_restantes_en_pagina;
            flag = 1;
        }
        else
        {
            direccionLogica = direccionLogica + tamanio_pagina;
        }
        //printf("carge un int al buffer\n");
    }

    t_paquete *paquete = crear_super_paquete(ESCRIBIR_MEMORIA, buffer);
    enviar_paquete(paquete, fd_memoria);
    destruir_paquete(paquete);
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

    if (tamanio_dato == 1)
    { // si es un u8
        mandarDatoAEscribir(direccionLogica, direccion_fisica, 0, dato, 1, 0, bytes_restantes_en_pagina);
        uint8_t datoParaMostrar = (uint8_t)dato;
        
        //log_info(cpu_logger, "PID: %d  - Acción: ESCRIBIR - Dirección Física: %d - Valor: %u", pcb_ejecucion.pid, direccion_fisica, datoParaMostrar);
    }
    else
    { // si es un u32
        uint32_t datoParaMostrar = (uint32_t)dato;
        //printf("los bytes_restantes_en_pagina son: %d\n", bytes_restantes_en_pagina);
        if (bytes_restantes_en_pagina < 4)
        { // se tiene que escribir en 2 paginas diferentes
            //printf("Entro al if turbio\n");
            direccionLogica = direccionLogica + bytes_restantes_en_pagina;
            int segundaDF = traducir_dl(direccionLogica);
            mandarDatoAEscribir(direccionLogica, direccion_fisica, segundaDF, dato, 4, 1, bytes_restantes_en_pagina);
            //log_info(cpu_logger, "PID: %d  - Acción: ESCRIBIR - Direcciones Físicas: %d, %d - Valor: %u", pcb_ejecucion.pid, direccion_fisica, segundaDF, datoParaMostrar);
        }
        else
        { // no se tiene que escribir en 2 paginas diferentes
            //printf("Entro al else turbio\n");
            mandarDatoAEscribir(direccionLogica, direccion_fisica, 0, dato, 4, 0, bytes_restantes_en_pagina);
            //log_info(cpu_logger, "PID: %d  - Acción: ESCRIBIR - Direcciones Físicas: %d - Valor: %u", pcb_ejecucion.pid, direccion_fisica, datoParaMostrar);
        }
    }
}

//
void _mov_out(char *registroDireccion, char *registroDatos)
{

    if (primeraSolicitudTamanioDePagina)
    {
        solicitarTamanioPagina();
        primeraSolicitudTamanioDePagina = false;
    }

    // Obtengo el valor de los registros y se los paso a hacerMovOut
    void *direccionLogica = (void *)get_registry(registroDireccion);
    void *dato = (void *)get_registry(registroDatos);

    int tamanioDato = conocerTamanioDeLosRegistros(registroDatos);

    // Determinar el tamaño del registro de direccionLogica
    int tamanioDireccionLogica = conocerTamanioDeLosRegistros(registroDireccion);

    // Convertir el puntero de direccionLogica al tipo adecuado
    if (tamanioDireccionLogica == sizeof(uint8_t))
    {
        uint8_t *dirLogic = (uint8_t *)direccionLogica;
        hacerMovOut(*dirLogic, dato, tamanioDato);
    }
    else
    {
        uint32_t *dirLogic = (uint32_t *)direccionLogica;
        hacerMovOut(*dirLogic, dato, tamanioDato);
    }
}

void _sum(char *registroDestino, char *registroOrigen)
{
    /*uint32_t *destino = get_registry(registroDestino);
    uint32_t *origen = get_registry(registroOrigen);

    *destino = *destino + *origen;*/
    uint32_t *destino = get_registry(registroDestino);
    uint32_t *origen = get_registry(registroOrigen);

    uint32_t destinoValue = 0;
    uint32_t origenValue = 0;

    // Detectar si el origen es un registro de 8 bits
    if (strcmp(registroOrigen, "AX") == 0 || strcmp(registroOrigen, "BX") == 0 ||
        strcmp(registroOrigen, "CX") == 0 || strcmp(registroOrigen, "DX") == 0)
    {
        origenValue = *(uint8_t *)origen;
    }
    else
    {
        origenValue = *origen;
    }

    // Detectar si el destino es un registro de 8 bits
    if (strcmp(registroDestino, "AX") == 0 || strcmp(registroDestino, "BX") == 0 ||
        strcmp(registroDestino, "CX") == 0 || strcmp(registroDestino, "DX") == 0)
    {
        destinoValue = *(uint8_t *)destino;
        destinoValue += origenValue;
        *(uint8_t *)destino = (uint8_t)destinoValue;
    }
    else
    {
        destinoValue = *destino;
        destinoValue += origenValue;
        *destino = destinoValue;
    }
}

void _sub(char *registroDestino, char *registroOrigen)
{
    uint32_t *destino = get_registry(registroDestino);
    uint32_t *origen = get_registry(registroOrigen);

    uint32_t destinoValue = 0;
    uint32_t origenValue = 0;

    // Detectar si el origen es un registro de 8 bits
    if (strcmp(registroOrigen, "AX") == 0 || strcmp(registroOrigen, "BX") == 0 ||
        strcmp(registroOrigen, "CX") == 0 || strcmp(registroOrigen, "DX") == 0)
    {
        origenValue = *(uint8_t *)origen;
    }
    else
    {
        origenValue = *origen;
    }

    // Detectar si el destino es un registro de 8 bits
    if (strcmp(registroDestino, "AX") == 0 || strcmp(registroDestino, "BX") == 0 ||
        strcmp(registroDestino, "CX") == 0 || strcmp(registroDestino, "DX") == 0)
    {
        destinoValue = *(uint8_t *)destino;
        destinoValue -= origenValue;
        *(uint8_t *)destino = (uint8_t)destinoValue;
    }
    else
    {
        destinoValue = *destino;
        destinoValue -= origenValue;
        *destino = destinoValue;
    }
}

void _jnz(char *registro, char *instruccion)
{
    uint32_t *registroAComparar = get_registry(registro);
    int numeroDeInstr = atoi(instruccion);

    if (*registroAComparar != 0)
    {
        pcb_ejecucion.program_counter = numeroDeInstr;
    }
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

void concat_uint8_to_string(char *str, uint8_t ch)
{
    size_t len = strlen(str); // Encuentra la longitud actual del string

    //printf("el caracter es: %c\n", (char)ch);

    str[len] = (char)ch; // Añade el carácter al final del string
    str[len + 1] = '\0'; // Añade el terminador nulo
}

int obtener_cant_direcciones(int direccionLogica, int tamanioAEscribir, int bytes_restantes_en_pagina)
{
    int cont = 0;

    while (tamanioAEscribir > 0)
    {

        if (cont == 0)
        {
            tamanioAEscribir = tamanioAEscribir - bytes_restantes_en_pagina;
        }
        else
        {
            tamanioAEscribir = tamanioAEscribir - tamanio_pagina;
        }
        cont++;
    }

    return cont;
}

void leerCaracterMemoria(int direccionLogica)
{
    int df = traducir_dl(direccionLogica);

    t_buffer *buffer = crear_buffer();
    buffer->size = 0;
    buffer->stream = NULL;

    cargar_int_al_buffer(buffer, df);

    //printf("la direccion fisica: %d\n", df);

    t_paquete *paquete = crear_super_paquete(LEER_CARACTER_MEMORIA, buffer);
    enviar_paquete(paquete, fd_memoria);
    destruir_paquete(paquete);

    sem_wait(&esperar_lectura_caracter);
}

void _copy_string(char *tamanio)
{
    // Leer del SI
    int desplazamiento_en_pagina_read = (int)pcb_ejecucion.registros_cpu.SI % tamanio_pagina; // offset
    int bytes_restantes_en_pagina_read = tamanio_pagina - desplazamiento_en_pagina_read;           // cuanto queda en la pagina

    int tamanioAEscribir = atoi(tamanio);

    int tamanioALeer = tamanioAEscribir;
    logicaDeLeer(bytes_restantes_en_pagina_read, tamanioALeer);
    sem_wait(&esperarLecturaDeString);

    log_info(cpu_logger, "Valor leido: %s", stringLeido);

    int desplazamiento_en_pagina = (int)pcb_ejecucion.registros_cpu.DI % tamanio_pagina; // offset
    int bytes_restantes_en_pagina = tamanio_pagina - desplazamiento_en_pagina;           // cuanto queda en la pagina

    // Escribir en el DI
    int cantDireccionesNecesarias = obtener_cant_direcciones((int)pcb_ejecucion.registros_cpu.DI, tamanioAEscribir, bytes_restantes_en_pagina);

    t_buffer *buffer = crear_buffer();
    buffer->size = 0;
    buffer->stream = NULL;
    
    cargar_int_al_buffer(buffer, pcb_ejecucion.pid);
    cargar_string_al_buffer(buffer, stringLeido);
    cargar_int_al_buffer(buffer, bytes_restantes_en_pagina);
    cargar_int_al_buffer(buffer, tamanioAEscribir);

    int flag = 0;

    int copiaDI = (int)pcb_ejecucion.registros_cpu.DI;

    log_info(cpu_logger, "PID: %d  - Acción: ESCRIBIR ", pcb_ejecucion.pid);

    for (int i = 0; i < cantDireccionesNecesarias; i++)
    {
        int df = traducir_dl(copiaDI);
        cargar_int_al_buffer(buffer, df);
        log_info(cpu_logger, "DIRECCIÓN FÍSICA: %d ", df);

        if (flag == 0)
        {
            copiaDI = copiaDI + bytes_restantes_en_pagina;
            flag = 1;
        }
        else
        {
            copiaDI = copiaDI + tamanio_pagina;
        }
        //printf("carge un int al buffer\n");
    }
    
    log_info(cpu_logger, "Valor escrito: %s", stringLeido);

    t_paquete *paquete = crear_super_paquete(ESCRIBIR_MEMORIA, buffer);
    enviar_paquete(paquete, fd_memoria);
    destruir_paquete(paquete);
}

void logicaDeLeer(int bytes_restantes_en_pagina, int tamanioALeer){ 
    int cantDireccionesNecesarias = obtener_cant_direcciones((int)pcb_ejecucion.registros_cpu.SI, tamanioALeer, bytes_restantes_en_pagina);

    t_buffer *buffer = crear_buffer(); 
    buffer->size = 0;
    buffer->stream = NULL;
    cargar_int_al_buffer(buffer, pcb_ejecucion.pid);
    cargar_int_al_buffer(buffer, bytes_restantes_en_pagina);
    cargar_int_al_buffer(buffer, tamanioALeer);

    int flag = 0;

    int copiaSI = (int)pcb_ejecucion.registros_cpu.SI;

    for (int i = 0; i < cantDireccionesNecesarias; i++)
    {
        int df = traducir_dl(copiaSI);
        cargar_int_al_buffer(buffer, df);

        if (flag == 0)
        {
            copiaSI = copiaSI + bytes_restantes_en_pagina;
            flag = 1;
        }
        else
        {
            copiaSI = copiaSI + tamanio_pagina;
        }
        //printf("carge un int al buffer\n");
        log_info(cpu_logger, "PID: %d  - Acción: LEER - Dirección Física: %d", pcb_ejecucion.pid, df);
    }

    t_paquete *paquete = crear_super_paquete(LEER_EN_MEMORIA_UN_STRING, buffer);
    enviar_paquete(paquete, fd_memoria);
    destruir_paquete(paquete);

}

void ioGenSleep(char *nombreInterfaz, char *unidadesTrabajo)
{
    int uniTraba = atoi(unidadesTrabajo);

    t_buffer *buffer_IOKernel = crear_buffer();
    buffer_IOKernel->size = 0;
    buffer_IOKernel->stream = NULL;

    cargar_string_al_buffer(buffer_IOKernel, nombreInterfaz);
    cargar_int_al_buffer(buffer_IOKernel, uniTraba);

    t_paquete *paquete_IOKernel = crear_super_paquete(ENVIAR_IO_GEN_SLEEP, buffer_IOKernel);
    enviar_paquete(paquete_IOKernel, fd_kernel_dispatch);
    destruir_paquete(paquete_IOKernel);
}

// void ioSTDINRead(param1, param2, param3);

int obtenerValorRegistro(char *registro)
{

    void *dato = (void *)get_registry(registro);

    int registroDevolver;

    if (is_8bit_register(registro))
    {
        registroDevolver = *((uint8_t *)dato);
    }
    else
    {
        registroDevolver = *((uint32_t *)dato);
    }

    return registroDevolver;
}

void io_stdout_write(char *nombreInterfaz, char *registro_direccionLogica, char *registro_tamanio)
{
    int dirLogicaDelDato = obtenerValorRegistro(registro_direccionLogica);
    int tamanioDato = obtenerValorRegistro(registro_tamanio);

    //printf("la dl es: %d\n", dirLogicaDelDato);
    //printf("el tamanio es: %d\n", tamanioDato);

    int desplazamiento_en_pagina = dirLogicaDelDato % tamanio_pagina;          // offset
    int bytes_restantes_en_pagina = tamanio_pagina - desplazamiento_en_pagina; // cuanto queda en la pagina

    int cantDireccionesNecesarias = obtener_cant_direcciones(dirLogicaDelDato, tamanioDato, bytes_restantes_en_pagina);

    t_buffer *buffer = crear_buffer();
    buffer->size = 0;
    buffer->stream = NULL;

    cargar_string_al_buffer(buffer, nombreInterfaz);
    cargar_int_al_buffer(buffer, bytes_restantes_en_pagina); 
    cargar_int_al_buffer(buffer, tamanioDato);
    cargar_int_al_buffer(buffer, cantDireccionesNecesarias);

    int flag = 0;

    for (int i = 0; i < cantDireccionesNecesarias; i++)
    {
        int df = traducir_dl(dirLogicaDelDato);

        //printf("direccion fisica: %d\n", df);

        cargar_int_al_buffer(buffer, df);

        if (flag == 0)
        {
            dirLogicaDelDato = dirLogicaDelDato + bytes_restantes_en_pagina;
            flag = 1;
        }
        else
        {
            dirLogicaDelDato = dirLogicaDelDato + tamanio_pagina;
        }

        //printf("carge un int al buffer\n");
    }

    t_paquete *paquete = crear_super_paquete(ENVIAR_IO_STDOUT_WRITE, buffer);
    enviar_paquete(paquete, fd_kernel_dispatch);
    destruir_paquete(paquete);
}

void ioSTDINRead(char *nombreInterfaz, char *registro_direccion, char *registro_tamanio)
{
    if (primeraSolicitudTamanioDePagina)
    {
        solicitarTamanioPagina();
        primeraSolicitudTamanioDePagina = false;
    }

    int dirLogicaDelDato = obtenerValorRegistro(registro_direccion);
    int tamanioDato = obtenerValorRegistro(registro_tamanio);

    //printf("la dl es: %d\n", dirLogicaDelDato);
    //printf("el tamanio es: %d\n", tamanioDato);

    int desplazamiento_en_pagina = dirLogicaDelDato % tamanio_pagina;          // offset
    int bytes_restantes_en_pagina = tamanio_pagina - desplazamiento_en_pagina; // cuanto queda en la pagina

    int cantDireccionesNecesarias = obtener_cant_direcciones(dirLogicaDelDato, tamanioDato, bytes_restantes_en_pagina);

    t_buffer *buffer = crear_buffer();
    buffer->size = 0;
    buffer->stream = NULL;

    cargar_string_al_buffer(buffer, nombreInterfaz);
    cargar_int_al_buffer(buffer, bytes_restantes_en_pagina);
    cargar_int_al_buffer(buffer, tamanioDato);
    cargar_int_al_buffer(buffer, cantDireccionesNecesarias);

    int flag = 0;

    for (int i = 0; i < cantDireccionesNecesarias; i++)
    {
        int df = traducir_dl(dirLogicaDelDato);

        //printf("direccion fisica: %d\n", df);

        cargar_int_al_buffer(buffer, df);

        if (flag == 0)
        {
            dirLogicaDelDato = dirLogicaDelDato + bytes_restantes_en_pagina;
            flag = 1;
        }
        else
        {
            dirLogicaDelDato = dirLogicaDelDato + tamanio_pagina;
        }

        //printf("carge un int al buffer\n");
    }

    t_paquete *paquete = crear_super_paquete(ENVIAR_IO_STDIN_READ, buffer);
    enviar_paquete(paquete, fd_kernel_dispatch);
    destruir_paquete(paquete);
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

void establecerVariablesNecesarias(char *typeInstruccion, char *nameInterfaz)
{

    tipo_instruccion = typeInstruccion;
    nombre_interfaz = nameInterfaz;

    ejecute_instruccion_tipo_io = true;
    sem_post(&wait_instruccion);
}

void io_fs_create(char* interfaz,char* nombreArchivo){
    
    t_buffer *a_enviar = crear_buffer();
    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_string_al_buffer(a_enviar,interfaz);
    cargar_string_al_buffer(a_enviar,nombreArchivo);

    t_paquete *un_paquete = crear_super_paquete(ENVIAR_IO_FS_CREATE, a_enviar);
    enviar_paquete(un_paquete, fd_kernel_dispatch);
    destruir_paquete(un_paquete);

}

void io_fd_delete(char* interfaz,char* nombreArchivo){

    t_buffer *a_enviar = crear_buffer();
    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_string_al_buffer(a_enviar,interfaz);
    cargar_string_al_buffer(a_enviar,nombreArchivo);

    t_paquete *un_paquete = crear_super_paquete(ENVIAR_IO_FS_DELETE, a_enviar);
    enviar_paquete(un_paquete, fd_kernel_dispatch);
    destruir_paquete(un_paquete);
}

void io_fd_truncate(char* interfaz,char* nombreArchivo, char* registroTamanio){

    int valor = obtenerValorRegistro(registroTamanio);

    t_buffer *a_enviar = crear_buffer();
    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_string_al_buffer(a_enviar,interfaz);
    cargar_string_al_buffer(a_enviar,nombreArchivo);
    cargar_int_al_buffer(a_enviar,valor);

    t_paquete *un_paquete = crear_super_paquete(ENVIAR_IO_FS_TRUNCATE, a_enviar);
    enviar_paquete(un_paquete, fd_kernel_dispatch);
    destruir_paquete(un_paquete);
}

void io_fs_write(char* interfaz, char* nombreArchivo, char* registroDireccion, char* registroTamanio, char* registroPunteroArchivo){

    int dirLogicaDelDato = obtenerValorRegistro(registroDireccion);
    int tamanioDato = obtenerValorRegistro(registroTamanio);
    int registroPuntero = obtenerValorRegistro(registroPunteroArchivo);

    //cargar_string_al_buffer(buffer_IOKernel, interfaz);
    //cargar_string_al_buffer(buffer_IOKernel, nombreArchivo);
    //cargar_int_al_buffer(buffer_IOKernel,registroPuntero);
    //cargar_int_al_buffer(buffer_IOKernel, tamanioDato);

    int desplazamiento_en_pagina = dirLogicaDelDato % tamanio_pagina;          // offset
    int bytes_restantes_en_pagina = tamanio_pagina - desplazamiento_en_pagina; // cuanto queda en la pagina

    int cantDireccionesNecesarias = obtener_cant_direcciones(dirLogicaDelDato, tamanioDato, bytes_restantes_en_pagina);

    t_buffer *buffer = crear_buffer();
    buffer->size = 0;
    buffer->stream = NULL;

    cargar_string_al_buffer(buffer, interfaz);
    cargar_string_al_buffer(buffer,nombreArchivo);
    cargar_int_al_buffer(buffer,registroPuntero);
    cargar_int_al_buffer(buffer, bytes_restantes_en_pagina);
    cargar_int_al_buffer(buffer, tamanioDato);
    cargar_int_al_buffer(buffer, cantDireccionesNecesarias);

    int flag = 0;

    for (int i = 0; i < cantDireccionesNecesarias; i++)
    {
        int df = traducir_dl(dirLogicaDelDato);

        //printf("direccion fisica: %d\n", df);

        cargar_int_al_buffer(buffer, df);

        if (flag == 0)
        {
            dirLogicaDelDato = dirLogicaDelDato + bytes_restantes_en_pagina;
            flag = 1;
        }
        else
        {
            dirLogicaDelDato = dirLogicaDelDato + tamanio_pagina;
        }

        //printf("carge un int al buffer\n");
    }

    t_paquete *paquete = crear_super_paquete(ENVIAR_IO_FS_WRITE, buffer);
    enviar_paquete(paquete, fd_kernel_dispatch);
    destruir_paquete(paquete);
}

void io_fs_read(char* interfaz,char* nombreArchivo,char* registroDireccion,char* registroTamanio,char* registroPunteroArchivo){

    if (primeraSolicitudTamanioDePagina)
    {
        solicitarTamanioPagina();
        primeraSolicitudTamanioDePagina = false;
    }

    int dirLogicaDelDato = obtenerValorRegistro(registroDireccion);
    int tamanioDato = obtenerValorRegistro(registroTamanio);
    int registroPuntero = obtenerValorRegistro(registroPunteroArchivo);

    //printf("la dl es: %d\n", dirLogicaDelDato);
    //printf("el tamanio es: %d\n", tamanioDato);

    int desplazamiento_en_pagina = dirLogicaDelDato % tamanio_pagina;          // offset
    int bytes_restantes_en_pagina = tamanio_pagina - desplazamiento_en_pagina; // cuanto queda en la pagina

    int cantDireccionesNecesarias = obtener_cant_direcciones(dirLogicaDelDato, tamanioDato, bytes_restantes_en_pagina);

    t_buffer *buffer = crear_buffer();
    buffer->size = 0;
    buffer->stream = NULL;

    cargar_string_al_buffer(buffer, interfaz);
    cargar_string_al_buffer(buffer,nombreArchivo);
    cargar_int_al_buffer(buffer,registroPuntero);
    cargar_int_al_buffer(buffer, bytes_restantes_en_pagina);
    cargar_int_al_buffer(buffer, tamanioDato);
    cargar_int_al_buffer(buffer, cantDireccionesNecesarias);

    int flag = 0;

    for (int i = 0; i < cantDireccionesNecesarias; i++)
    {
        int df = traducir_dl(dirLogicaDelDato);

        //printf("direccion fisica: %d\n", df);

        cargar_int_al_buffer(buffer, df);

        if (flag == 0)
        {
            dirLogicaDelDato = dirLogicaDelDato + bytes_restantes_en_pagina;
            flag = 1;
        }
        else
        {
            dirLogicaDelDato = dirLogicaDelDato + tamanio_pagina;
        }

        //printf("carge un int al buffer\n");
    }

    t_paquete *paquete = crear_super_paquete(ENVIAR_IO_FS_READ, buffer);
    enviar_paquete(paquete, fd_kernel_dispatch);
    destruir_paquete(paquete);
}


void ejecutar_instruccion(char *instruccion, PCB *pcb)
{
    char instr[20], param1[20], param2[20], param3[20], param4[20], param5[20];

    sscanf(instruccion, "%s %s %s %s %s %s", instr, param1, param2, param3, param4, param5);

    nombre_instruccion instruction = str_to_instruction(instr);

    // solo van los semaforos en las funciones que solo se ejecutan en CPU las demas memoria/ IO
    // le avisan a CPU de que termino y recien ahi se liberan

    switch (instruction)
    {
    case SET:
        _set(param1, param2);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        sem_post(&wait_instruccion);
        break;
    case MOV_IN:
        _mov_in(param1, param2);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        break;
    case MOV_OUT:
        _mov_out(param1, param2);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        break;
    case SUM:
        _sum(param1, param2);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        sem_post(&wait_instruccion);
        break;
    case SUB:
        _sub(param1, param2);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        sem_post(&wait_instruccion);
        break;
    case JNZ:
        _jnz(param1, param2);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        sem_post(&wait_instruccion);
        break;
    case RESIZE:
        _resize(param1);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        break;
    case COPY_STRING:
        _copy_string(param1);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        break;
    case WAIT:
        _wait(param1);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        sem_post(&wait_instruccion);
        break;
    case SIGNAL:
        _signal(param1);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        sem_post(&wait_instruccion);
        break;
    case IO_GEN_SLEEP:
        ioGenSleep(param1, param2);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        establecerVariablesNecesarias("IO_GEN_SLEEP", param1);
        break;
    case IO_STDIN_READ:
        ioSTDINRead(param1, param2, param3);
        establecerVariablesNecesarias("IO_STDIN_READ", param1);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        break;
    case IO_STDOUT_WRITE:
        io_stdout_write(param1, param2, param3);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        establecerVariablesNecesarias("IO_STDOUT_WRITE", param1);
        break;
    case IO_FS_CREATE:
        io_fs_create(param1,param2);
        establecerVariablesNecesarias("IO_FS_CREATE", param1);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        break;
    case IO_FS_DELETE:
        io_fd_delete(param1,param2);
        establecerVariablesNecesarias("IO_FS_DELETE", param1);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        break;
    case IO_FS_TRUNCATE:
        io_fd_truncate(param1,param2,param3);
        establecerVariablesNecesarias("IO_FS_TRUNCATE", param1);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        break;
    case IO_FS_WRITE:
        io_fs_write(param1,param2,param3,param4,param5);
        establecerVariablesNecesarias("IO_FS_WRITE", param1);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        break;
    case IO_FS_READ:
        io_fs_read(param1,param2,param3,param4,param5);
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        establecerVariablesNecesarias("IO_FS_READ", param1);
        break;
    case EXIT:
        nombre_interfaz = param1;
        terminarPorExit = true;
        log_info(cpu_logger, "PID: %d - Ejecutando: %s", pcb->pid, instruccion);
        sem_post(&wait_instruccion);
        break;
    case INVALID_INSTRUCTION:
        printf("aprende a escribir\n");
        break;
    }
}

void solicitar_instruccion(int pid, int program_counter)
{

    t_buffer *a_enviar = crear_buffer();

    a_enviar->size = 0;
    a_enviar->stream = NULL;

    // printf("el pid en solicitar_instruccion es: %d\n", pid);
    //printf("el program counter en solicitar instrucciones: %d\n", program_counter);
    log_info(cpu_logger, "PID: %d - FETCH - Program Counter: %d", pcb_ejecucion.pid, pcb_ejecucion.program_counter);

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

    //printf("envie el mensaje de CPU LISTA\n");

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

    //printf("Antes del semanforo\n");

    sem_wait(&esperarMarco);

    //printf("Pase el semanforo\n");
}

//////////////////////TLB///////////////////////////

lineaTLB *inicializarLineaTLB(int pid, int pagina, int marco)
{
    lineaTLB *lineaTL = (lineaTLB *)malloc(sizeof(lineaTLB));
    if (lineaTL == NULL)
    {
        fprintf(stderr, "Error al asignar memoria para lineaTLB\n");
        exit(EXIT_FAILURE);
    }
    lineaTL->pid = pid;
    lineaTL->pagina = pagina;
    lineaTL->marco = marco;

    return lineaTL;
}

int list_index_of(t_list *self, void *data, bool (*comp)(void *, void *))
{
    int index = 0;
    t_link_element *current = self->head;
    while (current != NULL)
    {
        if (comp(current->data, data))
        {
            return index;
        }
        current = current->next;
        index++;
    }
    return -1; // Elemento no encontrado
}

bool comparar_lineaTLB(void *a, void *b)
{
    lineaTLB *lineaA = (lineaTLB *)a;
    lineaTLB *lineaB = (lineaTLB *)b;
    return (lineaA->pid == lineaB->pid) && (lineaA->pagina == lineaB->pagina);
}

void actualizarPrioridadesTLB(lineaTLB *lineaTL)
{
    int index = list_index_of(cola_tlb->elements, lineaTL, comparar_lineaTLB); // Obtengo el índice del que quiero actualizar

    if (index != -1)
    {                                                                     // Verifica que el elemento esté en la lista
        lineaTLB *lineaRemovida = list_remove(cola_tlb->elements, index); // Lo borro
        //printf("agrege al final por LRU la pag: %d\n", lineaRemovida->pagina);
        queue_push(cola_tlb, lineaRemovida); // Lo vuelvo a agregar al final
    }
}

void agregarPaginaTLB(int pid, int pagina, int marco)
{
    lineaTLB *lineaTL = inicializarLineaTLB(pid, pagina, marco);

    if (queue_size(cola_tlb) < CANTIDAD_ENTRADAS_TLB)
    { // Si entra acá es porque no hay que hacer ningún reemplazo
        queue_push(cola_tlb, lineaTL);
        //printf("agrege la pagina %d\n", lineaTL->pagina);
    }
    else
    {                                                 // Significa que ya se llenó la TLB entonces hay que reemplazar una
        lineaTLB *lineaABorrar = queue_pop(cola_tlb); // Eliminar el elemento más antiguo (FIFO)
        //printf("saque la pagina %d\n", lineaABorrar->pagina);
        free(lineaABorrar);
        queue_push(cola_tlb, lineaTL);
        //printf("agrege la pagina %d\n", lineaTL->pagina);
    }
}

bool condicion_id_pagina(void *elemento)
{
    lineaTLB *dato = (lineaTLB *)elemento;
    return (dato->pid == id_global && dato->pagina == pagina_global);
}

int buscarMarcoTLB(int pid, int pagina)
{
    id_global = pid;
    pagina_global = pagina;

    lineaTLB *lineaTL = list_find(cola_tlb->elements, condicion_id_pagina);

    if (lineaTL == NULL)
    { // Se produce un MISS al no encontrarlo
        log_info(cpu_logger, "PID: %d - TLB MISS - Pagina: %d", pid, pagina);
        return -1;
    }

    log_info(cpu_logger, "PID: %d - TLB HIT - Pagina: %d", pid, pagina);

    if (strcmp(ALGORITMO_TLB, "LRU") == 0)
    { // En caso de que sea LRU hay que actualizar la prioridad
        actualizarPrioridadesTLB(lineaTL);
    }

    return lineaTL->marco; // Encontró el marco y lo devuelve
}

int traducir_dl(int direccionLogica)
{
    if (primeraSolicitudTamanioDePagina) // Sirve para obtener el tamaño de página de memoria
    {
        solicitarTamanioPagina();
        primeraSolicitudTamanioDePagina = false;
    }

    int num_pag = direccionLogica / tamanio_pagina;
    int desplazamiento = direccionLogica % tamanio_pagina;

    if (CANTIDAD_ENTRADAS_TLB > 0)
    {
        marco = buscarMarcoTLB(pcb_ejecucion.pid, num_pag);

        if (marco != -1)
        { // Hubo un HIT
            log_info(cpu_logger, "PID: %d - OBTENER MARCO - Página: %d - Marco: %d", pcb_ejecucion.pid, num_pag, marco);
            return (marco * tamanio_pagina + desplazamiento);
        }
    }

    enviar_pedido_marco(num_pag, pcb_ejecucion.pid); // En caso de un MISS busca en memoria

    if (CANTIDAD_ENTRADAS_TLB > 0)
    {

        agregarPaginaTLB(pcb_ejecucion.pid, num_pag, marco); // Después del MISS se actualiza la TLB
    }

    log_info(cpu_logger, "PID: %d - OBTENER MARCO - Página: %d - Marco: %d", pcb_ejecucion.pid, num_pag, marco);

    return (marco * tamanio_pagina + desplazamiento);
}

////////////////////////////////////////////////////

void obtener_cantidad_instrucciones(int pid)
{
    t_buffer *a_enviar = crear_buffer();

    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_int_al_buffer(a_enviar, pid);

    t_paquete *un_paquete = crear_super_paquete(CANT_INTRUCCIONES, a_enviar);
    enviar_paquete(un_paquete, fd_memoria);
    destruir_paquete(un_paquete);

    sem_wait(&wait_instruccion);
}

void _wait(char *recurso)
{
    terminaPorSenial = true;
}

void _signal(char *recurso)
{
    terminaPorSenial = true;
}

void devolverPCBKernelSenial()
{
    char instr[20], recurso[20];

    sscanf(instruccion_actual, "%s %s", instr, recurso);

    t_buffer *buffer = cargar_pcb_buffer(pcb_ejecucion);

    // printf("la instruccion actual: %s\n", instruccion_actual);
    // printf("la inst: %s\n", instr);
    // printf("el recurso: %s\n", recurso);

    cargar_string_al_buffer(buffer, recurso);
    cargar_int_al_buffer(buffer,numeroID_hilo);

    if (strcmp(instr, "WAIT") == 0)
    {
        t_paquete *paquete = crear_super_paquete(DESALOJO_POR_WAIT, buffer);
        enviar_paquete(paquete, fd_kernel_dispatch);
        destruir_paquete(paquete);
    }
    else
    {
        t_paquete *paquete = crear_super_paquete(DESALOJO_POR_SIGNAL, buffer);
        enviar_paquete(paquete, fd_kernel_dispatch);
        destruir_paquete(paquete);
    }
}

void devolverPCBKernel_exit_o_bloqueado()
{
    pcb_ejecucion.program_counter++;
    t_buffer *buffer = cargar_pcb_buffer(pcb_ejecucion); // te da un buffer ya con el pcb cargado

    cargar_string_al_buffer(buffer, nombre_interfaz);
    cargar_string_al_buffer(buffer, tipo_instruccion);
    cargar_int_al_buffer(buffer,numeroID_hilo);

    t_paquete *paquete = crear_super_paquete(INSTRUCCION_TIPO_IO, buffer);
    enviar_paquete(paquete, fd_kernel_dispatch);
    destruir_paquete(paquete);

    //printf("desaloje al proceso al ejectuar una instruccion de tipo IO\n");
}

bool instruccion_es_tipo_io(char *instruccion_actual)
{
    char instr[20], nombre_interfazXD[20];

    sscanf(instruccion_actual, "%s %s ", instr, nombre_interfazXD);

    nombre_interfaz = malloc(strlen(nombre_interfazXD) + 1);
    tipo_instruccion = malloc(strlen(instr) + 1);

    // Copiar las cadenas a las variables globales
    strcpy(nombre_interfaz, nombre_interfazXD);
    strcpy(tipo_instruccion, instr);

    //printf("el nombre de la interfaz: %s\n", nombre_interfazXD);
    //printf("la instruccion es: %s\n", instr);

    if (strcmp(instr, "IO_GEN_SLEEP") == 0 || strcmp(instr, "IO_STDIN_READ") == 0 || strcmp(instr, "IO_STDOUT_WRITE") == 0 || strcmp(instr, "IO_FS_CREATE") == 0 || strcmp(instr, "IO_FS_DELETE") == 0 || strcmp(instr, "IO_FS_TRUNCATE") == 0 || strcmp(instr, "IO_FS_WRITE") == 0 || strcmp(instr, "IO_FS_READ") == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void devolverPCBKernelCambioContexto()
{
    t_buffer *buffer = cargar_pcb_buffer(pcb_ejecucion);
    cargar_int_al_buffer(buffer, 1); // si hay cambio de contexto envio un 1 osea fue desalojado => le faltan instrucciones por ejecutar
    cargar_int_al_buffer(buffer,numeroID_hilo);
    t_paquete *un_paquete = crear_super_paquete(RECIBIR_PCB, buffer);
    enviar_paquete(un_paquete, fd_kernel_dispatch);
    destruir_paquete(un_paquete);
}

void devolverPCBKernelEXit()
{
    t_buffer *buffer = cargar_pcb_buffer(pcb_ejecucion);
    cargar_int_al_buffer(buffer, 2); // si se queda sin instrucciones va un 2
    cargar_int_al_buffer(buffer,numeroID_hilo);
    t_paquete *un_paquete = crear_super_paquete(RECIBIR_PCB, buffer);
    enviar_paquete(un_paquete, fd_kernel_dispatch);
    destruir_paquete(un_paquete);
}

void devolverPCBKernelOutOfMemory(){

    t_buffer *buffer = cargar_pcb_buffer(pcb_ejecucion);
    cargar_int_al_buffer(buffer,numeroID_hilo);
    t_paquete *un_paquete = crear_super_paquete(OUT_OF_MEMORY, buffer);
    enviar_paquete(un_paquete, fd_kernel_dispatch);
    destruir_paquete(un_paquete);
}

void procesar_instruccion()
{
    //printf("la cantidad de instrucciones son: %d\n", cantInstucciones);

    terminarPorExit = false;
    terminaPorSenial = false;
    cambioContexto = false;
    ejecute_instruccion_tipo_io = false;
    terminePorOutOfMemory = false;
    yaDevolvioPcb = false;

    while (!terminarPorExit && !cambioContexto && !terminaPorSenial && !ejecute_instruccion_tipo_io && !terminePorOutOfMemory)
    {

        solicitar_instruccion(pcb_ejecucion.pid, pcb_ejecucion.program_counter);

        sem_wait(&wait_instruccion);

        //printf("se ejecuto la instruccion\n");

        if (instruccion_es_tipo_io(instruccion_actual)) // si se ejecuta algo de tipo io y lo desaloja
        {
            devolverPCBKernel_exit_o_bloqueado();
            yaDevolvioPcb = true;
        }

        ejecutar_instruccion(instruccion_actual, &pcb_ejecucion);

        sem_wait(&wait_instruccion);

        // printf("el PID: %d\n", pcb_ejecucion.pid);
        printf("Estado de los registros:\n");
        printf("AX: %d, BX: %d, CX: %d, DX: %d\n", pcb_ejecucion.registros_cpu.AX, pcb_ejecucion.registros_cpu.BX, pcb_ejecucion.registros_cpu.CX, pcb_ejecucion.registros_cpu.DX);
        printf("EAX: %u, EBX: %u, ECX: %u, EDX: %u\n", pcb_ejecucion.registros_cpu.EAX, pcb_ejecucion.registros_cpu.EBX, pcb_ejecucion.registros_cpu.ECX, pcb_ejecucion.registros_cpu.EDX);
        printf("PC: %d\n\n", pcb_ejecucion.program_counter);
        printf("------------------------------------------------\n\n");

        pcb_ejecucion.program_counter++;
    }

    // sale del while o porque se queda sin instrucciones o porque es desalojado

    if (terminarPorExit && !yaDevolvioPcb) // si termina por wait o signal
    {        
        devolverPCBKernelEXit();
        yaDevolvioPcb = true;     
    }
    else
    {
        if (terminaPorSenial && !yaDevolvioPcb)
        {
             devolverPCBKernelSenial();
             yaDevolvioPcb = true;
        }
        else
        {
            if (cambioContexto && !yaDevolvioPcb)
            {
                devolverPCBKernelCambioContexto();
                yaDevolvioPcb = true;
            }
            else{
                if(terminePorOutOfMemory && !yaDevolvioPcb){

                    devolverPCBKernelOutOfMemory();
                }
            }
        }
    }

    printf("volvi al kernel\n");
}
