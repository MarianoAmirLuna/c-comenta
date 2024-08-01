#include "../include/memoria_io.h"
#include <utils/shared.h>
#include "../include/memoria_cpu.h"

void aviso_finalizacion_a_IO(int fd)
{
	t_buffer *buffer = crear_buffer();
	buffer->size = 0;
	buffer->stream = NULL;

	cargar_int_al_buffer(buffer, 1);

	t_paquete *paquete = crear_super_paquete(LIBERAR_INTERFAZ, buffer);
	enviar_paquete(paquete, fd);
	destruir_paquete(paquete);
}

int encontrar_fd_interfaz(char *nombre_buscado)
{
	for (int i = 0; i < list_size(lista_interfaces); i++)
	{
		interfaces_io_memoria *elemento = list_get(lista_interfaces, i);

		if (strcmp(elemento->nombre_interfaz, nombre_buscado) == 0)
		{
			//printf("encontre el fd\n");
			return elemento->fd_interfaz;
		}
	}

	return -1; // No se encontró la interfaz con el nombre buscado
}

void leerMemoria(t_buffer *un_buffer)
{
	char *nombre_interfaz = extraer_string_del_buffer(un_buffer);
	int fd_encontrado = encontrar_fd_interfaz(nombre_interfaz);

	//printf("el nombre_interfaz: %s\n", nombre_interfaz);
	//printf("su fd %d\n", fd_encontrado);

	int length = extraer_int_del_buffer(un_buffer);

	//printf("llegue a leer memoria\n");

	usleep(1000 * RETARDO_RESPUESTA);
	//printf("me fui a mimir 1 sg\n");

	char palabra[length + 1];
	palabra[length] = '\0';

	for (int i = 0; i < length; i++)
	{

		uint8_t datoLeido;
		int df = extraer_int_del_buffer(un_buffer);
		memcpy(&datoLeido, memoriaPrincipal + df, 1);
		palabra[i] = (char)datoLeido;
	}

	//printf("la palabra es: %s\n", palabra);

	char *palabra_final = (char *)malloc(length + 1);

	memcpy(palabra_final, palabra, length);
	// Asegura que la cadena esté terminada con un carácter nulo
	palabra_final[length] = '\0';

	t_buffer *buffer = crear_buffer();
	buffer->size = 0;
	buffer->stream = NULL;

	cargar_string_al_buffer(buffer, palabra_final);

	t_paquete *paquete = crear_super_paquete(DEVOLVER_STRING_STDOUT, buffer);
	enviar_paquete(paquete, fd_encontrado);
	destruir_paquete(paquete);
}

void leerMemoriaFS(t_buffer *un_buffer)
{
	char *nombre_interfaz = extraer_string_del_buffer(un_buffer);
	int fd_encontrado = encontrar_fd_interfaz(nombre_interfaz);

	//printf("el nombre_interfaz: %s\n", nombre_interfaz);
	//printf("su fd %d\n", fd_encontrado);

	int length = extraer_int_del_buffer(un_buffer);

	//printf("llegue a leer memoria\n");

	usleep(1000 * RETARDO_RESPUESTA);
	//printf("me fui a mimir 1 sg\n");

	char palabra[length + 1];
	palabra[length] = '\0';

	for (int i = 0; i < length; i++)
	{

		uint8_t datoLeido;
		int df = extraer_int_del_buffer(un_buffer);
		memcpy(&datoLeido, memoriaPrincipal + df, 1);
		palabra[i] = (char)datoLeido;
	}

	//printf("la palabra es: %s\n", palabra);

	char *palabra_final = (char *)malloc(length + 1);

	memcpy(palabra_final, palabra, length);
	// Asegura que la cadena esté terminada con un carácter nulo
	palabra_final[length] = '\0';

	t_buffer *buffer = crear_buffer();
	buffer->size = 0;
	buffer->stream = NULL;

	cargar_string_al_buffer(buffer, palabra_final);

	t_paquete *paquete = crear_super_paquete(DEVOLVER_STRING_FS, buffer);
	enviar_paquete(paquete, fd_encontrado);
	destruir_paquete(paquete);
}

