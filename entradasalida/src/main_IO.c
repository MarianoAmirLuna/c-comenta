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
#include <../include/bitarray_mmap.h>

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

int bytesABloques(int bytes,int tamanioBloques){

	if(bytes<tamanioBloques){
		return 1;
	}
	else{
		int cantBloques = (int)ceil((double) bytes / (double) tamanioBloques);
		return cantBloques;
	}
	
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

int setearPrimerBitDisponible(t_bitarray *bloq_dis) // setea en 1 el primer bit en 0 que encuentre y devuelve la posicion donde hizo el cambio
{

	for (int base = 0; base < bitarray_get_max_bit(bloq_dis); base++)
	{
		if (bitarray_test_bit(bloq_dis, base) == 0)
		{
			bitarray_set_bit(bloq_dis, base);
			return base;
		}
	}
}

void escribirCentinelaBLoquesDesdeHasta(char* PATH_bloques, int primerBloque,int cantBLoques,int tamanioBLoques, char caracterCentinela){
	

	int fd = open(PATH_bloques, O_RDWR);
	if (fd == -1)
	{
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}

	// Obtener el tamaño del archivo
	struct stat sb;
	if (fstat(fd, &sb) == -1)
	{
		perror("Error al obtener el tamaño del archivo");
		close(fd);
		exit(EXIT_FAILURE);
	}

	// Mapear el archivo en memoria
	char *map = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED)
	{
		perror("Error al mapear el archivo");
		close(fd);
		exit(EXIT_FAILURE);
	}

	// Escribir '\0' desde el bloque primerBloque (inclusive) hasta el bloque cantBLoques (no inclusive)
	for(int i = primerBloque * tamanioBLoques; i<cantBLoques * tamanioBLoques; i++){
		map[i] = caracterCentinela;
	}

	// Sincronizar los cambios
	if (msync(map, sb.st_size, MS_SYNC) == -1)
	{
		perror("Error al sincronizar los cambios");
	}

	// Desmapear el archivo y cerrar el descriptor
	if (munmap(map, sb.st_size) == -1)
	{
		perror("Error al desmapear el archivo");
	}
	close(fd);

}

void escribirCentinelaInicialBLoques(char *PATH_FS, int numPagina,int tamanioBLoque, char escribir)
{

	char *PATH_bloques = string_duplicate(PATH_FS);
	string_append(&PATH_bloques, "/ bloques.dat");
	/*
		numPagina=0;
		escribir='/0';
	*/
	int fd = open(PATH_bloques, O_RDWR);
	if (fd == -1)
	{
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}

	// Obtener el tamaño del archivo
	struct stat sb;
	if (fstat(fd, &sb) == -1)
	{
		perror("Error al obtener el tamaño del archivo");
		close(fd);
		exit(EXIT_FAILURE);
	}
	/*
		// Asegurarse de que el archivo tenga al menos 16 bytes
		if (sb.st_size < 16) {
			if (ftruncate(fd, 16) == -1) {
				perror("Error al redimensionar el archivo");
				close(fd);
				exit(EXIT_FAILURE);
			}
		}
	*/
	// Mapear el archivo en memoria
	char *map = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED)
	{
		perror("Error al mapear el archivo");
		close(fd);
		exit(EXIT_FAILURE);
	}

	// Escribir '\0' en los primeros tamanioBLoque bytes
	for (int i = (numPagina) * tamanioBLoque; i < (numPagina + 1) * tamanioBLoque; i++)
	{
		map[i] = escribir;
	}

	// Sincronizar los cambios
	if (msync(map, sb.st_size, MS_SYNC) == -1)
	{
		perror("Error al sincronizar los cambios");
	}

	// Desmapear el archivo y cerrar el descriptor
	if (munmap(map, sb.st_size) == -1)
	{
		perror("Error al desmapear el archivo");
	}
	close(fd);
}

