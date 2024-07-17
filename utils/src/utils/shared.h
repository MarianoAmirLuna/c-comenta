#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<assert.h>
#include<semaphore.h>
#include<math.h>
#include<stdint.h>
typedef enum
{
	MENSAJE,
	PAQUETE,
	HANDSHAKE,
	RPTA_HANDSHAKE,
	//KERNEL-MEMORIA------
	CREAR_PROCESO_KM,
	ELIMINAR_PROCESO,
	RPTA_CREAR_PROCESO_MK,
	//KERNEL-CPU------
	CONSULTA_PLANIFICACION,
	RECIBIR_PCB,
	SOLICITUD_INSTRUCCION,
	RECIBIR_INSTRUCCION,
	EJECUTAR_INSTRUCCION,
	SOLICITUD_PCB,
	CPU_LISTA,
	ENVIAR_PID,
	AVISO_DESALOJO,
	INICIAR_CPU,
	RECIBIR_TAMANIO,
	ESCRIBIR_IO_MEMORIA,
	DEVOLVER_MARCO,
	RECIBIR_MARCO,
	MANDAR_DATO_A_ESCRIBIR,
	MANDAR_DATO_A_LEER,
	PEDIR_DIRECCION,
	RECIBIR_DF,
	SEGUNDA_DIRECCION,
	SEGUNDA_DIRECCION_A_LEER,
	ESCRIBIO_PRIMERA_PARTE,
	LEYO_PRIMERA_PARTE,
	CANT_INTRUCCIONES,
	ESCRIBIR_MEMORIA,
	TERMINO_INSTRUCCION_MEMORIA,
	ENVIAR_IOGEN,
	DESALOJO_POR_WAIT,
	DESALOJO_POR_SIGNAL,
	LEER_CARACTER_MEMORIA,
	RECIBIR_CARACTER,
	
	//////////////////
	EJECUTAR_RESIZE,
	EJECUTAR_MOV_IN,
	EJECUTAR_MOV_OUT,
	DEVOLVER_TAMANIO_PAGINA,
	ESCRITURA_HECHA,
	LECTURA_HECHA,
	OUT_OF_MEMORY,

	//IO
	CREAR_INTERFAZ,

}op_code;

typedef enum
{
    SET,                // 0
    MOV_IN,             // 1
    MOV_OUT,            // 2
    SUM,                // 3
    SUB,                // 4
    JNZ,                // 5
    RESIZE,             // 6
    COPY_STRING,        // 7
    WAIT,               // 8
    SIGNAL,             // 9
    IO_GEN_SLEEP,       // 10
    IO_STDIN_READ,      // 11
    IO_STDOUT_WRITE,    // 12
    IO_FS_CREATE,       // 13
    IO_FS_DELETE,       // 14
    IO_FS_TRUNCATE,     // 15
    IO_FS_WRITE,        // 16
    IO_FS_READ,         // 17
    EXIT,               // 18
    INVALID_INSTRUCTION // Para manejar instrucciones no válidas
} nombre_instruccion;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct {
    void* data;    
} t_page;

typedef struct{
	uint8_t  AX;
	uint8_t  BX;
	uint8_t  CX;
	uint8_t  DX;
	uint32_t  EAX;
	uint32_t  EBX;
	uint32_t  ECX;
	uint32_t  EDX;
	uint32_t  SI;
	uint32_t  DI;
	uint8_t AUX1;
	uint32_t AUX2;
}registros;

typedef struct {
    int fd_interfaz;
	char* nombre_interfaz;
	char* tipo_interfaz;
} interfaces_io;

typedef struct{
	int pid;
	int program_counter;
	int quantum;
	registros registros_cpu;
} PCB;

typedef struct{
	int id;
	char* path;
} path_conID;

typedef struct{
	int marco;
	bool bitValidez;
} marcoBit;
typedef struct{
	int pid;
	int cantMarcos;
	marcoBit array[40];
} tablaPaginas;
typedef struct{
	int pid;
	int pagina;
	int marco;
} lineaTLB;

extern t_log* logger;

void* recibir_buffer(int*, int);

int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
int crear_servidor(char* puerto, char* nombreCliente);
void iterator(char* value);
int iniciar_servidor(char *puerto);

int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);

int iniciar_conexion(char* puerto,char *nombre,t_log* logger_debug);
t_log* iniciar_logger(void);
t_config* iniciar_configuracion(char *rutaConexion);
void leer_consola(t_log* logger);
void paquete(int conexion);
void terminar_programa(int conexion, t_log* logger, t_config* config);
char* concatenar(char* str1, char* str2);

/////////////////////////////
t_buffer* recibir_todo_el_buffer(int conexion);
void* extraer_choclo_del_buffer(t_buffer* un_buffer);
int extraer_int_del_buffer(t_buffer* un_buffer);
char *extraer_string_del_buffer(t_buffer* un_buffer);
uintptr_t extraer_uintptr_t_del_buffer(t_buffer* un_buffer);
uint32_t extraer_uint32_del_buffer(t_buffer *un_buffer);
uint8_t extraer_uint8_del_buffer(t_buffer *un_buffer);
t_buffer* crear_buffer();
void destruir_buffer(t_buffer* un_buffer);
void cargar_choclo_al_buffer(t_buffer* un_buffer, void* un_choclo, int size_choclo);
void cargar_uintptr_t_al_buffer(t_buffer* un_buffer, uintptr_t uintptr_t_value);
void cargar_int_al_buffer(t_buffer* un_buffer, int int_value);
void cargar_uint32_al_buffer(t_buffer* un_buffer, uint32_t un_valor);
void cargar_uint8_al_buffer(t_buffer* un_buffer, uint32_t un_valor);
void cargar_string_al_buffer(t_buffer* un_buffer, char* un_string);
t_paquete* crear_super_paquete(op_code cod_op, t_buffer* un_buffer);
void destruir_paquete(t_paquete* un_paquete);
int contarLineas(char *nombreArchivo);

PCB* inicializar_PCB(int, int, int, uint8_t, uint8_t, uint8_t,
					uint8_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

PCB* atender_recibir_pcb(t_buffer* un_buffer);
int contarInstrucciones(char *path);
int list_index_of(t_list *self, void *data);
t_buffer* cargar_pcb_buffer(PCB pcb);

#endif /* UTILS_H_ */