void leerMemoriaUnString_io(t_buffer *un_buffer)
{
	int pid = extraer_int_del_buffer(un_buffer);
	char *nameInterface = extraer_string_del_buffer(un_buffer);
	int fd_encontradoXD = encontrar_fd_interfaz(nameInterface);
	int bytes_restantes_en_pagina = extraer_int_del_buffer(un_buffer);
	int tamanioALeer = extraer_int_del_buffer(un_buffer);

	int cantIteraciones = 0;
	t_list *cortesPagina = list_create();

	obtenerCortesDePagina(cortesPagina, tamanioALeer, bytes_restantes_en_pagina); // [10,42,74] Cada cuantos bytes necesitas un nuevo marco (osea una nueva df)

	int longitud = list_size(cortesPagina);

	for (int i = 0; i < longitud; i++)
	{
		int *pepe = (int *)list_get(cortesPagina, i);
		//printf("valor de la lista cortes: %d\n", *pepe);
	}

	//printf("la longitud es: %d\n", longitud);

	int df = extraer_int_del_buffer(un_buffer);
	uint8_t infoLeida;
	char str[tamanioALeer + 1]; // Inicializa el string como vacío
	str[0] = '\0';

	log_info(memoria_log_debug, "El PID: %d - REALIZARÁ UNA LECTURA DE TAMAÑO %d EN LAS DIRECCIONES FÍSICAS:", pid, tamanioALeer);
	while (cantIteraciones < tamanioALeer)
	{

		if (necesitoNuevaDF(cortesPagina, cantIteraciones))
		{
			df = extraer_int_del_buffer(un_buffer);
		}

		log_info(memoria_log_debug, "DIRECCION FÍSICA: %d", df);
		//printf("en la direccion fisica: %d\n", df);
		memcpy(&infoLeida, memoriaPrincipal + df, 1);
		concat_uint8_to_string(str, infoLeida);
		cantIteraciones++;
		df++;
	}

	//printf("cantidad de iteraciones: %d\n", cantIteraciones);

	//printf("###################### MENSAJE LEIDO: %s \n", str);

	t_buffer *buffer = crear_buffer();
	buffer->size = 0;
	buffer->stream = NULL;

	cargar_string_al_buffer(buffer, str);

	t_paquete *paquete = crear_super_paquete(DEVOLVER_STRING_STDOUT, buffer);
	enviar_paquete(paquete, fd_encontradoXD);
	destruir_paquete(paquete);
	//safe_list_destroy_and_destroy_elements(cortesPagina, safe_int_destroyer);
}

void leerMemoriaUnString_fs(t_buffer *un_buffer)
{
	int pid = extraer_int_del_buffer(un_buffer);
	char *nameInterface = extraer_string_del_buffer(un_buffer);
	int fd_encontradoXD = encontrar_fd_interfaz(nameInterface);
	int bytes_restantes_en_pagina = extraer_int_del_buffer(un_buffer);
	int tamanioALeer = extraer_int_del_buffer(un_buffer);

	int cantIteraciones = 0;
	t_list *cortesPagina = list_create();

	obtenerCortesDePagina(cortesPagina, tamanioALeer, bytes_restantes_en_pagina); // [10,42,74] Cada cuantos bytes necesitas un nuevo marco (osea una nueva df)

	int longitud = list_size(cortesPagina);

	for (int i = 0; i < longitud; i++)
	{
		int *pepe = (int *)list_get(cortesPagina, i);
		//printf("valor de la lista cortes: %d\n", *pepe);
	}

	//printf("la longitud es: %d\n", longitud);

	int df = extraer_int_del_buffer(un_buffer);
	uint8_t infoLeida;
	char str[tamanioALeer + 1]; // Inicializa el string como vacío
	str[0] = '\0';

	log_info(memoria_log_debug, "El PID: %d - REALIZARÁ UNA LECTURA DE TAMAÑO %d EN LAS DIRECCIONES FÍSICAS:", pid, tamanioALeer);
	while (cantIteraciones < tamanioALeer)
	{

		if (necesitoNuevaDF(cortesPagina, cantIteraciones))
		{
			df = extraer_int_del_buffer(un_buffer);
		}

		log_info(memoria_log_debug, "DIRECCION FÍSICA: %d", df);
		//printf("en la direccion fisica: %d\n", df);
		memcpy(&infoLeida, memoriaPrincipal + df, 1);
		concat_uint8_to_string(str, infoLeida);
		cantIteraciones++;
		df++;
	}

	//printf("cantidad de iteraciones: %d\n", cantIteraciones);

	//printf("###################### MENSAJE LEIDO: %s \n", str);

	t_buffer *buffer = crear_buffer();
	buffer->size = 0;
	buffer->stream = NULL;

	cargar_string_al_buffer(buffer, str);

	t_paquete *paquete = crear_super_paquete(DEVOLVER_STRING_FS, buffer);
	enviar_paquete(paquete, fd_encontradoXD);
	destruir_paquete(paquete);
	//safe_list_destroy_and_destroy_elements(cortesPagina, safe_int_destroyer);
}

