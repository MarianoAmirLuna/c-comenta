
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

void _set(char *registro, char *valor)
{
    uint32_t *destino = get_registry(registro);
    *destino = atoi(valor);
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
    }
    else
    { // si es un u32

        if (bytes_restantes_en_pagina < 4)
        { // se tiene que leer en 2 paginas diferentes

            dirLogicaDelDato = dirLogicaDelDato + bytes_restantes_en_pagina;
            int segundaDF = traducir_dl(dirLogicaDelDato);
            mandarDatoALeer(dirFisicaDelDato, segundaDF, 4, 1, bytes_restantes_en_pagina, registroDatos);
        }
        else
        { // Entra entero, osea que no se tiene que escribir en 2 paginas diferentes
            mandarDatoALeer(dirFisicaDelDato, 0, 4, 0, bytes_restantes_en_pagina, registroDatos);
        }
    }
}

void _mov_in(char *registroDatos, char *registroDireccion)
{
    // obtengo el valor de los registros y se los paso
    void *direccionLogicaDelDato = (void *)get_registry(registroDireccion);

    int tamanioDatoALeer = conocerTamanioDeLosRegistros(registroDatos); // para saber el tamaño de lo que voy a leer
    printf("el tamanio del dato: %d\n", tamanioDatoALeer);

    int *dirLogicaDelDato = (int *)direccionLogicaDelDato;

    hacerMovIn(*dirLogicaDelDato, tamanioDatoALeer, registroDatos);
}

void mandarDatoAEscribir(int direccion_logica, int direccion_fisica, int segundaDF, void *queEscribir, int bytes_a_escribir, int seEscribe2paginas, int tamanioRestantePagina)
{
    t_buffer *a_enviar = crear_buffer();
    a_enviar->size = 0;
    a_enviar->stream = NULL;

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
    }
    else
    { // si es un u32
        printf("los bytes_restantes_en_pagina son: %d\n", bytes_restantes_en_pagina);
        if (bytes_restantes_en_pagina < 4)
        { // se tiene que escribir en 2 paginas diferentes

            printf("Entro al if turbio\n");
            direccionLogica = direccionLogica + bytes_restantes_en_pagina;
            int segundaDF = traducir_dl(direccionLogica);
            mandarDatoAEscribir(direccionLogica, direccion_fisica, segundaDF, dato, 4, 1, bytes_restantes_en_pagina);
        }
        else
        { // no se tiene que escribir en 2 paginas diferentes
            printf("Entro al else turbio\n");
            mandarDatoAEscribir(direccionLogica, direccion_fisica, 0, dato, 4, 0, bytes_restantes_en_pagina);
        }
    }
}

//
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
    uint32_t *registroAComparar = get_registry(registro);
    int numeroDeInstr = atoi(instruccion);

    if (*registroAComparar != 0 )
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

void _copy_string(char *tamanio)
{
    int desplazamiento_en_pagina = (int)pcb_ejecucion.registros_cpu.DI % tamanio_pagina; // offset
    int bytes_restantes_en_pagina = tamanio_pagina - desplazamiento_en_pagina;           // cuanto queda en la pagina

    int tamanioAEscribir = atoi(tamanio);

    char str[tamanioAEscribir + 1]; // Inicializa el string como vacío
    str[0] = '\0';
    pcb_ejecucion.registros_cpu.AUX2 = pcb_ejecucion.registros_cpu.SI;

    for (int i = 0; i < tamanioAEscribir; i++)
    {
        _mov_in("AUX1", "AUX2");
        concat_uint8_to_string(str, pcb_ejecucion.registros_cpu.AUX1);
        pcb_ejecucion.registros_cpu.AUX2++;
    }

    printf("$$$$$ El string es: %s\n", str);

    int cantDireccionesNecesarias = obtener_cant_direcciones((int)pcb_ejecucion.registros_cpu.DI, tamanioAEscribir, bytes_restantes_en_pagina);

    t_buffer *buffer = crear_buffer();
    buffer->size = 0;
    buffer->stream = NULL;

    cargar_string_al_buffer(buffer, str);
    cargar_int_al_buffer(buffer, bytes_restantes_en_pagina);
    cargar_int_al_buffer(buffer, tamanioAEscribir);

    pcb_ejecucion.registros_cpu.AUX2 = pcb_ejecucion.registros_cpu.DI;
    int flag = 0;

    for (int i = 0; i < cantDireccionesNecesarias; i++)
    {
        int df = traducir_dl((int)pcb_ejecucion.registros_cpu.AUX2);
        cargar_int_al_buffer(buffer, df);

        if (flag == 0)
        {
            pcb_ejecucion.registros_cpu.AUX2 = pcb_ejecucion.registros_cpu.AUX2 + bytes_restantes_en_pagina;
            flag = 1;
        }
        else
        {
            pcb_ejecucion.registros_cpu.AUX2 = pcb_ejecucion.registros_cpu.AUX2 + tamanio_pagina;
        }
        printf("carge un int al buffer\n");
    }

    t_paquete *paquete = crear_super_paquete(EJECUTAR_CPYSTRING, buffer);
    enviar_paquete(paquete, fd_memoria);
    destruir_paquete(paquete);
}

