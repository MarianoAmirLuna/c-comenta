#include "shared.h"

#include <unistd.h>
#include <sys/socket.h>
#include <errno.h> // Necesario para errno y constantes de error como EINTR

t_log *logger;

/*int iniciar_conexion(char *puerto, char *nombre, t_log *logger_debug)
{
	// sleep(1);
	int socket_cliente = crear_conexion("127.0.0.1", puerto); // crear conexion te retorna el socket

	log_trace(logger_debug, concatenar("Se conecto correctamente el ", nombre));

	return socket_cliente;
}*/
int iniciar_conexion(char* ip, char *puerto, char *nombre, t_log *logger_debug)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,
								server_info->ai_socktype,
								server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo

	setsockopt(socket_cliente, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) < 0)
	{
		printf(" Salio del programa \n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

t_log *iniciar_logger(void)
{
	t_log *nuevo_logger = log_create("cliente.log", "", 1, LOG_LEVEL_INFO);

	if (nuevo_logger == NULL)
	{
		perror("Hay un error al iniciar el log.");
		exit(EXIT_FAILURE);
	}

	return nuevo_logger;
}

t_config *iniciar_configuracion(char *rutaConexion)
{
	t_config *nuevo_config = config_create(rutaConexion); // esto te pide la ruta del config

	if (nuevo_config == NULL)
	{
		perror("Hay un error al iniciar el config.");
		exit(EXIT_FAILURE);
	}

	return nuevo_config;
}

void leer_consola(t_log *logger)
{
	// La primera te la dejo de yapa
	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	char *linea;
	linea = readline(">");
	log_info(logger, ">> %s", linea);

	while (strcmp(linea, "") != 0)
	{
		free(linea);
		linea = readline(">");
		log_info(logger, ">> %s", linea);
	}
	// ¡No te olvides de liberar las lineas antes de regresar!
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char *leido = NULL;
	t_paquete *paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete

	leido = readline(">");
	while (strcmp(leido, "") != 0)
	{
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		free(leido);
		leido = readline("> ");
	}

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	free(leido);

	enviar_paquete(paquete, conexion);

	eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log *logger, t_config *config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config)
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
}

////////////////////

void *serializar_paquete(t_paquete *paquete, int bytes)
{
	void *magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	return magic;
}

int crear_conexion(char *ip, char *puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_servidor = socket(server_info->ai_family,
								 server_info->ai_socktype,
								 server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo

	connect(socket_servidor, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);

	return socket_servidor;
}

void enviar_mensaje(char *mensaje, int socket_cliente)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2 * sizeof(int);

	void *a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

/*
void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}
*/

t_paquete *crear_paquete(void)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete *paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void *a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0); // se me rompio la tuberia mal ahi

	free(a_enviar);
}

void eliminar_paquete(t_paquete *paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

char *concatenar(char *str1, char *str2)
{
	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);
	size_t len_total = len1 + len2 + 1;

	char *resultado = (char *)malloc(len_total * sizeof(char));

	strcpy(resultado, str1);

	strcat(resultado, str2);

	return resultado;
}

// ####################################
// ##############SERVER################
// ####################################

/*int iniciar_servidor(char *puerto)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL,puerto, &hints, &servinfo);

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
	//log_trace(logger, "listo para escuchar al cliente");

	return socket_servidor;

	//checkeado
}*/
int iniciar_servidor(char *puerto)
{

	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family,
							 servinfo->ai_socktype,
							 servinfo->ai_protocol);

	if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
		herror("setsockopt(SO_REUSEADDR) failed");

	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// printf(" MENSAJE LLEGO AL IF \n");

	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);
	freeaddrinfo(servinfo);
	// log_trace(aux_log, "Server: %s",msj_server);

	return socket_servidor;
}
/*
int esperar_cliente(int socket_servidor)
{
	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL); //se queda esperando al que el cliente envie algo
	//log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}*/
int esperar_cliente(int socket_servidor)
{
	// Aceptamos un nuevo cliente
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
	int socket_cliente = accept(socket_servidor, NULL, NULL);

	return socket_cliente;
}

