#include "utils/shared.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <../include/IO.h>
#include <../include/inicializar_io.h>
#include <readline/readline.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int bitsToBytes(int bits)
{
	int bytes;
	if (bits < 8)
		bytes = 1;
	else
	{
		double c = (double)bits;
		bytes = ceil(c / 8.0);
	}

	return bytes;
}

char *asignarMemoriaBits(int bits) // recibe bits asigna bytes
{
	char *aux;
	int bytes;
	bytes = bitsToBytes(bits);
	// printf("BYTES: %d\n", bytes);
	aux = malloc(bytes);
	memset(aux, 0, bytes);
	return aux;
}
t_config *crearConfig(char *direccion)
{

	t_config *configDevolver = config_create(direccion); // esto te pide la ruta del config

	if (configDevolver == NULL)
	{
		perror("Hay un error al iniciar el config.");
		exit(EXIT_FAILURE);
	}

	return configDevolver;
}

void ejecutarInterfazGenerica(char *nombre, t_config *config_interface)
{
	log_info(io_logger, "Iniciando interfaz Generica");
	int TIEMPO_UNIDAD_TRABAJOO = config_get_int_value(config_interface, "TIEMPO_UNIDAD_TRABAJO"); // por default es 250, osea, 4 unidades de tiempo equivalen a un 1 segundo
	int Unidades_De_Trabajo = 4;

	log_info(io_logger, "Iniciando sleep");
	usleep(Unidades_De_Trabajo * (TIEMPO_UNIDAD_TRABAJOO * 1000)); // esta funcion anda con microsegundos, por eso se multiplican por 1000, asi cada unidad de trabajo equivale a un milisegundo
	log_info(io_logger, "Fin de sleep");

	log_trace(io_log_debug, "Fin de Interfaz Generica");
}

void escribirEnMemoria(int direccionLogica, char *texto, int tamanio)
{

	t_buffer *buffer = crear_buffer();
	buffer->size = 0;
	buffer->stream = NULL;

	cargar_int_al_buffer(buffer, direccionLogica);
	cargar_int_al_buffer(buffer, tamanio);
	cargar_string_al_buffer(buffer, texto);

	t_paquete *paquete = crear_super_paquete(ESCRIBIR_IO_MEMORIA, buffer);
	enviar_paquete(paquete, fd_memoria);
	destruir_paquete(paquete);
}

void ejecutarInterfazSTDIN(char *nombre, t_config *config_interface)
{
	log_info(io_logger, "Iniciando interfaz STDIN");

	char *textoAEscribir;
	log_info(io_logger, "Escriba el texto que desea guardar");
	textoAEscribir = readline(">");

	log_info(io_logger, "El texto a guardar es %s", textoAEscribir);

	free(textoAEscribir);
}

void ejecutarInterfazSTDOUT(char *nombre, t_config *config_interface)
{

	log_info(io_logger, "Iniciando interfaz STDOUT");

	char *textoLeido;

	textoLeido = "Texto de prueba";

	log_info(io_logger, "El texto leido es %s", textoLeido);
}

// Inicio DialFS

void crearArchivosInicialesFS(t_config *config_interface)
{

	char *PATH_Creacion = config_get_string_value(config_interface, "PATH_BASE_DIALFS");

	char *PATH_Bloques = string_duplicate(PATH_Creacion);
	char *PATH_Bitmap = string_duplicate(PATH_Creacion);

	char *direccionArchivoBloques = string_from_format("/ %s", "bloques.dat");
	char *direccionArchivoBitmap = string_from_format("/ %s", "bitmap.dat");

	string_append(&PATH_Bloques, direccionArchivoBloques);
	string_append(&PATH_Bitmap, direccionArchivoBitmap);

	FILE *archivoBloques = fopen(PATH_Bloques, "w");
	FILE *archivoBitmap = fopen(PATH_Bitmap, "w");

	int fd_BLoques = fileno(archivoBloques);

	int Tamnio_BLoque = config_get_int_value(config_interface, "BLOCK_SIZE");

	int Cantidad_BLoque = config_get_int_value(config_interface, "BLOCK_COUNT");

	ftruncate(fd_BLoques, Tamnio_BLoque * Cantidad_BLoque);

	fclose(archivoBloques);

	fclose(archivoBitmap);
}

