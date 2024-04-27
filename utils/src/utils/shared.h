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

}op_code;

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


extern t_log* logger;

void* recibir_buffer(int*, int);

int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
void crear_servidor(char* puerto, char* nombreCliente);
void iterator(char* value);
int iniciar_servidor(char *puerto);

int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);

void iniciar_conexion(char* puerto,char *nombre);
t_log* iniciar_logger(void);
t_config* iniciar_configuracion(char *rutaConexion);
void leer_consola(t_log* logger);
void paquete(int conexion);
void terminar_programa(int conexion, t_log* logger, t_config* config);
char* concatenar(char* str1, char* str2);

#endif /* UTILS_H_ */