void atender_creacion_interfaz(int *arg)
{
	int fd_entradasalida_memoria = *arg;
	free(arg);
	bool control_key = 1;
	t_buffer *un_buffer;

	while (control_key)
	{
		int cod_op = recibir_operacion(fd_entradasalida_memoria);
		//printf("el codigo de operacion es: %d\n", cod_op);
		switch (cod_op)
		{
		case CREAR_INTERFAZ:
			//printf("estoy por crear la interfaz\n");

			interfaces_io_memoria *nueva_interfaz = (interfaces_io_memoria *)malloc(sizeof(interfaces_io_memoria));

			un_buffer = recibir_todo_el_buffer(fd_entradasalida_memoria);

			char *nombre_interfaz = extraer_string_del_buffer(un_buffer);

			//printf("el nombre dela interfaz: %s\n", nombre_interfaz);

			nueva_interfaz->fd_interfaz = fd_entradasalida_memoria;
			nueva_interfaz->nombre_interfaz = nombre_interfaz;

			list_add(lista_interfaces, nueva_interfaz);

			//printf("agrege la nueva interfaz a la queue\n");

			int size = list_size(lista_interfaces);

			//printf("el tamanio de la list interfaces: %d\n", size);

			break;

		case ESCRIBIR_MEMORIA_IO:

			un_buffer = recibir_todo_el_buffer(fd_entradasalida_memoria);
			char *nombre_interfazXD = extraer_string_del_buffer(un_buffer);
			int fd_interfazXD = encontrar_fd_interfaz(nombre_interfazXD);
			escribirMemoria(un_buffer);
			aviso_finalizacion_a_IO(fd_interfazXD);

			break;

		case LEER_MEMORIA_PALABRA: // probablemente haya que sacarlo
			un_buffer = recibir_todo_el_buffer(fd_entradasalida_memoria);
			leerMemoria(un_buffer);
			break;
		case LEER_MEMORIA_PALABRA_DIALS_FS: // probablemente haya que sacarlo
			un_buffer = recibir_todo_el_buffer(fd_entradasalida_memoria);
			leerMemoriaFS(un_buffer);
			break;

		case LEER_EN_MEMORIA_UN_STRING:
			un_buffer = recibir_todo_el_buffer(fd_entradasalida_memoria);
			leerMemoriaUnString_io(un_buffer);

			break;
		case LEER_MEMORIA_UN_STRING_FS:
		    un_buffer = recibir_todo_el_buffer(fd_entradasalida_memoria);
			leerMemoriaUnString_fs(un_buffer);
			break;
		case -1:
			printf("Desconexion de KERNEL - IO");
			control_key = 0;
			break;
		default:
			log_warning(logger, "Operacion desconocida de KERNEL - IO");
			break;
		}
	}
}