void ejecutarInterfazDIALFS(char *nombre, t_config *config_interface)
{

	log_info(io_logger, "Iniciando interfaz DIALFS");

	char *PATH_FS = config_get_string_value(config_interface, "PATH_BASE_DIALFS");

	crearArchivosInicialesFS(config_interface);

	int cant_bloques = config_get_int_value(config_interface, "BLOCK_COUNT");

	char *data = asignarMemoriaBits(cant_bloques);

	if (data == NULL)
	{
		printf("MALLOC FAIL!\n");
	}

	memset(data, 0, cant_bloques / 8);

	bitarray_bloques = bitarray_create_with_mode(data, cant_bloques / 8, MSB_FIRST);

	char *comandoEjecutar = "IO_FS_CREATE";

	if (strcmp(comandoEjecutar, "IO_FS_CREATE") == 0)
	{

		log_info(io_logger, "La instruccion a ejecutar es IO_FS_CREATE");

		char *nombre_Archivo = "pruebaFS.txt";

		crearArchivo(nombre_Archivo, config_interface, bitarray_bloques);
	}
	else if (strcmp(comandoEjecutar, "IO_FS_DELETE") == 0)
	{

		log_info(io_logger, "La instruccion a ejecutar es IO_FS_DELETE");
		log_error(io_log_debug, "El comando no se encuntra implementado");
	}
	else if (strcmp(comandoEjecutar, "IO_FS_TRUNCATE") == 0)
	{

		log_info(io_logger, "La instruccion a ejecutar es IO_FS_TRUNCATE");
		log_error(io_log_debug, "El comando no se encuntra implementado");
	}
	else if (strcmp(comandoEjecutar, "IO_FS_WRITE") == 0)
	{

		log_info(io_logger, "La instruccion a ejecutar es IO_FS_WRITE");
		log_error(io_log_debug, "El comando no se encuntra implementado");
	}
	else if (strcmp(comandoEjecutar, "IO_FS_READ") == 0)
	{

		log_info(io_logger, "La instruccion a ejecutar es IO_FS_READ");
		log_error(io_log_debug, "El comando no se encuntra implementado");
	}
	else
	{

		log_error(io_log_debug, "No se pudo identificar correctamente la instruccion a ejecutar");
	}
}

// Fin DialFS

void iniciarInterfaz(char *nombre_Interface, char *direccion_Config)
{
	log_info(io_logger, "Incializando interfaz");

	t_config *config_interface = crearConfig(direccion_Config);

	char *TIPO_INTERFAZZ = config_get_string_value(config_interface, "TIPO_INTERFAZ");
	log_info(io_logger, "%s", TIPO_INTERFAZZ);

	if (strcmp(TIPO_INTERFAZZ, "GENERICA") == 0)
	{

		log_info(io_logger, "La interfaz a iniciar es del tipo Generica");
		ejecutarInterfazGenerica(nombre_Interface, config_interface);
	}
	else if (strcmp(TIPO_INTERFAZZ, "STDIN") == 0)
	{

		log_info(io_logger, "La interfaz a iniciar es del tipo STDIN");
		ejecutarInterfazSTDIN(nombre_Interface, config_interface);
	}
	else if (strcmp(TIPO_INTERFAZZ, "STDOUT") == 0)
	{

		log_info(io_logger, "La interfaz a iniciar es del tipo STDOUT");
		ejecutarInterfazSTDOUT(nombre_Interface, config_interface);
	}
	else if (strcmp(TIPO_INTERFAZZ, "DIALFS") == 0)
	{

		log_info(io_logger, "La interfaz a iniciar es del tipo DialFS");
		ejecutarInterfazDIALFS(nombre_Interface, config_interface);
	}
	else
	{

		log_error(io_log_debug, "No se pudo identificar correctamente el tipo de interfaz");
	}
}