int hayBloquesOcupados(t_bitarray* bitarray_revisar,int bloqueInicial ,int cantBLoques){ //devuelve 1 si hay bloques ocupados a la hora de truncar (se debe compactar), sino devuelve 0
	for(int i = bloqueInicial; i<cantBLoques;i++){
		if(bitarray_test_bit(bitarray_revisar,i) == 1){
			return 1;
		}
	}
	return 0;
}

void compactar(char* PATH_FS,char* nombre_ArchivoCompactar){

}

void truncarArchivo(char *nombre_Archivo, t_config *config_interface, int tamanioTruncar){

	log_info(io_logger, "Iniciando truncar del archivo %s", nombre_Archivo);

	char *PATH_FS = config_get_string_value(config_interface, "PATH_BASE_DIALFS");
	char *PATH_metadata = string_duplicate(PATH_FS);

	char *direccionMetadata = string_from_format("/ %s", nombre_Archivo);

	string_append(&PATH_metadata, direccionMetadata);

	t_config *connfig_metadatos = config_create(PATH_metadata);

	char *PATH_bitmap = string_duplicate(PATH_FS);
	char *direccionBitmap = string_from_format("/ %s", "bitmap.dat");
	string_append(&PATH_bitmap, direccionBitmap);

	t_bitarray *mapped_bitarray = bitarray_mmap_from_file(PATH_bitmap, MSB_FIRST);
	if (mapped_bitarray == NULL)
	{
		fprintf(stderr, "Error al mapear el archivo\n");
		return;
	}


	int tamanioBloques = config_get_int_value(config_interface,"BLOCK_SIZE");

	int bloquesOffsetFinal = bytesABloques(tamanioTruncar, tamanioBloques);

	int bloqueInicialArchivo = config_get_int_value(connfig_metadatos,"BLOQUE_INICIAL");

	int tamnioInicialArchivo = config_get_int_value(connfig_metadatos,"TAMANIO_ARCHIVO");

	int bloquesOffsetInicial = bytesABloques(tamnioInicialArchivo, tamanioBloques);

	char *PATH_bloques = string_duplicate(PATH_FS);
	string_append(&PATH_bloques, "/ bloques.dat");

	if(hayBloquesOcupados(mapped_bitarray,bloqueInicialArchivo,bloquesOffsetFinal)==1){
		//compactar
		bitarray_munmap(mapped_bitarray);


	}else{
		//trunco sin problema
		config_set_value(connfig_metadatos,"TAMANIO_ARCHIVO",tamanioTruncar);
		config_save(connfig_metadatos);

		for(int i = bloquesOffsetInicial; i < bloquesOffsetFinal; i++){
			bitarray_set_bit(mapped_bitarray,i);
		}
		escribirCentinelaBLoquesDesdeHasta(PATH_bloques,bloquesOffsetInicial,bloquesOffsetFinal,tamanioBloques,'\0');
		bitarray_munmap(mapped_bitarray);

	}

}

