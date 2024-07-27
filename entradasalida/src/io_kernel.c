#include "../include/io_kernel.h"
#include <utils/shared.h>
#include <readline/readline.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <../include/bitarray_mmap.h>
#include <dirent.h>

int bytesABloques(int bytes, int tamanioBloques)
{

	if (bytes < tamanioBloques)
	{
		return 1;
	}
	else
	{
		int cantBloques = (int)ceil((double)bytes / (double)tamanioBloques);
		return cantBloques;
	}
}

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
	return -1;
}

void escribirCentinelaBLoquesDesdeHasta(char *PATH_bloques, int primerBloque, int cantBLoques, int tamanioBLoques, char caracterCentinela)
{

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
	// primerBLoque=5, cantbloques=4, tamanioBLoque=16, i= 5* 16 =80 (posicion del primer byte del bloque 5),
	//  Escribir '\0' desde el bloque primerBloque (inclusive) hasta el bloque cantBLoques (no inclusive)
	for (int i = primerBloque * tamanioBLoques; i < cantBLoques * tamanioBLoques; i++)
	{
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

void escribirCentinelaInicialBLoques(char *PATH_FS, int numPagina, int tamanioBLoque, char escribir)
{

	char *PATH_bloques = string_duplicate(PATH_FS);
	string_append(&PATH_bloques, "/bloques.dat");
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
	for (int i = (numPagina)*tamanioBLoque; i < (numPagina + 1) * tamanioBLoque; i++)
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

int hayBloquesOcupados(t_bitarray *bitarray_revisar, int bloqueFinalInicial, int bloqueFinalOcupar)
{ // devuelve 1 si hay bloques ocupados a la hora de truncar (se debe compactar), sino devuelve 0
	for (int i = (bloqueFinalInicial + 1); i <= bloqueFinalOcupar; i++)
	{
		if (bitarray_test_bit(bitarray_revisar, i) == 1)
		{
			return 1;
		}
	}
	return 0;
}

typedef struct
{
	char *PATH;
	char *contenido;
	int bloque_inicial;
	int tamanio_en_bloques;
} colArchivo;

void guardar_en_bloque(const char *filename, const char *contenido, int bloque, int tamanio_bloque)
{
	// Abrir el archivo en modo binario para lectura/escritura
	FILE *file = fopen(filename, "r+b");
	if (file == NULL)
	{
		perror("No se pudo abrir el archivo");
		exit(EXIT_FAILURE);
	}

	// Calcular la posición donde se debe escribir
	int posicion = bloque * tamanio_bloque;

	// Mover el puntero del archivo a la posición deseada
	if (fseek(file, posicion, SEEK_SET) != 0)
	{
		perror("Error al mover el puntero del archivo");
		fclose(file);
		exit(EXIT_FAILURE);
	}

	// Escribir el contenido en la posición deseada
	size_t contenido_len = strlen(contenido);
	if (fwrite(contenido, sizeof(char), contenido_len, file) != contenido_len)
	{
		perror("Error al escribir en el archivo");
		fclose(file);
		exit(EXIT_FAILURE);
	}

	// Cerrar el archivo
	fclose(file);
}

char *leerArchivo(char *nombre_Archivo, int registro_puntero, int registro_tamanio)
{

	log_info(io_logger, "Iniciando lectura de archivo %s", nombre_Archivo);

	char *PATH_FS = PATH_BASE_DIALFS;

	char *PATH_bloques = string_duplicate(PATH_FS);
	string_append(&PATH_bloques, "/bloques.dat");

	char *PATH_metadata = string_duplicate(PATH_FS);
	char *direccionMetadata = string_from_format("/%s", nombre_Archivo);
	string_append(&PATH_metadata, direccionMetadata);

	t_config *config_metadata = config_create(PATH_metadata);
	printf("PATH_metadata: %s\n", PATH_metadata);

	int bloque_inicial = config_get_int_value(config_metadata, "BLOQUE_INICIAL");

	int tamanio_de_bloque = BLOCK_SIZE;

	FILE *archivo = fopen(PATH_bloques, "rb");
	if (archivo == NULL)
	{
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}

	// Calcular la posición desde donde se debe leer
	int posicion_lectura = (bloque_inicial * tamanio_de_bloque) + registro_puntero;

	// Mover el puntero del archivo a la posición de lectura calculada
	if (fseek(archivo, posicion_lectura, SEEK_SET) != 0)
	{
		perror("Error al posicionarse en el archivo");
		fclose(archivo);
		exit(EXIT_FAILURE);
	}

	// Leer los bytes indicados por registro_tamanio
	char *buffer = (char *)malloc(registro_tamanio + 1); // +1 para el terminador nulo
	if (buffer == NULL)
	{
		perror("Error al asignar memoria");
		fclose(archivo);
		exit(EXIT_FAILURE);
	}

	size_t leido = fread(buffer, sizeof(char), registro_tamanio, archivo);
	if (leido != registro_tamanio)
	{
		if (feof(archivo))
		{
			printf("Se alcanzó el final del archivo antes de leer la cantidad solicitada.\n");
		}
		else
		{
			perror("Error al leer del archivo");
			free(buffer);
			fclose(archivo);
			exit(EXIT_FAILURE);
		}
	}

	// Añadir terminador nulo al buffer para imprimirlo como cadena
	buffer[registro_tamanio] = '\0';

	// Mostrar el resultado leído en pantalla
	log_info(io_logger, "Datos leídos: %s\n", buffer);

	// Liberar memoria y cerrar el archivo

	fclose(archivo);
	config_destroy(config_metadata);

	log_info(io_logger, "Fin de lectura");

	return buffer;
}

void escribirArchivo(char *nombre_Archivo, int registro_puntero, char *texto_a_escribir)
{

	log_info(io_logger, "Iniciando escritura de archivo");
	log_info(io_logger, "Se va a escribir %s en el archivo %s", texto_a_escribir, nombre_Archivo);

	char *PATH_FS = PATH_BASE_DIALFS;

	char *PATH_bloques = string_duplicate(PATH_FS);
	string_append(&PATH_bloques, "/bloques.dat");

	char *PATH_metadata = string_duplicate(PATH_FS);
	char *direccionMetadata = string_from_format("/%s", nombre_Archivo);
	string_append(&PATH_metadata, direccionMetadata);

	t_config *config_metadata = config_create(PATH_metadata);
	printf("PATH_metadata: %s\n", PATH_metadata);

	int bloque_inicial = config_get_int_value(config_metadata, "BLOQUE_INICIAL");

	int tamanio_de_bloque = BLOCK_SIZE;

	// Abrir el archivo en modo lectura/escritura
	FILE *archivo = fopen(PATH_bloques, "r+b");
	if (archivo == NULL)
	{
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}

	// Calcular la posición desde donde se debe escribir
	int posicion_escritura = (bloque_inicial * tamanio_de_bloque) + registro_puntero;

	// Mover el puntero del archivo a la posición de escritura calculada
	if (fseek(archivo, posicion_escritura, SEEK_SET) != 0)
	{
		perror("Error al posicionarse en el archivo");
		fclose(archivo);
		exit(EXIT_FAILURE);
	}

	// Escribir el texto en la posición calculada
	size_t texto_len = strlen(texto_a_escribir);
	size_t escrito = fwrite(texto_a_escribir, sizeof(char), texto_len, archivo);
	if (escrito != texto_len)
	{
		perror("Error al escribir en el archivo");
		fclose(archivo);
		exit(EXIT_FAILURE);
	}

	// Cerrar el archivo
	fclose(archivo);
	config_destroy(config_metadata);
	log_info(io_logger, "Fin de escritura");
}

void compactar(char *PATH_FS, char *nombre_ArchivoCompactar, int tamanioTruncar, int pid)
{
	log_info(io_logger, "PID: %i - Inicio compactacion", pid);

	char *PATH_bloques = string_duplicate(PATH_FS);
	string_append(&PATH_bloques, "/bloques.dat");

	int tamanioBloque = BLOCK_SIZE;

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

	t_queue *colaArchivos = queue_create();

	DIR *dir;
	struct dirent *entry;

	// Abrir el directorio
	if ((dir = opendir(PATH_FS)) == NULL)
	{
		perror("opendir");
		return;
	}

	// Leer los archivos en el directorio
	while ((entry = readdir(dir)) != NULL)
	{

		// Saltar ".", "..", "bitmap.dat" y "bloques.dat"
		// strcmp(entry->d_name, ".") == 0 //devuelve true cuando "."
		printf("-----------------------------------------------------------\n");
		printf("entry no se cuanto: %s\n", entry->d_name);
		printf("-----------------------------------------------------------\n");
		printf(" strcmp(entry->d_name, .) %i\n", strcmp(entry->d_name, "."));
		printf(" strcmp(entry->d_name, ..) %i\n", strcmp(entry->d_name, ".."));
		printf(" strcmp(entry->d_name, bloques.dat) %i\n", strcmp(entry->d_name, "bloques.dat"));
		printf(" strcmp(entry->d_name, bitmap.dat) %i\n", strcmp(entry->d_name, "bitmap.dat"));
		printf(" strcmp(entry->d_name, nombre_ArchivoCompactar %i\n", strcmp(entry->d_name, nombre_ArchivoCompactar));

		/*
		if ( strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, "bloques.dat") == 0 || strcmp(entry->d_name, "bitmap.dat") || strcmp(entry->d_name, nombre_ArchivoCompactar)== 0 )
		{
			printf("entry no se cuanto: %s\n",entry->d_name);
			printf("SOY TRUE \n");
		}else{
			printf("entry no se cuanto: %s\n",entry->d_name);
			printf("soy False\n");
		}
		*/

		printf("el if tira esto: %i\n", !(strcmp(entry->d_name, ".") == 0) && !(strcmp(entry->d_name, "..") == 0) && !(strcmp(entry->d_name, "bloques.dat") == 0) && !(strcmp(entry->d_name, "bitmap.dat") == 0) && !(strcmp(entry->d_name, nombre_ArchivoCompactar) == 0));

		if (!(strcmp(entry->d_name, ".") == 0) && !(strcmp(entry->d_name, "..") == 0) && !(strcmp(entry->d_name, "bloques.dat") == 0) && !(strcmp(entry->d_name, "bitmap.dat") == 0) && !(strcmp(entry->d_name, nombre_ArchivoCompactar) == 0))
		{
			printf("funciono bien por fin");

			// Construir la ruta completa del archivo
			char *PATH_archivo;
			PATH_archivo = string_duplicate(PATH_FS);
			char *direccion_archivo = string_from_format("/%s", entry->d_name);
			string_append(&PATH_archivo, direccion_archivo);
			printf("Hasta aca ejecuto\n");
			printf("PATH_archivo: %s\n", PATH_archivo);
			t_config *connfig_metadatos = config_create(PATH_archivo);

			// Obtener el valor de BLOQUE_INICIAL

			int tamanioArchivo = config_get_int_value(connfig_metadatos, "TAMANIO_ARCHIVO");

			int bloqueInicial = config_get_int_value(connfig_metadatos, "BLOQUE_INICIAL");

			int tamanio_en_bloques_del_archivo = bytesABloques(tamanioArchivo, tamanioBloque);

			int bloque_Final_del_archivo = bloqueInicial + (tamanio_en_bloques_del_archivo - 1);

			char *contenido = string_new();
			printf("bloqueInicial: %i,bloque_Final_del_archivo: %i\n", bloqueInicial, bloque_Final_del_archivo);
			for (int i = bloqueInicial * tamanioBloque; i < (bloque_Final_del_archivo + 1) * tamanioBloque; i++)
			{
				char c = map[i];
				char str[2]; // Necesitamos 2 espacios: uno para el carácter y otro para el carácter nulo

				str[0] = c;
				str[1] = '\0'; // Terminador nulo para que sea una cadena válida
				string_append(&contenido, str);
			}

			printf("contenido: %s\n", contenido);

			colArchivo *guardarCola = (colArchivo *)malloc(sizeof(colArchivo));
			guardarCola->contenido = contenido;
			guardarCola->PATH = PATH_archivo;
			guardarCola->tamanio_en_bloques = tamanio_en_bloques_del_archivo;
			guardarCola->bloque_inicial = bloqueInicial;

			// Insertar en el diccionario
			queue_push(colaArchivos, guardarCola);
			config_destroy(connfig_metadatos);
		}
	}

	// printf("Contenido diccionario en key=0: %s, Contenido diccionario en key=2: %s\n",dictionary_get(diccionarioArchivos,"0"),dictionary_get(diccionarioArchivos,"2"));

	closedir(dir);

	//----------FIN 1er parte de la compactacion: Crear diccionario con todos los archivos--------
	char *PATH_bitmap = string_duplicate(PATH_FS);
	string_append(&PATH_bitmap, "/bitmap.dat");

	t_bitarray *mapped_bitarray = bitarray_mmap_from_file(PATH_bitmap, MSB_FIRST);
	if (mapped_bitarray == NULL)
	{
		fprintf(stderr, "Error al mapear el archivo\n");
		return;
	}

	t_bitarray *copia_bitmap = mapped_bitarray;

	// Me creo un bitarray del mismo tamanio que el bitmap.dat pero con todos sus bits en 0
	for (int i = 0; i < bitarray_get_max_bit(copia_bitmap); i++)
	{
		if (bitarray_test_bit(copia_bitmap, i) == 1)
		{
			bitarray_clean_bit(copia_bitmap, i);
		}
	}

	int primerBloqueLibre;

	while (!queue_is_empty(colaArchivos))
	{
		colArchivo *archivo_NO_truncar = queue_pop(colaArchivos);

		int bloqueLibre = 0;

		while (bitarray_test_bit(copia_bitmap, bloqueLibre) != 0)
		{
			bloqueLibre++;
		}

		int bloque_Final_del_archivo_no_truncar = bloqueLibre + (archivo_NO_truncar->tamanio_en_bloques - 1);

		for (int i = bloqueLibre; i <= bloque_Final_del_archivo_no_truncar; i++)
		{
			bitarray_set_bit(copia_bitmap, i);
		}

		t_config *config_archivo_NO_truncar = config_create(archivo_NO_truncar->PATH);

		config_set_value(config_archivo_NO_truncar, "BLOQUE_INICIAL", string_itoa(bloqueLibre));

		config_save(config_archivo_NO_truncar);

		config_destroy(config_archivo_NO_truncar);

		guardar_en_bloque(PATH_bloques, archivo_NO_truncar->contenido, bloqueLibre, tamanioBloque);

		primerBloqueLibre = bloque_Final_del_archivo_no_truncar + 1;
		// free(archivo_NO_truncar->PATH);
		// free(archivo_NO_truncar->contenido);
		// free(archivo_NO_truncar);
	}

	// queue_destroy(colaArchivos);

	// guaro en el ultimo bloque libre que me quedo el contenido del archivo a truncar y ademas los trunco
	char *PATH_archivo_truncar;
	PATH_archivo_truncar = string_duplicate(PATH_FS);
	char *direccion_archivo_truncar = string_from_format("/%s", nombre_ArchivoCompactar);
	string_append(&PATH_archivo_truncar, direccion_archivo_truncar);

	t_config *connfig_archivo_truncar = config_create(PATH_archivo_truncar);

	int tamanioArchivoTruncar = config_get_int_value(connfig_archivo_truncar, "TAMANIO_ARCHIVO");

	int bloqueInicialTruncar = config_get_int_value(connfig_archivo_truncar, "BLOQUE_INICIAL");

	int tamanio_en_bloques_del_archivo_truncar = bytesABloques(tamanioArchivoTruncar, tamanioBloque);

	int bloque_Final_del_archivo_truncar = bloqueInicialTruncar + (tamanio_en_bloques_del_archivo_truncar - 1);

	// me guardo el contenido del archivo que deseo truncar
	char *contenidoTruncable = string_new();
	// printf("bloqueInicial: %i,bloque_Final_del_archivo: %i\n",bloqueInicial,bloque_Final_del_archivo);
	for (int i = bloqueInicialTruncar * tamanioBloque; i < (bloque_Final_del_archivo_truncar + 1) * tamanioBloque; i++)
	{
		char c = map[i];
		char str[2]; // Necesitamos 2 espacios: uno para el carácter y otro para el carácter nulo

		str[0] = c;
		str[1] = '\0'; // Terminador nulo para que sea una cadena válida
		string_append(&contenidoTruncable, str);
	}

	config_set_value(connfig_archivo_truncar, "BLOQUE_INICIAL", string_itoa(primerBloqueLibre));

	config_set_value(connfig_archivo_truncar, "TAMANIO_ARCHIVO", string_itoa(tamanioTruncar));

	config_save(connfig_archivo_truncar);

	config_destroy(connfig_archivo_truncar);

	printf("primerBloqueLibre: %i\n", primerBloqueLibre);

	int bloque_Final_del_archivo_truncar_oficial = primerBloqueLibre + (tamanio_en_bloques_del_archivo_truncar - 1);

	for (int i = primerBloqueLibre; i <= bloque_Final_del_archivo_truncar_oficial; i++)
	{
		bitarray_set_bit(copia_bitmap, i);
	}

	printf("contenidoTruncable: %s", contenidoTruncable);
	guardar_en_bloque(PATH_bloques, contenidoTruncable, primerBloqueLibre, tamanioBloque);

	// elimino el archivo que deseo truncar del copia_bitmap
	/*
	for(int i =bloqueInicialTruncar;i<=bloque_Final_del_archivo_truncar;i++){
		bitarray_clean_bit(copia_bitmap,i);
	}
	*/
	log_info(io_logger, "PID: %i - Fin Compactación.", pid);

	log_info(io_logger, "Aplicando retraso de compactacion");

	int retraso_compactacion = RETRASO_COMPACTACION;

	usleep(retraso_compactacion * 1000);

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
	printf("bloque_Final_del_archivo_truncar_oficial: %i\n", bloque_Final_del_archivo_truncar_oficial);

	// int bloque_Final_del_archivo_truncar_oficial = primerBloqueLibre + (tamanio_en_bloques_del_archivo_truncar- 1);

	int tamanio_en_bloques_despues_de_truncar = bytesABloques(tamanioTruncar, tamanioBloque);

	int bloque_final_despues_de_truncar = primerBloqueLibre + (tamanio_en_bloques_despues_de_truncar - 1);
	// printf("tamanio_en_bloques_despues_de_truncar: %i\n",tamanio_en_bloques_despues_de_truncar);

	printf("tamanio_en_bloques_despues_de_truncar: %i\n", tamanio_en_bloques_despues_de_truncar);
	printf("bloque_final_despues_de_truncar: %i\n", bloque_final_despues_de_truncar);

	for (int i = bloque_Final_del_archivo_truncar_oficial + 1; i <= bloque_final_despues_de_truncar; i++)
	{
		bitarray_set_bit(copia_bitmap, i);
	}

	escribirCentinelaBLoquesDesdeHasta(PATH_bloques, bloque_Final_del_archivo_truncar_oficial + 1, (bloque_final_despues_de_truncar + 1), tamanioBloque, '\0');

	mapped_bitarray = copia_bitmap;
	bitarray_munmap(mapped_bitarray);

	// bitarray_destroy(copia_bitmap);
}

void truncarArchivo(char *nombre_Archivo, int tamanioTruncar, int pid)
{

	log_info(io_logger, "Iniciando truncar del archivo %s", nombre_Archivo);

	char *PATH_FS = PATH_BASE_DIALFS;

	char *PATH_metadata = string_duplicate(PATH_FS);

	char *direccionMetadata = string_from_format("/%s", nombre_Archivo);
	string_append(&PATH_metadata, direccionMetadata);

	t_config *connfig_metadatos = config_create(PATH_metadata);

	char *PATH_bitmap = string_duplicate(PATH_FS);
	string_append(&PATH_bitmap, "/bitmap.dat");

	t_bitarray *mapped_bitarray = bitarray_mmap_from_file(PATH_bitmap, MSB_FIRST);
	if (mapped_bitarray == NULL)
	{
		fprintf(stderr, "Error al mapear el archivo\n");
		return;
	}

	int tamanioBloque = BLOCK_SIZE;

	int tamanioArchivo = config_get_int_value(connfig_metadatos, "TAMANIO_ARCHIVO");

	int bloqueInicial = config_get_int_value(connfig_metadatos, "BLOQUE_INICIAL");

	int tamanio_en_bloques_del_archivo = bytesABloques(tamanioArchivo, tamanioBloque); //(int)ceil((double)tamanioArchivo / (double)tamanioBloque);

	int bloque_Final_del_archivo = bloqueInicial + (tamanio_en_bloques_del_archivo - 1);

	int primer_bloque_a_reclamar = bloque_Final_del_archivo + 1;

	int nueva_cantidad_de_bloques = bytesABloques(tamanioTruncar, tamanioBloque); //(int)ceil((double)tamanioTruncar / (double)tamanioBloque);

	int nuevo_Bloque_Final = bloqueInicial + (nueva_cantidad_de_bloques - 1);
	// 2-3-4-5 (4 bloques inciales), 6-7-8-9 (bloques a reclamar), cantidad_de_bloque_a_reclamar = 4, 4= 8 - 4, cantidad_de_bloques_a_reclamar = nueva_cantidad_de_bloques - tamanio_en_bloques_del_archivo
	// int cantidad_de_bloques_a_reclamar = nueva_cantidad_de_bloques - tamanio_en_bloques_del_archivo;

	char *PATH_bloques = string_duplicate(PATH_FS);
	string_append(&PATH_bloques, "/bloques.dat");

	// BI=4, TI=0, TT=32, TB=16, quiero aumentar 2 bloques : 4-5 0 + 1
	if (tamanioArchivo < tamanioBloque)
	{ // Procedimiento para cuando un archivo inicialmente ocupa 1 solo bloque, osea, su bloque_incial es igual al bloque_final
		primer_bloque_a_reclamar = bloqueInicial + 1;
		bloque_Final_del_archivo = bloqueInicial;
	}

	if (tamanio_en_bloques_del_archivo == nueva_cantidad_de_bloques)
	{ // por ejemplo, los bloques miden 16, y quiero truncar del tamanio 23 a 32, la cantidad de bloques asignados al archivo sigue siendo 2 antes y despues de truncar
		log_info(io_logger, "Los bloques que el archivo ocupa antes y despues de truncar son los mismos");

		bitarray_munmap(mapped_bitarray);

		config_set_value(connfig_metadatos, "TAMANIO_ARCHIVO", string_itoa(tamanioTruncar));
		config_save(connfig_metadatos);
		config_destroy(connfig_metadatos);

		log_info(io_logger, "FIN de truncar");
		return;
	}

	if (hayBloquesOcupados(mapped_bitarray, bloque_Final_del_archivo, nuevo_Bloque_Final) == 1)
	{
		log_info(io_logger, "Sin espacio contiguo suficiente, iniciando compactacion");
		// compactar
		bitarray_munmap(mapped_bitarray);

		compactar(PATH_FS, nombre_Archivo, tamanioTruncar, pid);
	}
	else
	{
		log_info(io_logger, "Hay espacio suficiente para truncar, procediendo con truncar");
		// trunco sin problema
		config_set_value(connfig_metadatos, "TAMANIO_ARCHIVO", string_itoa(tamanioTruncar));
		config_save(connfig_metadatos);
		config_destroy(connfig_metadatos);

		for (int i = primer_bloque_a_reclamar; i <= nuevo_Bloque_Final; i++)
		{
			bitarray_set_bit(mapped_bitarray, i);
		}
		escribirCentinelaBLoquesDesdeHasta(PATH_bloques, primer_bloque_a_reclamar, (nuevo_Bloque_Final + 1), tamanioBloque, '\0'); // cambiar '*' a '\0'
		bitarray_munmap(mapped_bitarray);
	}

	log_info(io_logger, "FIN de truncar");
}

void eliminarArchivo(char *nombre_Archivo)
{
	log_info(io_logger, "Iniciando borrado del archivo %s", nombre_Archivo);

	char *PATH_FS = PATH_BASE_DIALFS;
	char *PATH_metadata = string_duplicate(PATH_FS);

	char *direccionMetadata = string_from_format("/%s", nombre_Archivo);

	string_append(&PATH_metadata, direccionMetadata);

	t_config *connfig_metadatos = config_create(PATH_metadata);

	char *PATH_bitmap = string_duplicate(PATH_FS);
	string_append(&PATH_bitmap, "/bitmap.dat");

	t_bitarray *mapped_bitarray = bitarray_mmap_from_file(PATH_bitmap, MSB_FIRST);
	if (mapped_bitarray == NULL)
	{
		fprintf(stderr, "Error al mapear el archivo\n");
		return;
	}

	int bloqueInicial = config_get_int_value(connfig_metadatos, "BLOQUE_INICIAL");
	int tamnioArchivo = config_get_int_value(connfig_metadatos, "TAMANIO_ARCHIVO");
	config_destroy(connfig_metadatos);

	log_info(io_logger, "Liberando bloques bitmap");
	if (tamnioArchivo == 0)
	{
		bitarray_clean_bit(mapped_bitarray, bloqueInicial);
	}
	else
	{
		int tamanioBloque = BLOCK_SIZE;
		// 0-1-2 64 64/16 = 4 el archivo ocupa 4 bloques, osea, ocupa los bloque 2,3,4,5 .... 5= 2+3 = 2 + (4-1) bloqeu final = bloque inicial + (tamaño_en_bloques -1)

		// bloque_inicial = 0,  el archivo mide 64, 64/16 = 4, el archivo mide 4 bloques: 0-1-2-3, bloque_final = 3, 3 = 0 + 3 = 0 + (4-1) ---> bloque_final = bloque_inicial + (tamanio_en_bloques-1)

		int tamanio_en_bloques_del_archivo = (int)ceil((double)tamnioArchivo / (double)tamanioBloque);

		int bloque_Final_del_archivo = bloqueInicial + (tamanio_en_bloques_del_archivo - 1);

		// int bloquesOffset = (int)ceil((double)tamnioArchivo / (double)tamanioBloque);

		for (int i = bloqueInicial; i <= bloque_Final_del_archivo; i++)
		{
			// bloqueInicial=0, 64 bytes, 64/16 = 4, 0-1-2-3, bloque_Final_del_archivo=3, i=0, 0<=3,2<=3,3<=3
			// bloque_inicial = 2, 64 bytes, 64/16 = 4, 2-3-4-5, bloque_Final_del_archivo = 5, i=2, 2<=5,3<=5,4<=4,5<=5
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

void crearArchivo(char *nombre_Archivo)
{
	log_info(io_logger, "Iniciando creacion de archivo de Metadata %s", nombre_Archivo);

	char *PATH_FS = PATH_BASE_DIALFS;

	printf("EL PATH FS: %s\n", PATH_FS);

	char *PATH_metadata = string_duplicate(PATH_FS);

	char *direccionMetadata = string_from_format("/%s", nombre_Archivo);

	string_append(&PATH_metadata, direccionMetadata);

	FILE *archivoACrear = fopen(PATH_metadata, "w"); // creo el archivo de metadatos

	char *PATH_bitmap = string_duplicate(PATH_FS);
	string_append(&PATH_bitmap, "/bitmap.dat");

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
	int tamanioBloque = BLOCK_SIZE;
	escribirCentinelaInicialBLoques(PATH_FS, bloqueInicial, tamanioBloque, '\0'); // cambiar '0' a '\0'

	log_info(io_logger, "Fin creacion de archivo de metadata");
}

void avisarKernelTerminoEjecutarIO()
{

	t_buffer *buffer = crear_buffer();
	buffer->size = 0;
	buffer->stream = NULL;

	cargar_string_al_buffer(buffer, nombreInterACrear);

	printf("el nombre de la interfaz es: %s\n", nombreInterACrear);

	t_paquete *paquete = crear_super_paquete(LIBERAR_INTERFAZ, buffer);
	enviar_paquete(paquete, fd_kernel);
	destruir_paquete(paquete);

	printf("fui a liberar la interfaz\n");
}

void atender_interfaz_kernel(int *arg)
{
	int fd_entradasalida_kernel = *arg;
	bool control_key = 1;
	t_buffer *un_buffer;
	int pid;
	char *nombreArchivo;
	int registro_tamanio;

	while (control_key)
	{
		int cod_op = recibir_operacion(fd_entradasalida_kernel);
		printf("el codigo de operacion es: %d\n", cod_op);

		switch (cod_op)
		{
		case ENVIAR_IO_GEN_SLEEP:

			un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
			pid = extraer_int_del_buffer(un_buffer);
			int unidades_trabajo = extraer_int_del_buffer(un_buffer);

			log_info(io_logger, "PID: %i - Operacion: IO_GEN_SLEEP", pid);

			printf("mande a dormir a la io\n");

			usleep(unidades_trabajo * TIEMPO_UNIDAD_TRABAJO * 1000);

			avisarKernelTerminoEjecutarIO();

			break;

		case ENVIAR_IO_STDIN_READ:

			un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
			pid = extraer_int_del_buffer(un_buffer);
			int tamanio_restante_pag_read = extraer_int_del_buffer(un_buffer);
			int tamanio_escribir_read = extraer_int_del_buffer(un_buffer);
			int cant_direcciones_read = extraer_int_del_buffer(un_buffer);
			char *input_usuario = readline("input >");

			log_info(io_logger, "PID: %i - Operacion: IO_STDIN_READ", pid);

			t_buffer *buffer = crear_buffer();
			buffer->size = 0;
			buffer->stream = NULL;

			cargar_string_al_buffer(buffer, nombreInterACrear); // hay que pasar esto asi desp memoria sabe a quien responderle
			cargar_string_al_buffer(buffer, input_usuario);
			cargar_int_al_buffer(buffer, tamanio_restante_pag_read);
			cargar_int_al_buffer(buffer, tamanio_escribir_read);

			int numero;

			for (int i = 0; i < cant_direcciones_read; i++)
			{
				numero = extraer_int_del_buffer(un_buffer);
				cargar_int_al_buffer(buffer, numero);
			}

			t_paquete *paquete = crear_super_paquete(ESCRIBIR_MEMORIA_IO, buffer);
			enviar_paquete(paquete, fd_memoria);
			destruir_paquete(paquete);

			// Se libera la interfaz en io-memoria

			break;

		case ENVIAR_IO_STDOUT_WRITE:
			un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
			pid = extraer_int_del_buffer(un_buffer);
			int tamanioXD = extraer_int_del_buffer(un_buffer);

			log_info(io_logger, "PID: %i - Operacion: IO_STDOUT_WRITE", pid);

			t_buffer *buffer2 = crear_buffer();
			buffer2->size = 0;
			buffer2->stream = NULL;

			cargar_string_al_buffer(buffer2, nombreInterACrear); // hay que pasar esto asi desp memoria sabe a quien responderle
			cargar_int_al_buffer(buffer2, tamanioXD);

			for (int i = 0; i < tamanioXD; i++)
			{

				numero = extraer_int_del_buffer(un_buffer);
				cargar_int_al_buffer(buffer2, numero);
			}

			t_paquete *paquete2 = crear_super_paquete(LEER_MEMORIA_PALABRA, buffer2);
			enviar_paquete(paquete2, fd_memoria);
			destruir_paquete(paquete2);

			// Se libera la interfaz en io-memoria

			break;
		case ENVIAR_IO_FS_CREATE:
			un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
			pid = extraer_int_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);
			printf("ejecute un create\n");

			// INICIAR_PROCESO /scripts_memoria/instrucciones.txt

			// log_info(io_logger, "PID: %i - Operacion: IO_FS_CREATE",pid);

			printf("el pid es %d\n", pid);
			printf("el nombre del archivo es: %s\n", nombreArchivo);
			printf("el path: %s\n", PATH_BASE_DIALFS);

			// log_info(io_logger, "PID: %i - Operacion: IO_FS_CREATE",pid);

			// log_info(io_logger, "PID: %i - Crear Archivo: %s",nombreArchivo);
			crearArchivo(nombreArchivo);

			log_info(io_logger, "Consumiendo unidad de tiempo");
			usleep(TIEMPO_UNIDAD_TRABAJO * 1000);
			log_info(io_logger, "Unidad de tiempo consumida");

			avisarKernelTerminoEjecutarIO();

			break;
		case ENVIAR_IO_FS_DELETE:
			un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
			pid = extraer_int_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);
			printf("ejecute un delete\n");

			// log_info(io_logger, "PID: %i - Operacion: IO_FS_DELETE",pid);

			printf("el pid es %d\n", pid);
			printf("el nombre del archivo es: %s\n", nombreArchivo);

			// log_info(io_logger, "PID: %i - Eliminar Archivo: %s",nombreArchivo);
			eliminarArchivo(nombreArchivo);

			log_info(io_logger, "Consumiendo unidad de tiempo");
			usleep(TIEMPO_UNIDAD_TRABAJO * 1000);
			log_info(io_logger, "Unidad de tiempo consumida");

			avisarKernelTerminoEjecutarIO();

			break;
		case ENVIAR_IO_FS_TRUNCATE:
			un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
			pid = extraer_int_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);
			registro_tamanio = extraer_int_del_buffer(un_buffer);
			printf("ejecute un truncate\n");

			// log_info(io_logger, "PID: %i - Operacion: IO_FS_TRUNCATE",pid);

			printf("el pid es %d\n", pid);
			printf("el nombre del archivo es: %s\n", nombreArchivo);
			printf("el registro tamanio es: %d\n", registro_tamanio);

			// log_info(io_logger, "PID: %i - Truncar Archivo: %s - Tamaño: %i",pid,nombreArchivo,registro_tamanio);
			truncarArchivo(nombreArchivo, registro_tamanio, pid);

			log_info(io_logger, "Consumiendo unidad de tiempo");
			usleep(TIEMPO_UNIDAD_TRABAJO * 1000);
			log_info(io_logger, "Unidad de tiempo consumida");

			avisarKernelTerminoEjecutarIO();

			break;
		case ENVIAR_IO_FS_WRITE:
			un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
			pid = extraer_int_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);
			int registro_puntero_write = extraer_int_del_buffer(un_buffer);
			int tamanio_write = extraer_int_del_buffer(un_buffer);

			log_info(io_logger, "PID: %i - Operacion: IO_FS_WRITE", pid);

			t_buffer *buffer3 = crear_buffer();
			buffer3->size = 0;
			buffer3->stream = NULL;

			cargar_string_al_buffer(buffer3, nombreInterACrear); // hay que pasar esto asi desp memoria sabe a quien responderle
			cargar_int_al_buffer(buffer3, tamanio_write);

			for (int i = 0; i < tamanio_write; i++)
			{

				numero = extraer_int_del_buffer(un_buffer);
				printf("el numero %d\n", numero);
				cargar_int_al_buffer(buffer3, numero);
			}

			t_paquete *paquete3 = crear_super_paquete(LEER_MEMORIA_PALABRA_DIALS_FS, buffer3);
			enviar_paquete(paquete3, fd_memoria);
			destruir_paquete(paquete3);

			printf("el pid es %d\n", pid);
			printf("el nombre del archivo es: %s\n", nombreArchivo);

			sem_wait(&esperar_palabra_memoria);

			printf("mensaje obtenido: %s\n", palabraIOWrite);

			log_info(io_logger, "PID: %i - Escribir Archivo: %s - Tamaño a Escribir: %i - Puntero Archivo: %i", pid, nombreArchivo, tamanio_write, registro_puntero_write);
			escribirArchivo(nombreArchivo, registro_puntero_write, palabraIOWrite);

			log_info(io_logger, "Consumiendo unidad de tiempo");
			usleep(TIEMPO_UNIDAD_TRABAJO * 1000);
			log_info(io_logger, "Unidad de tiempo consumida");

			avisarKernelTerminoEjecutarIO();

			break;
		case ENVIAR_IO_FS_READ:

			un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
			pid = extraer_int_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);
			int punteroRead = extraer_int_del_buffer(un_buffer);
			int bytesRestantesPagina = extraer_int_del_buffer(un_buffer);
			int tamanioEscribirRead = extraer_int_del_buffer(un_buffer);
			int cantidadDireccionesXD = extraer_int_del_buffer(un_buffer);

			// faltan extraer las direcciones fisicas del buffer

			log_info(io_logger, "PID: %i - Operacion: IO_FS_READ", pid); // falta pid

			printf("el pid es %d\n", pid);
			printf("el nombre del archivo es: %s\n", nombreArchivo);
			printf("puntero read %d\n", punteroRead);
			printf("byts restantes %d\n", bytesRestantesPagina);
			printf("tamanio escribir %d\n", tamanioEscribirRead);
			printf("cantidad direcciones %d\n", cantidadDireccionesXD);

			// printf("mensaje obtenido: %s\n",palabraIOWrite);

			// log_info(io_logger, "PID: %i - Escribir Archivo: %s - Tamaño a Escribir: %i - Puntero Archivo: %i",pid,nombreArchivo,tamanio_write,registro_puntero_write);
			char *leidoDelArchivo = leerArchivo(nombreArchivo, registro_puntero_write, registro_tamanio);

			t_buffer *bufferXDD = crear_buffer();
			bufferXDD->size = 0;
			bufferXDD->stream = NULL;

			cargar_string_al_buffer(bufferXDD, nombreInterACrear);
			cargar_string_al_buffer(bufferXDD, leidoDelArchivo);
			cargar_int_al_buffer(bufferXDD, bytesRestantesPagina);
			cargar_int_al_buffer(bufferXDD, tamanioEscribirRead);

			int numero23;

			for (int i = 0; i < cantidadDireccionesXD; i++)
			{
				numero23 = extraer_int_del_buffer(un_buffer);
				printf("las direcciones fisicas son: %d\n", numero23);
				cargar_int_al_buffer(bufferXDD, numero23);
			}

			t_paquete *paquete23 = crear_super_paquete(ESCRIBIR_MEMORIA_IO, bufferXDD);
			enviar_paquete(paquete23, fd_memoria);
			destruir_paquete(paquete23);

			log_info(io_logger, "Consumiendo unidad de tiempo");
			usleep(TIEMPO_UNIDAD_TRABAJO * 1000);
			log_info(io_logger, "Unidad de tiempo consumida");

			avisarKernelTerminoEjecutarIO();

			break;

		case -1:

			control_key = 0;
			break;
		default:
			log_warning(logger, "Operacion desconocida de KERNEL - IO");
			break;
		}
	}
}

/*
ENVIAR_IO_FS_CREATE,
	ENVIAR_IO_FS_DELETE,
	ENVIAR_IO_FS_TRUNCATE,
	ENVIAR_IO_FS_WRITE,
	ENVIAR_IO_FS_READ,*/