/*int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}*/
int recibir_operacion(int socket_cliente)
{
int cod_op;
    ssize_t bytes_recibidos = recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL);
    if (bytes_recibidos > 0) {
        return cod_op;
    } else if (bytes_recibidos == 0) {
        printf("El cliente cerró la conexión.");
    } else {
        printf((char*)strerror(errno));
    }
    close(socket_cliente);
    return -1;
}

/*int recibir_operacion(int socket_cliente)
{
	int cod_op;
	ssize_t bytes_recibidos;
	int reintentos = 0;

	while (reintentos < 10) {
		bytes_recibidos = recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL);

		if (bytes_recibidos > 0) {
			return cod_op;
		} else if (bytes_recibidos == 0) {
			// El cliente cerró la conexión
			printf("Cliente cerró la conexión.\n");
			close(socket_cliente);
			return -1;
		} else {
			// Ocurrió un error
			printf("Error al recibir datos: %s\n", strerror(errno));

			if (errno == EINTR) {
				// Interrupción por señal, intentar de nuevo
				printf("recv fue interrumpido por una señal. Intentando de nuevo...\n");
				reintentos++;
				continue; // Intentar de nuevo
			} else {
				// Error irreparable
				close(socket_cliente);
				return -1;
			}
		}
	}

	// Si llegamos aquí, excedimos el número de reintentos permitidos
	printf("Excedido el número de reintentos permitidos (%d).\n", 10);
	close(socket_cliente);
	return -1;
}*/

/*int recibir_operacion(int socket_cliente) //la tercera es la vencida
{
	int cod_op;
	ssize_t bytes_recibidos;
	int reintentos = 0;

	while (reintentos < 10) {
		bytes_recibidos = recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL);

		if (bytes_recibidos > 0) {
			return cod_op;
		}
		else {
			// Ocurrió un error
			printf("Error al recibir datos: %s\n", strerror(errno));

			if (errno == EINTR) {
				// Interrupción por señal, intentar de nuevo
				printf("recv fue interrumpido por una señal. Intentando de nuevo...\n");
				reintentos++;
				continue; // Intentar de nuevo
			}
		}
	}

	// Si llegamos aquí, excedimos el número de reintentos permitidos
	close(socket_cliente);
	return -1;
}*/

