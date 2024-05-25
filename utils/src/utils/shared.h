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
typedef enum
{
	MENSAJE,
	PAQUETE,
	HANDSHAKE,
	RPTA_HANDSHAKE,
	//KERNEL-MEMORIA------
	CREAR_PROCESO_KM,
	RPTA_CREAR_PROCESO_MK,
	//KERNEL-CPU------
	CONSULTA_PLANIFICACION,
	RECIBIR_PCB,
	SOLICITUD_INSTRUCCION,
	RECIBIR_INSTRUCCION,
	INICIAR_CPU,

}op_code;

//semaforos

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
}registros;

typedef struct{
	int pid;
	int program_counter;
	int quantum;
	char* pathTXT;
	registros registros_cpu;
} PCB;

typedef struct{
	int id;
	char* path;
} path_conID;

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
uint32_t extraer_uint32_del_buffer(t_buffer *un_buffer);
uint8_t extraer_uint8_del_buffer(t_buffer *un_buffer);
t_buffer* crear_buffer();
void destruir_buffer(t_buffer* un_buffer);
void cargar_choclo_al_buffer(t_buffer* un_buffer, void* un_choclo, int size_choclo);
void cargar_int_al_buffer(t_buffer* un_buffer, int int_value);
void cargar_uint32_al_buffer(t_buffer* un_buffer, uint32_t un_valor);
void cargar_uint8_al_buffer(t_buffer* un_buffer, uint32_t un_valor);
void cargar_string_al_buffer(t_buffer* un_buffer, char* un_string);
t_paquete* crear_super_paquete(op_code cod_op, t_buffer* un_buffer);
void destruir_paquete(t_paquete* un_paquete);
int contarLineas(char *nombreArchivo);

#endif /* UTILS_H_ */