void crearInterfaz(char *nombre_Interfaz, char *direccion_Config)
{
	t_config *io_config = crearConfig(direccion_Config);

	IP_MEMORIA = config_get_string_value(io_config, "IP_MEMORIA");
	TIPO_INTERFAZ = config_get_string_value(io_config, "TIPO_INTERFAZ");
	PUERTO_MEMORIA = config_get_string_value(io_config, "PUERTO_MEMORIA");
	IP_KERNEL = config_get_string_value(io_config, "IP_KERNEL");
	PUERTO_KERNEL = config_get_string_value(io_config, "PUERTO_KERNEL");
	TIEMPO_UNIDAD_TRABAJO = config_get_int_value(io_config, "TIEMPO_UNIDAD_TRABAJO");
	PATH_BASE_DIALFS = config_get_string_value(io_config, "PATH_BASE_DIALFS");
	BLOCK_SIZE = config_get_int_value(io_config, "BLOCK_SIZE");
	BLOCK_COUNT = config_get_int_value(io_config, "BLOCK_COUNT");
	RETRASO_COMPACTACION = config_get_int_value(io_config, "RETRASO_COMPACTACION");

	printf("se creo una interfaz de tipo %s\n", TIPO_INTERFAZ);

	log_info(io_logger, "Creando conexión con Kernel");
	fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
	log_info(io_logger, "Conexion con kernel exitosa!");

	log_info(io_logger, "Creando conexión con Memoria");
	fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
	log_info(io_logger, "Conexion con memoria exitosa!");

	//////////////////////////////////////////////////////////

	t_buffer *a_enviar = crear_buffer();

    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_string_al_buffer(a_enviar, nombre_Interfaz);
	cargar_string_al_buffer(a_enviar,TIPO_INTERFAZ);

    t_paquete *un_paquete = crear_super_paquete(CREAR_INTERFAZ, a_enviar);
    enviar_paquete(un_paquete, fd_kernel);
    destruir_paquete(un_paquete);

	printf("mande un buffer a kernel\n");

	////////////////////////////////////////////////////////////

	t_buffer *a_enviar2 = crear_buffer();

    a_enviar2->size = 0;
    a_enviar2->stream = NULL;

    cargar_string_al_buffer(a_enviar2, nombre_Interfaz);
	cargar_string_al_buffer(a_enviar2,TIPO_INTERFAZ);

    t_paquete *un_paquete2 = crear_super_paquete(CREAR_INTERFAZ, a_enviar2);
    enviar_paquete(un_paquete2, fd_memoria);
    destruir_paquete(un_paquete2);

	printf("mande un buffer a memoria\n");

	///////////////////////////////////////////////////////////

	pthread_t hilo_generica; 
	pthread_create(&hilo_generica, NULL, (void *)atender_interfaz_kernel, &fd_kernel);
	pthread_detach(hilo_generica);

	pthread_t hilo_generica2; // planificador largo plazo
	pthread_create(&hilo_generica2, NULL, (void *)atender_interfaz_memoria, &fd_memoria);
	pthread_join(hilo_generica2,NULL);

}

int main()
{
	inicializar_io();

	log_info(io_logger, "Inicializando Entrada/Salida");

	do
	{
		log_info(io_logger, "Escriba el nombre de la interfaz");
		nombreInterACrear = readline(">");

		if (string_is_empty(nombreInterACrear))
		{
			log_warning(io_log_debug, "Una interfaz no puede tener el nombre vacio");
		}
	} while (string_is_empty(nombreInterACrear));

	log_info(io_logger, "El nombre elegido es %s", nombreInterACrear);

	do
	{
		log_info(io_logger, "Escriba la direccion del archivo de configuracion");
		direccionConfigInterCrear = readline(">");

		if (string_is_empty(direccionConfigInterCrear))
		{
			log_warning(io_log_debug, "Una direccion no puede ser vacia");
		}

	} while (string_is_empty(direccionConfigInterCrear));

	//direccionConfigInterCrear = "/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/entradasalida/entradasalida.config"; // hardcodeo el path del config para hacer pruebas
	log_info(io_logger, "La direccion elegida es %s", direccionConfigInterCrear);											  // TODO: ¿Se deberia considerar la posibilidad de que en esa direccion no haya archivo de configuracion?

	crearInterfaz(nombreInterACrear, direccionConfigInterCrear);
	//iniciarInterfaz(nombreInterACrear, direccionConfigInterCrear);
	// free(direccionConfigInterCrear);
	free(nombreInterACrear);

	log_info(io_logger, "Fin de Entrada/Salida");

	return 0;
}