void *recibir_buffer(int *size, int socket_cliente)
{
	void *buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char *buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list *recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void *buffer;
	t_list *valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while (desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		char *valor = malloc(tamanio);
		memcpy(valor, buffer + desplazamiento, tamanio);
		desplazamiento += tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

///////////

int crear_servidor(char *puerto, char *nombreCliente)
{
	logger = log_create("log.log", "", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor(puerto);

	log_info(logger, concatenar("listo para recibir al ", nombreCliente));
	int cliente_fd = esperar_cliente(server_fd); // dentro de esta funcion hay un accept la cual es bloqueante
	log_info(logger, "espero al cliente");
	// retorna un nuevo socket (file descriptor) que representa la conexión BIDIRECCIONAL entre ambos procesos.

	t_list *lista;
	while (1)
	{
		int cod_op = recibir_operacion(cliente_fd); // en base a la operacion te da un valor para desp clasificar los datos que se enviaron en el switch
		// sirve para filtrar y saber que mandaste
		switch (cod_op)
		{
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void *)iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger, "Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}

void iterator(char *value)
{
	log_info(logger, "%s", value);
}

//////////////////////////////////////////
t_buffer *recibir_todo_el_buffer(int conexion)
{
	t_buffer *un_buffer = malloc(sizeof(t_buffer));

	if (recv(conexion, &(un_buffer->size), sizeof(int), MSG_WAITALL) > 0)
	{
		un_buffer->stream = malloc(un_buffer->size);
		if (recv(conexion, un_buffer->stream, un_buffer->size, MSG_WAITALL) > 0)
		{
			return un_buffer;
		}
		else
		{
			perror("Error al recibir el void* del buffer de la conexion");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		perror("Error al recibir el tamanio del buffer de la conexion");
		exit(EXIT_FAILURE);
	}
	return un_buffer;
}

void *extraer_choclo_del_buffer(t_buffer *un_buffer)
{
	if (un_buffer->size == 0)
	{
		printf("\n[ERROR] Al intentar extraer un contenido de un t_buffer vacio");
		exit(EXIT_FAILURE);
	}
	if (un_buffer->size < 0)
	{
		printf("\n[ERROR] Esto es raro. el t_buffer contiene un size NEGATIVO \n");
		exit(EXIT_FAILURE);
	}

	int size_choclo;
	memcpy(&size_choclo, un_buffer->stream, sizeof(int));
	void *choclo = malloc(size_choclo);
	memcpy(choclo, un_buffer->stream + sizeof(int), size_choclo);

	int nuevo_size = un_buffer->size - sizeof(int) - size_choclo;

	if (nuevo_size == 0)
	{
		un_buffer->size = 0;
		free(un_buffer->stream);
		un_buffer->stream = NULL;
		return choclo;
	}
	if (nuevo_size < 0)
	{
		perror("\n[ERROR CHOCLO] BUFFER con tamanio negativo");
		exit(EXIT_FAILURE);
	}

	void *nuevo_stream = malloc(nuevo_size);
	memcpy(nuevo_stream, un_buffer->stream + sizeof(int) + size_choclo, nuevo_size);
	free(un_buffer->stream);
	un_buffer->size = nuevo_size;
	un_buffer->stream = nuevo_stream;

	return choclo;
}

uintptr_t extraer_uintptr_t_del_buffer(t_buffer *un_buffer)
{
	uintptr_t *un_uintptr_t = extraer_choclo_del_buffer(un_buffer);
	uintptr_t valor_retorno = *un_uintptr_t;
	free(un_uintptr_t);
	return valor_retorno;
}

int extraer_int_del_buffer(t_buffer *un_buffer)
{
	int *un_entero = extraer_choclo_del_buffer(un_buffer);
	int valor_retorno = *un_entero;
	free(un_entero);
	return valor_retorno;
}
char *extraer_string_del_buffer(t_buffer *un_buffer)
{
	char *un_string = extraer_choclo_del_buffer(un_buffer);
	return un_string;
}

uint32_t extraer_uint32_del_buffer(t_buffer *un_buffer)
{
	uint32_t *un_valor = extraer_choclo_del_buffer(un_buffer);
	uint32_t valor_retorno = *un_valor;
	free(un_valor);
	return valor_retorno;
}

uint8_t extraer_uint8_del_buffer(t_buffer *un_buffer)
{
	uint8_t *un_valor = extraer_choclo_del_buffer(un_buffer);
	uint8_t valor_retorno = *un_valor;
	free(un_valor);
	return valor_retorno;
}

t_buffer *crear_buffer()
{
	t_buffer *un_buffer = malloc(sizeof(t_buffer));
	un_buffer->size = 0;
	un_buffer->stream = NULL;
	return un_buffer;
}

void destruir_buffer(t_buffer *un_buffer)
{
	if (un_buffer->stream != NULL)
	{
		free(un_buffer->stream);
	}
	free(un_buffer);
}

void cargar_choclo_al_buffer(t_buffer *un_buffer, void *un_choclo, int size_choclo)
{
	if (un_buffer->size == 0)
	{
		un_buffer->stream = malloc(sizeof(int) + size_choclo);
		memcpy(un_buffer->stream, &size_choclo, sizeof(int));
		memcpy(un_buffer->stream + sizeof(int), un_choclo, size_choclo);
	}
	else
	{
		un_buffer->stream = realloc(un_buffer->stream, un_buffer->size + sizeof(int) + size_choclo);
		memcpy(un_buffer->stream + un_buffer->size, &size_choclo, sizeof(int));
		memcpy(un_buffer->stream + un_buffer->size + sizeof(int), un_choclo, size_choclo);
	}
	un_buffer->size += sizeof(int);
	un_buffer->size += size_choclo;
}

void cargar_uintptr_t_al_buffer(t_buffer *un_buffer, uintptr_t uintptr_t_value)
{
	cargar_choclo_al_buffer(un_buffer, &uintptr_t_value, sizeof(uintptr_t));
}

void cargar_int_al_buffer(t_buffer *un_buffer, int int_value)
{
	cargar_choclo_al_buffer(un_buffer, &int_value, sizeof(int));
}

void cargar_uint32_al_buffer(t_buffer *un_buffer, uint32_t un_valor)
{
	cargar_choclo_al_buffer(un_buffer, &un_valor, sizeof(uint32_t));
}

void cargar_uint8_al_buffer(t_buffer *un_buffer, uint32_t un_valor)
{
	cargar_choclo_al_buffer(un_buffer, &un_valor, sizeof(uint8_t));
}

void cargar_string_al_buffer(t_buffer *un_buffer, char *un_string)
{
	cargar_choclo_al_buffer(un_buffer, un_string, strlen(un_string) + 1);
}

//==============================

t_paquete *crear_super_paquete(op_code cod_op, t_buffer *un_buffer)
{
	t_paquete *un_paquete = malloc(sizeof(t_paquete));
	un_paquete->codigo_operacion = cod_op;
	un_paquete->buffer = un_buffer;
	return un_paquete;
}

void destruir_paquete(t_paquete *un_paquete)
{
	destruir_buffer(un_paquete->buffer);
	free(un_paquete);
}

int contarLineas(char *nombreArchivo)
{

	FILE *archivo = fopen(nombreArchivo, "r");
	if (archivo == NULL)
	{
		perror("Error al abrir el archivo");
		return -1;
	}

	int lineas = 0;
	int caracter;

	while ((caracter = fgetc(archivo)) != EOF)
	{
		if (caracter == '\n')
		{
			lineas++;
		}
	}

	fclose(archivo);
	return lineas;
}

PCB *atender_recibir_pcb(t_buffer *un_buffer)
{
	int pid = extraer_int_del_buffer(un_buffer);
	int program_counter = extraer_int_del_buffer(un_buffer);
	int quantum = extraer_int_del_buffer(un_buffer);
	uint8_t ax = extraer_uint8_del_buffer(un_buffer);
	uint8_t bx = extraer_uint8_del_buffer(un_buffer);
	uint8_t cx = extraer_uint8_del_buffer(un_buffer);
	uint8_t dx = extraer_uint8_del_buffer(un_buffer);
	uint32_t eax = extraer_uint32_del_buffer(un_buffer);
	uint32_t ebx = extraer_uint32_del_buffer(un_buffer);
	uint32_t ecx = extraer_uint32_del_buffer(un_buffer);
	uint32_t edx = extraer_uint32_del_buffer(un_buffer);
	uint32_t si = extraer_uint32_del_buffer(un_buffer);
	uint32_t di = extraer_uint32_del_buffer(un_buffer);

	PCB *pcb = inicializar_PCB(pid, program_counter, quantum, ax, bx, cx, dx, eax, ebx, ecx, edx, si, di);
	return pcb;
}

PCB *inicializar_PCB(int pid, int program_counter, int quantum, uint8_t ax, uint8_t bx, uint8_t cx,
					 uint8_t dx, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t si, uint32_t di)
{

	PCB *pcb = malloc(sizeof(PCB));

	pcb->pid = pid;
	pcb->program_counter = program_counter;
	pcb->quantum = quantum;
	pcb->registros_cpu.AX = ax;
	pcb->registros_cpu.BX = bx;
	pcb->registros_cpu.CX = cx;
	pcb->registros_cpu.DX = dx;
	pcb->registros_cpu.EAX = eax;
	pcb->registros_cpu.EBX = ebx;
	pcb->registros_cpu.ECX = ecx;
	pcb->registros_cpu.EDX = edx;
	pcb->registros_cpu.SI = si;
	pcb->registros_cpu.DI = di;

	return pcb;

} // recibe todos los registros por eso es tan largo

int contarInstrucciones(char *path)
{ // Le pasas un .txt y te dice cuantas instrucciones tiene
	FILE *archivo = fopen(path+1, "r");
	if (archivo == NULL)
	{
		perror("Error al abrir el archivo");
		return -1;
	}

	int contador = 0;
	char linea[256];

	// Leer el archivo línea por línea y contar cada línea
	while (fgets(linea, sizeof(linea), archivo)) 
	{
		contador++;
	}

	fclose(archivo);
	return contador;
}

/*
int list_index_of(t_list *self, void *data)
{
	int index = 0;
	t_link_element *current = self->head;
	while (current != NULL)
	{
		if (current->data == data)
		{
			return index;
		}
		current = current->next;
		index++;
	}
	return -1; // Elemento no encontrado
}*/

t_buffer *cargar_pcb_buffer(PCB pcb)
{
	t_buffer *buffer = crear_buffer();

	buffer->size = 0;
	buffer->stream = NULL;

	cargar_int_al_buffer(buffer, pcb.pid);
	cargar_int_al_buffer(buffer, pcb.program_counter);
	cargar_int_al_buffer(buffer, pcb.quantum);
	cargar_uint8_al_buffer(buffer, pcb.registros_cpu.AX);
	cargar_uint8_al_buffer(buffer, pcb.registros_cpu.BX);
	cargar_uint8_al_buffer(buffer, pcb.registros_cpu.CX);
	cargar_uint8_al_buffer(buffer, pcb.registros_cpu.DX);
	cargar_uint32_al_buffer(buffer, pcb.registros_cpu.EAX);
	cargar_uint32_al_buffer(buffer, pcb.registros_cpu.EBX);
	cargar_uint32_al_buffer(buffer, pcb.registros_cpu.ECX);
	cargar_uint32_al_buffer(buffer, pcb.registros_cpu.EDX);
	cargar_uint32_al_buffer(buffer, pcb.registros_cpu.SI);
	cargar_uint32_al_buffer(buffer, pcb.registros_cpu.DI);

	return buffer;
}

bool contiene_numero(t_list *lista, int numero)
{
    int longitud = list_size(lista);
    for (int i = 0; i < longitud; i++)
    {
        int *elemento = (int *)list_get(lista, i);
        if (*elemento == numero)
        {
            return true; // El número está en la lista
        }
    }
    return false; // El número no está en la lista
}

void removerNumeroLista(t_list* lista,int numero){

	for(int i = 0; i < list_size(lista); i++){

		int* numeroXD = list_get(lista,i);

		if(*numeroXD == numero){

			int* numerillo = list_remove(lista,i);
			free(numerillo);
		}
	}
}

void safe_int_destroyer(void* data) {
    if (data != NULL) {
        // Asegúrate de que el puntero no sea nulo antes de liberar
        // printf("Destruyendo entero: %d\n", *(int*)data);
        free(data);
    }
}

// Función para destruir la lista y sus elementos con manejo de errores
void safe_list_destroy_and_destroy_elements(t_list* list, void(*element_destroyer)(void*)) {
    if (list == NULL) {
        return;
    }

    if (element_destroyer == NULL) {
        return;
    }

    int size = list_size(list);
    void** elements = malloc(size * sizeof(void*)); // Array para almacenar los elementos

    if (elements == NULL) {
        // Manejo de errores si malloc falla
        return;
    }

    // Copiar elementos de la lista a un array
    for (int i = 0; i < size; ++i) {
        elements[i] = list_get(list, i);
    }

    // Destruir cada elemento y evitar doble liberación
    for (int i = 0; i < size; ++i) {
        if (elements[i] != NULL) {
            element_destroyer(elements[i]);
            elements[i] = NULL; // Marcar como liberado
        }
    }

    // Liberar el array de elementos
    free(elements);

    // Destruir la lista
    list_destroy(list);
}

void free_instruccion(instruccion* inst) {
    if (inst == NULL) {
        return;
    }

    // Liberar nombre_instruccion si no es nulo
    if (inst->nombre_instruccion != NULL) {
        free(inst->nombre_instruccion);
        inst->nombre_instruccion = NULL; // Marcamos el puntero como NULL para evitar problemas
    }

    // Liberar nombre_archivo si no es nulo
    if (inst->nombre_archivo != NULL) {
        free(inst->nombre_archivo);
        inst->nombre_archivo = NULL; // Marcamos el puntero como NULL para evitar problemas
    }

    // Liberar lista_enteros si no es nulo
    if (inst->lista_enteros != NULL) {
        int size = list_size(inst->lista_enteros);
        for (int i = 0; i < size; ++i) {
            void* element = list_get(inst->lista_enteros, i);
            if (element != NULL) {
                free(element); // Suponiendo que los elementos son punteros a enteros o a otros datos dinámicos
            }
        }
        list_destroy(inst->lista_enteros); // Destruir la lista
        inst->lista_enteros = NULL; // Marcamos la lista como NULL
    }

    // Finalmente liberar la estructura misma
    free(inst);
}