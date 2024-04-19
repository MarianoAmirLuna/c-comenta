#include "utils_server.h"
#include <commons/config.h>
#include <string.h>

t_log* logger;

int iniciar_servidor(char *ip,char *puerto)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip,puerto, &hints, &servinfo);	

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family,
                         servinfo->ai_socktype,
                         servinfo->ai_protocol);
	
	//creamos el socket (no importan los 3 parametros se ve en otra materia)

	// Asociamos el socket a un puerto

	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen); //linkea el socket y el puerto 
	//bind está recibiendo el puerto que debe ocupar a partir de los datos que le suministramos al getaddrinfo con anterioridad.

	// Escuchamos las conexiones entrantes

	listen(socket_servidor, SOMAXCONN); //marca en el sistema el socket cuya única responsabilidad es notificar cuando un nuevo cliente esté intentando conectarse.

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;

	//checkeado
}

int esperar_cliente(int socket_servidor)
{

	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL); //se queda esperando al que el cliente envie algo
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

///////////

int crear_servidor(char *nombreCliente,char *ip,char *puerto) {
	logger = log_create("log.log", "", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor(ip, puerto);

	log_info(logger, concatenar("listo para recibir al ", nombreCliente));
	int cliente_fd = esperar_cliente(server_fd); //dentro de esta funcion hay un accept la cual es bloqueante
	//retorna un nuevo socket (file descriptor) que representa la conexión BIDIRECCIONAL entre ambos procesos.

	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd); //en base a la operacion te da un valor para desp clasificar los datos que se enviaron en el switch
		//sirve para filtrar y saber que mandaste
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger,"Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

//aca borre iniciar config y concatenar
/*
t_config* iniciar_config(char *rutaConexion)
{
	t_config* nuevo_config = config_create(rutaConexion); //esto te pide la ruta del config

	if ( nuevo_config == NULL)	
	{
		perror("Hay un error al iniciar el config.");
	}
	
	return nuevo_config;

}

char* concatenar(char* str1, char* str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t len_total = len1 + len2 + 1; 
    
    char* resultado = (char*)malloc(len_total * sizeof(char));
    
    strcpy(resultado, str1);
 
    strcat(resultado, str2);
    
    return resultado;
}
*/