void eliminarArchivo(char *nombre_Archivo, t_config *config_interface)
{
	log_info(io_logger, "Iniciando borrado del archivo %s", nombre_Archivo);

	char *PATH_FS = config_get_string_value(config_interface, "PATH_BASE_DIALFS");
	char *PATH_metadata = string_duplicate(PATH_FS);

	char *direccionMetadata = string_from_format("/ %s", nombre_Archivo);

	string_append(&PATH_metadata, direccionMetadata);

	t_config *connfig_metadatos = config_create(PATH_metadata);

	char *PATH_bitmap = string_duplicate(PATH_FS);
	char *direccionBitmap = string_from_format("/ %s", "bitmap.dat");
	string_append(&PATH_bitmap, direccionBitmap);

	t_bitarray *mapped_bitarray = bitarray_mmap_from_file(PATH_bitmap, MSB_FIRST);
	if (mapped_bitarray == NULL)
	{
		fprintf(stderr, "Error al mapear el archivo\n");
		return;
	}

	int bloqueInicial = config_get_int_value(connfig_metadatos, "BLOQUE_INICIAL");
	int tamnioArchivo = config_get_int_value(connfig_metadatos, "TAMANIO_ARCHIVO");

	log_info(io_logger, "Liberando bloques bitmap");
	if (tamnioArchivo == 0)
	{
		bitarray_clean_bit(mapped_bitarray, bloqueInicial);
	}
	else
	{
		int tamanioBloque = config_get_int_value(config_interface, "BLOCK_SIZE");

		int bloquesOffset = (int)ceil((double)tamnioArchivo / (double)tamanioBloque);

		for (int i = bloqueInicial; i < bloquesOffset; i++)
		{
			bitarray_clean_bit(mapped_bitarray, i);
		}
	}

	bitarray_munmap(mapped_bitarray);
	log_info(io_logger, "BLoques del bitmap liberados");

	log_info(io_logger, "Eliminando metadata");
	if (remove(PATH_metadata) == 0)
	{
		log_info(io_logger, "El archivo %s de matadata ha sido eliminado exitosamente.\n", nombre_Archivo);
	}
	else
	{

		perror("Error al eliminar el archivo");
	}

	log_info(io_logger, "Fin del proceso de borrado");
}

void crearArchivo(char *nombre_Archivo, t_config *config_interface)
{

	log_info(io_logger, "Iniciando creacion de archivo de Metadata %s", nombre_Archivo);

	char *PATH_FS = config_get_string_value(config_interface, "PATH_BASE_DIALFS");

	char *PATH_metadata = string_duplicate(PATH_FS);

	char *direccionMetadata = string_from_format("/ %s", nombre_Archivo);

	string_append(&PATH_metadata, direccionMetadata);

	FILE *archivoACrear = fopen(PATH_metadata, "w"); // creo el archivo de metadatos

	char *PATH_bitmap = string_duplicate(PATH_FS);
	char *direccionBitmap = string_from_format("/ %s", "bitmap.dat");
	string_append(&PATH_bitmap, direccionBitmap);

	t_bitarray *mapped_bitarray = bitarray_mmap_from_file(PATH_bitmap, MSB_FIRST); // Busco algun bloque libre y lo establezco como ocupado en el bitmap.dat
	if (mapped_bitarray == NULL)
	{
		fprintf(stderr, "Error al mapear el archivo\n");
		return;
	}

	int bloqueInicial = setearPrimerBitDisponible(mapped_bitarray);

	bitarray_munmap(mapped_bitarray);

	fprintf(archivoACrear, "BLOQUE_INICIAL=\nTAMANIO_ARCHIVO=\n");
	fclose(archivoACrear);

	t_config *config_metadata = config_create(PATH_metadata);

	config_set_value(config_metadata, "BLOQUE_INICIAL", string_itoa(bloqueInicial));
	config_set_value(config_metadata, "TAMANIO_ARCHIVO", "0");
	config_save(config_metadata);
	config_destroy(config_metadata);

	// printf("voy a escribir los bloques\n");
	// t_config* nuevoConfig = config_create("/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/entradasalida/entradasalida.config");
	int tamanioBloque = config_get_int_value(config_interface,"BLOCK_SIZE");
	escribirCentinelaInicialBLoques(PATH_FS, bloqueInicial, tamanioBloque, '\0');

	log_info(io_logger, "Fin creacion de archivo de metadata");
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

	FILE *archivoBloques = fopen(PATH_Bloques, "wb");
	FILE *archivoBitmap = fopen(PATH_Bitmap, "wb");

	// Inicio establecer tamaño maximo archivo bloques.dat
	int fd_BLoques = fileno(archivoBloques);

	int Tamanio_BLoque = config_get_int_value(config_interface, "BLOCK_SIZE");

	int Cantidad_BLoques = config_get_int_value(config_interface, "BLOCK_COUNT");

	ftruncate(fd_BLoques, Tamanio_BLoque * Cantidad_BLoques);
	// Fin establecer tamaño maximo archivo bloques.dat

	// Inicio bitmap.dat

	char *data = asignarMemoriaBits(Cantidad_BLoques);

	if (data == NULL)
	{
		printf("MALLOC FAIL!\n");
	}

	int tamanioBitArray = bitsToBytes(Cantidad_BLoques);

	memset(data, 0, tamanioBitArray);

	t_bitarray *bitarray_inicial = bitarray_create_with_mode(data, tamanioBitArray, MSB_FIRST);
	if (bitarray_inicial == NULL)
	{
		fprintf(stderr, "Error al crear el bit array\n");
		free(data);
		return;
	}

	if (bitarray_write_to_file(bitarray_inicial, PATH_Bitmap) == -1)
	{
		fprintf(stderr, "Error al escribir el bit array en el archivo\n");
		bitarray_destroy(bitarray_inicial);
		free(data);
		return;
	}

	// printf("variable Cantidad_BLoques: %i\n",Cantidad_BLoques);
	// printf("Tamanio del bit array guardado: %i\n",bitarray_get_max_bit(bitarray_inicial));
	bitarray_destroy(bitarray_inicial);
	free(data);
	// Fin bitmap.dat

	fclose(archivoBloques);
	fclose(archivoBitmap);
}