void ioGenSleep(char *nombreInterfaz, char *unidadesTrabajo)
{
    int uniTra = atoi(unidadesTrabajo);

    t_buffer *buffer_IOKernel = crear_buffer();
    buffer_IOKernel->size = 0;
    buffer_IOKernel->stream = NULL;
    cargar_string_al_buffer(buffer_IOKernel,nombreInterfaz);
    cargar_int_al_buffer(buffer_IOKernel, unidadesTrabajo);
    t_paquete *paquete_IOKernel = crear_super_paquete(ENVIAR_IOGEN, buffer_IOKernel);
    enviar_paquete(paquete_IOKernel, fd_kernel_dispatch);
    destruir_paquete(paquete_IOKernel);

    
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

    //solo van los semaforos en las funciones que solo se ejecutan en CPU las demas memoria/ IO
    //le avisan a CPU de que termino y recien ahi se liberan

    switch (instruction)
    {
    case SET:
        _set(param1, param2);
         sem_post(&wait_instruccion);
        break;
    case MOV_IN:
        _mov_in(param1, param2);

        break;
    case MOV_OUT:
        _mov_out(param1, param2);

        break;
    case SUM:
        _sum(param1, param2);
         sem_post(&wait_instruccion);
        break;
    case SUB:
        _sub(param1, param2);
        sem_post(&wait_instruccion);
        break;
    case JNZ:
        _jnz(param1, param2);
        sem_post(&wait_instruccion);
        break;
    case RESIZE:
        _resize(param1);
        
        break;
    case COPY_STRING:
        _copy_string(param1);

        break;
    case WAIT:
        sem_post(&wait_instruccion);
        break;
    case SIGNAL:
        sem_post(&wait_instruccion);
        break;
    case IO_GEN_SLEEP:
        ioGenSleep(param1, param2);
        break;
    case IO_STDIN_READ:
        ioSTDINRead()
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
        sem_post(&wait_instruccion);
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

void actualizarPrioridadesTLB(lineaTLB lineaTL)
{

    int index = list_index_of(cola_tlb->elements, &lineaTL); // obtengo el index del que quiero borrar

    lineaTLB *lineaRemovida = list_remove(cola_tlb->elements, index); // lo borro

    queue_push(cola_tlb, lineaRemovida); // lo vuelvo a agregar al final
}

void agregarPaginaTLB(int pid, int pagina, int marco)
{
    lineaTLB *lineaTL = inicializarLineaTLB(pid, pagina, marco);

    if (queue_size(cola_tlb) < CANTIDAD_ENTRADAS_TLB)
    { // si entra aca es porque no hay que hacer ningun reemplazo
        // printf("VOY A AGREGAR LA LINEA A LA COLA CON MARCO: %d\n",lineaTL->marco);
        queue_push(cola_tlb, lineaTL);
    }
    else
    { // significa que ya se lleno la TLB entonces hay que reemplazar una

        lineaTLB *lineaABorrar = queue_pop(cola_tlb);
        free(lineaABorrar);
        queue_push(cola_tlb, lineaTL);

        lineaTLB *linea1 = list_get(cola_tlb->elements, 0);

        // printf("pagina 1: %d\n",linea1->pagina);
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
    { // se produce un MISS al no encontrarlo
        printf("se produjo un MISS\n");
        return -1;
    }
    printf("se produjo un HIT\n");

    if (strcmp(ALGORITMO_TLB, "LRU") == 0)
    { // en caso de que sea LRU hay que actualizar la prioridad
        // printf("actualize la prioridad del LRU\n");
        actualizarPrioridadesTLB(*lineaTL);
    }

    return lineaTL->marco; // encontro el marco y lo devuelve
}

int traducir_dl(int direccionLogica)
{
    if (primeraSolicitudTamanioDePagina) // sirve para obtener el tamanio de pagina de memoria
    {
        solicitarTamanioPagina();
        primeraSolicitudTamanioDePagina = false;
    }

    int num_pag = direccionLogica / tamanio_pagina;

    int desplazamiento = direccionLogica - num_pag * tamanio_pagina;

    marco = buscarMarcoTLB(pcb_ejecucion.pid, num_pag);

    if (marco != -1)
    { // hubo un HIT

        return (marco * tamanio_pagina + desplazamiento);
    }

    enviar_pedido_marco(num_pag, pcb_ejecucion.pid); // en caso de un miss busca en memoria

    agregarPaginaTLB(pcb_ejecucion.pid, num_pag, marco); // despues del miss se actualiza la TLB

    int sizeTLB = queue_size(cola_tlb);

    return (marco * tamanio_pagina + desplazamiento);
}

////////////////////////////////////////////////////

int obtener_cantidad_instrucciones(int pid)
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

void devolverPCBKernel()
{
    t_buffer *a_enviar = crear_buffer();

    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_int_al_buffer(a_enviar, pcb_ejecucion.pid);
    cargar_int_al_buffer(a_enviar, pcb_ejecucion.program_counter);
    cargar_int_al_buffer(a_enviar, pcb_ejecucion.quantum);
    cargar_uint8_al_buffer(a_enviar, pcb_ejecucion.registros_cpu.AX);
    cargar_uint8_al_buffer(a_enviar, pcb_ejecucion.registros_cpu.BX);
    cargar_uint8_al_buffer(a_enviar, pcb_ejecucion.registros_cpu.CX);
    cargar_uint8_al_buffer(a_enviar, pcb_ejecucion.registros_cpu.DX);
    cargar_uint32_al_buffer(a_enviar, pcb_ejecucion.registros_cpu.EAX);
    cargar_uint32_al_buffer(a_enviar, pcb_ejecucion.registros_cpu.EBX);
    cargar_uint32_al_buffer(a_enviar, pcb_ejecucion.registros_cpu.ECX);
    cargar_uint32_al_buffer(a_enviar, pcb_ejecucion.registros_cpu.EDX);
    cargar_uint32_al_buffer(a_enviar, pcb_ejecucion.registros_cpu.SI);
    cargar_uint32_al_buffer(a_enviar, pcb_ejecucion.registros_cpu.DI);

    if (cambioContexto)
    {
        cargar_int_al_buffer(a_enviar, 1); // si hay cambio de contexto envio un 1 osea fue desalojado => le faltan instrucciones por ejecutar
    }
    else
    {
        cargar_int_al_buffer(a_enviar, 2); // si se queda sin instrucciones va un 2
    }

    t_paquete *un_paquete = crear_super_paquete(RECIBIR_PCB, a_enviar);
    enviar_paquete(un_paquete, fd_kernel_dispatch);
    destruir_paquete(un_paquete);
}

void procesar_instruccion()
{
    obtener_cantidad_instrucciones(pcb_ejecucion.pid);

    printf("la cantidad de instrucciones son: %d\n", cantInstucciones);

    while (cantInstucciones >= pcb_ejecucion.program_counter && !cambioContexto)
    {
        solicitar_instruccion(pcb_ejecucion.pid, pcb_ejecucion.program_counter);

        sem_wait(&wait_instruccion);

        printf("se ejecuto la instruccion\n");
        ejecutar_instruccion(instruccion_actual, &pcb_ejecucion);

        sem_wait(&wait_instruccion);

        printf("el PID: %d\n",pcb_ejecucion.pid);
        printf("Estado de los registros:\n");
        printf("AX: %d, BX: %d, CX: %d, DX: %d\n", pcb_ejecucion.registros_cpu.AX, pcb_ejecucion.registros_cpu.BX, pcb_ejecucion.registros_cpu.CX, pcb_ejecucion.registros_cpu.DX);
        printf("EAX: %u, EBX: %u, ECX: %u, EDX: %u\n", pcb_ejecucion.registros_cpu.EAX, pcb_ejecucion.registros_cpu.EBX, pcb_ejecucion.registros_cpu.ECX, pcb_ejecucion.registros_cpu.EDX);
        printf("PC: %d\n\n", pcb_ejecucion.program_counter);
        printf("------------------------------------------------\n\n");

        pcb_ejecucion.program_counter++;
    }

    // sale del while o porque se queda sin instrucciones o porque es desalojado
    devolverPCBKernel();
    printf("termino de ejecutar\n");
    cambioContexto = false;
}
