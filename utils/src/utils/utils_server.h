#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

extern t_log* logger;

void* recibir_buffer(int*, int);

int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
int crear_servidor(char *nombreCliente,char *ip,char *puerto);
void iterator(char* value);
int iniciar_servidor(char *ip, char *puerto);
t_config* iniciar_config(char *rutaConexion);
char* concatenar(char* str1, char* str2);

#endif /* UTILS_H_ */