void ejecutarInterfazDIALFS(char *nombre, t_config *config_interface)
{

	log_info(io_logger, "Iniciando interfaz DIALFS");

	/*
	char* PATH_FS = config_get_string_value(config_interface,"PATH_BASE_DIALFS");
	char* PATH_bitmap = string_duplicate(PATH_FS);
	string_append(&PATH_bitmap, "/ bitmap.dat");

	struct stat buffer;
	if (stat(PATH_bitmap, &buffer)!=0) { //me fijo si el file system ya esta creado, esto significaria que la interfaz se desconecto y se volvio a conectar
	*/

	log_info(io_logger, "Generando archivos bloques.dat y bitmap.dat");
	crearArchivosInicialesFS(config_interface);

	/*
	}else{
		log_info(io_logger, "La interfaz se ha reconectado");
	}
	*/

	char *comandoEjecutar = "IO_FS_CREATE";

	if (strcmp(comandoEjecutar, "IO_FS_CREATE") == 0)
	{

		log_info(io_logger, "La instruccion a ejecutar es IO_FS_CREATE");

		char *nombre_Archivo = "pruebaFS.txt";

		crearArchivo(nombre_Archivo, config_interface);

		// nombre_Archivo = "segundo.txt";

		// crearArchivo(nombre_Archivo, config_interface);
	}
	else if (strcmp(comandoEjecutar, "IO_FS_DELETE") == 0)
	{

		log_info(io_logger, "La instruccion a ejecutar es IO_FS_DELETE");

		char *nombre_Archivo = "pruebaFS.txt";

		eliminarArchivo(nombre_Archivo, config_interface);
	}
	else if (strcmp(comandoEjecutar, "IO_FS_TRUNCATE") == 0)
	{

		log_info(io_logger, "La instruccion a ejecutar es IO_FS_TRUNCATE");

		char *nombre_Archivo = "pruebaFS.txt";

		int tamanioTruncar = 32;
		
		truncarArchivo(nombre_Archivo, config_interface, tamnioTruncar);
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

void crearInterfaz(char *nombre_Interfaz, char *direccion_Config)
{

	t_config *config_interface = crearConfig(direccion_Config);

	char *IP_KERNEL = config_get_string_value(config_interface, "IP_KERNEL");
	char *PUERTO_KERNEL = config_get_string_value(config_interface, "PUERTO_KERNEL");
	char *IP_MEMORIA = config_get_string_value(config_interface, "IP_MEMORIA");
	char *PUERTO_MEMORIA = config_get_string_value(config_interface, "PUERTO_MEMORIA");
	char *TIPO_INTERFAZ = config_get_string_value(config_interface, "TIPO_INTERFAZ");

	// CREA CONEXIONES A LOS MODULOS KERNEL Y MEMORIA
	if (strcmp(TIPO_INTERFAZ, "GENERICA") == 0)
	{
		// CONEXION CON KERNEL
		printf("se creo una generica\n");
		log_info(io_logger, "Creando conexión con Kernel");
		fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL); // puerto kernel = 49152 49152
		log_info(io_logger, "Conexion con kernel exitosa!");

		// printf("pase el sleep 3 voy a mandar un buffer a kernel\n");
		// sleep(3);
		// si se genera un problema de condicion de carrera ver de poner el sleep o un semaforo

		t_buffer *buffer = crear_buffer();
		buffer->size = 0;
		buffer->stream = NULL;

		cargar_string_al_buffer(buffer, nombre_Interfaz);
		cargar_string_al_buffer(buffer, TIPO_INTERFAZ);

		t_paquete *paquete = crear_super_paquete(CREAR_INTERFAZ, buffer);
		enviar_paquete(paquete, fd_kernel);
		destruir_paquete(paquete);

		printf("se envio el paquete para crear la interfaz a kernel\n");
	}
	else if (strcmp(TIPO_INTERFAZ, "STDIN") == 0)
	{
		// CONEXION CON KERNEL
		printf("se creo una stdin\n");
		log_info(io_logger, "Creando conexión con Kernel");
		fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
		log_info(io_logger, "Conexion con kernel exitosa!");

		// CONEXION CON MEMORIA
		log_info(io_logger, "Creando conexión con Memoria");
		fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
		log_info(io_logger, "Conexion con memoria exitosa!");
	}
	else if (strcmp(TIPO_INTERFAZ, "STDOUT") == 0)
	{
		// CONEXION CON KERNEL
		printf("se creo una stdout\n");
		log_info(io_logger, "Creando conexión con Kernel");
		fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
		log_info(io_logger, "Conexion con kernel exitosa!");
		// CONEXION CON MEMORIA
		log_info(io_logger, "Creando conexión con Memoria");
		fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
		log_info(io_logger, "Conexion con memoria exitosa!");
	}
	else if (strcmp(TIPO_INTERFAZ, "DIALFS") == 0)
	{
		// CONEXION CON KERNEL
		printf("se creo una dialfs\n");
		log_info(io_logger, "Creando conexión con Kernel");
		fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
		log_info(io_logger, "Conexion con kernel exitosa!");
		// CONEXION CON MEMORIA
		log_info(io_logger, "Creando conexión con Memoria");
		fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
		log_info(io_logger, "Conexion con memoria exitosa!");
	}
}

int main()
{
	inicializar_io();

	log_info(io_logger, "Inicializando Entrada/Salida");

	char *nombreInterACrear;
	char *direccionConfigInterCrear;

	// while (1)
	//{
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

	direccionConfigInterCrear = "/home/utnso/Desktop/ClonOperativos/tp-2024-1c-Granizado/entradasalida/entradasalida.config"; // hardcodeo el path del config para hacer pruebas
	log_info(io_logger, "La direccion elegida es %s\n", direccionConfigInterCrear);											  // TODO: ¿Se deberia considerar la posibilidad de que en esa direccion no haya archivo de configuracion?
																															  /*
																																	  t_config* config_Prueba = config_create(direccionConfigInterCrear);
																														  
																																	  int cantDeBloques = config_get_int_value(config_Prueba,"BLOCK_COUNT");
																																	  int tamanioBitmap = (int) ceil(cantDeBloques/8.0);
																															  */
	// crearInterfaz(nombreInterACrear, direccionConfigInterCrear);
	iniciarInterfaz(nombreInterACrear, direccionConfigInterCrear);
	// free(direccionConfigInterCrear);
	// free(nombreInterACrear);

	log_info(io_logger, "Fin de Entrada/Salida");

	return 0;
}