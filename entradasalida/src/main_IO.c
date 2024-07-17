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

void setearPrimerBitDisponible(t_bitarray* bloq_dis){

	for(int base=0;base<bitarray_get_max_bit(bloq_dis);base++){
		if(bitarray_test_bit(bloq_dis, base) == 0){
			bitarray_set_bit(bloq_dis, base);
			return;
		}
	}
}


void crearArchivo(char *nombre_Archivo, t_config *config_interface,t_bitarray* bloq_dis )
{

	log_info(io_logger, "Iniciando creacion de archivo");

	char *PATH_FS = config_get_string_value(config_interface, "PATH_BASE_DIALFS");

	char *direccionArchivoCrear = string_from_format("/ %s", nombre_Archivo);

	string_append(&PATH_FS, direccionArchivoCrear);

	FILE *archivoACrear = fopen(PATH_FS, "w"); //creo el archivo de metadatos

	fclose(archivoACrear);

	bool bit = bitarray_test_bit(bloq_dis, 0);
    printf("El valor del bit antes de setear es: %d\n", bit);

	setearPrimerBitDisponible(bloq_dis);

	 bit = bitarray_test_bit(bloq_dis, 0);
    printf("El valor del bit despues de setear es: %d\n", bit);
	

	log_info(io_logger, "Fin creacion de archivo");
}

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

	int cant_bloques = config_get_int_value(config_interface,"BLOCK_COUNT");

	char *data = asignarMemoriaBits(cant_bloques);

	if (data == NULL)
	{

		printf("MALLOC FAIL!\n");
	}

	memset(data, 0, cant_bloques / 8);

	t_bitarray* bloq_dis = bitarray_create_with_mode(data, cant_bloques / 8, MSB_FIRST);

	char *comandoEjecutar = "IO_FS_CREATE";

	if (strcmp(comandoEjecutar, "IO_FS_CREATE") == 0)
	{

		log_info(io_logger, "La instruccion a ejecutar es IO_FS_CREATE");

		char *nombre_Archivo = "pruebaFS.txt";

		crearArchivo(nombre_Archivo, config_interface, bloq_dis);
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

int main()
{

	inicializar_io();

	log_info(io_logger, "Inicializando Entrada/Salida");

	/*
	fd_kernel = iniciar_conexion(PUERTO_KERNEL, "KERNEL",io_log_debug);

	fd_memoria = iniciar_conexion(PUERTO_MEMORIA, "MEMORIA",io_log_debug);
	*/

	/*pthread_t hilo_kernel;
	pthread_create(&hilo_kernel, NULL, (void*)atender_io_kernel, NULL);
	pthread_detach(hilo_kernel);

	pthread_t hilo_memoria;
	pthread_create(&hilo_memoria, NULL, (void*)atender_io_memoria, NULL);
	pthread_join(hilo_memoria, NULL);*/

	char *nombreInterACrear;

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

	char *direccionConfigInterCrear;

	do
	{

		log_info(io_logger, "Escriba la direccion del archivo de configuracion");
		direccionConfigInterCrear = readline(">");

		if (string_is_empty(direccionConfigInterCrear))
		{
			log_warning(io_log_debug, "Una direccion no puede ser vacia");
		}

	} while (string_is_empty(direccionConfigInterCrear));

	log_info(io_logger, "La direccion elegida es %s", nombreInterACrear); // TODO: ¿Se deberia considerar la posibilidad de que en esa direccion no haya archivo de configuracion?

	free(direccionConfigInterCrear);

	direccionConfigInterCrear = "/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/entradasalida/entradasalida.config"; // TODO: borrar este hardcodeo de la direccion del config

	iniciarInterfaz(direccionConfigInterCrear, direccionConfigInterCrear);

	// free(direccionConfigInterCrear);
	free(nombreInterACrear);

	log_info(io_logger, "Fin de Entrada/Salida");

	return 0;
}