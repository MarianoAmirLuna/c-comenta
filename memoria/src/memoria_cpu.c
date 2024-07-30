#include "../include/memoria_cpu.h"
#include <utils/shared.h>
#include <inttypes.h>
#include <stdint.h>

#define MAX_LEN 100

char *obtenerInstruccion(char *path, int programCounter)
{ // devuelve la instrucción que está en la fila que indica el program counter
	FILE *archivo = fopen(path + 1, "r");
	if (archivo == NULL)
	{
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}

	char linea[MAX_LEN];
	int contador = 0;

	while (fgets(linea, sizeof(linea), archivo) != NULL)
	{
		contador++;
		if (contador == programCounter)
		{
			// Elimina el salto de línea al final de la línea
			char *nuevaLinea = strchr(linea, '\n');
			if (nuevaLinea != NULL)
			{
				*nuevaLinea = '\0';
			}
			fclose(archivo);
			return strdup(linea); // Devuelve una copia de la línea encontrada
		}
	}

	fclose(archivo);
	return NULL; // No se encontró la instrucción para el Program Counter dado
}

bool condition_id_igual_n(void *elemento)
{
	path_conID *dato = (path_conID *)elemento;
	return (dato->id == id_global);
}

char *obtener_instruccion_lista(int pid, int program_counter)
{
	id_global = pid;

	path_conID *elemento_lista = list_find(list_path_id, condition_id_igual_n);

	printf("el path es: %s\n", elemento_lista->path); // si rompe esto es probablemente porque saco algo inexistente de la lista

	return elemento_lista->path;
}

void devolver_instruccion(t_buffer *un_buffer)
{
	int pid = extraer_int_del_buffer(un_buffer);
	int program_counter = extraer_int_del_buffer(un_buffer);
	program_counter++;

	// printf("el pid que fue serializado: %d\n",pid);
	// printf("el pid que fue serializado: %d\n",program_counter);
	usleep(RETARDO_RESPUESTA * 1000);

	char *path_instruccion = obtener_instruccion_lista(pid, program_counter);

	t_buffer *a_enviar = crear_buffer();

	a_enviar->size = 0;
	a_enviar->stream = NULL;

	// printf("el path de instrucciones es: %s\n",path_instruccion);

	// printf("el path instruccion es:%s\n",path_instruccion);
	// printf("el program counter es:%d\n",program_counter);

	char *instruccion = obtenerInstruccion(path_instruccion, program_counter);

	// printf("la instruccion es: %s\n",instruccion);

	cargar_string_al_buffer(a_enviar, instruccion);

	printf("la instruccion es: %s\n", instruccion);

	t_paquete *un_paquete = crear_super_paquete(RECIBIR_INSTRUCCION, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);
}

void devolverTamanioPagina(t_buffer *un_buffer)
{
	int numero = extraer_int_del_buffer(un_buffer);

	t_buffer *a_enviar = crear_buffer();

	a_enviar->size = 0;
	a_enviar->stream = NULL;

	cargar_int_al_buffer(a_enviar, TAM_PAGINA);

	t_paquete *un_paquete = crear_super_paquete(RECIBIR_TAMANIO, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);
}

void solicitarDireccion(int direccion_logica)
{
	t_buffer *a_enviar = crear_buffer();
	a_enviar->size = 0;
	a_enviar->stream = NULL;

	cargar_int_al_buffer(a_enviar, direccion_logica);

	t_paquete *un_paquete = crear_super_paquete(PEDIR_DIRECCION, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);

	printf("toy esperando\n");
	sem_wait(&esperar_df);
	printf("pase el semaforo\n");
}

void leerDato(t_buffer *un_buffer)
{
	// Recibimos los datos para poder hacer el memcopy
	uint8_t data8;
	uint32_t data32;
	uint32_t datoLeido32;
	uint32_t datoLeido8;

	usleep(RETARDO_RESPUESTA * 1000);

	int dirFisicaDelDato = extraer_int_del_buffer(un_buffer);
	int segundaDF = extraer_int_del_buffer(un_buffer);
	int tamanioALeer = extraer_int_del_buffer(un_buffer);
	int seEscribe2paginas = extraer_int_del_buffer(un_buffer);
	int tamanioRestantePagina = extraer_int_del_buffer(un_buffer);
	char *registroDatos = extraer_string_del_buffer(un_buffer);

	printf("Llegaron los datos a leer.\n");
	printf("la direccion fisica donde hay que leer es: %d\n", dirFisicaDelDato);
	printf("el tamanio a leer: %d\n", tamanioALeer);
	printf("el tamanio que resta de pag es: %d\n", tamanioRestantePagina);

	if (tamanioALeer == 1) // Caso donde tenemos que leer algo de 1 byte
	{
		memcpy(&datoLeido8, (memoriaPrincipal + dirFisicaDelDato), tamanioALeer);
		printf("############## EL DATO A LEER ES:%" PRIu8 "\n", datoLeido8);
	}
	else
	{
		if (seEscribe2paginas == 1)
		{ // caso turbio que hay que leer en 2 paginas diferentes
			// ahora leo solo la parte 1 - escribo en el registro la 1ra parte del marco

			printf("direcion fisica dato: %d\n", dirFisicaDelDato);
			printf("segunda fd: %d\n", segundaDF);

			printf("leyo el caso turbio\n");

			memcpy(&datoLeido32, memoriaPrincipal + dirFisicaDelDato, tamanioRestantePagina);
			memcpy((uint8_t *)&datoLeido32 + tamanioRestantePagina, memoriaPrincipal + segundaDF, 4 - tamanioRestantePagina);

			printf("############## EL DATO A LEER ES EN EL CASO TRUBIO:%" PRIu32 "\n", datoLeido32);
		}
		else // caso donde tenemos que leer algo de 4 bytes, pero está todo en 1 solo marco
		{
			memcpy(&datoLeido32, (memoriaPrincipal + dirFisicaDelDato), tamanioALeer);
			printf("############## EL DATO A LEER ES:%" PRIu32 "\n", datoLeido32);
		}
	}

	t_buffer *a_enviar = crear_buffer();
	a_enviar->size = 0;
	a_enviar->stream = NULL;

	cargar_int_al_buffer(a_enviar, dirFisicaDelDato);
	cargar_string_al_buffer(a_enviar, registroDatos);
	cargar_int_al_buffer(a_enviar, tamanioALeer);

	if (tamanioALeer == 1)
	{
		cargar_uint8_al_buffer(a_enviar, datoLeido8);
	}
	else
	{
		cargar_uint32_al_buffer(a_enviar, datoLeido32);
	}

	t_paquete *un_paquete = crear_super_paquete(LECTURA_HECHA, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);

	terminoInstruccionMemoria();
}

void escribirDato(t_buffer *un_buffer)
{
	// Recibimos los datos para poder hacer el memcopy
	u_int8_t data8;
	uint32_t data32;

	usleep(RETARDO_RESPUESTA * 1000);

	int direccion_logica = extraer_int_del_buffer(un_buffer);
	int direccion_fisica = extraer_int_del_buffer(un_buffer);
	int segundaDF = extraer_int_del_buffer(un_buffer);
	int tamanio_a_escribir = extraer_int_del_buffer(un_buffer);

	t_buffer *a_enviar = crear_buffer();
	a_enviar->size = 0;
	a_enviar->stream = NULL;

	if (tamanio_a_escribir == 1)
	{
		data8 = extraer_uint8_del_buffer(un_buffer);
	}
	else
	{
		data32 = extraer_uint32_del_buffer(un_buffer);
	}

	int seEscribe2paginas = extraer_int_del_buffer(un_buffer);
	int tamanioRestantePagina = extraer_int_del_buffer(un_buffer);

	printf("la direccion fisica: %d\n", direccion_fisica);
	printf("el tamanio a escribir: %d\n", tamanio_a_escribir);
	printf("El valor de uint8_t es: %u\n", data8);
	printf("El valor de uint32_t es: %u\n", data32);

	// Le mando la dirección física a CPU para los LOGs obligatorios
	cargar_int_al_buffer(a_enviar, direccion_fisica);

	if (tamanio_a_escribir == 1)
	{
		memcpy((memoriaPrincipal + direccion_fisica), &data8, tamanio_a_escribir);
		tamanioRestantePagina = tamanioRestantePagina - 1;

		// Le mando el dato escrito a CPU
		cargar_int_al_buffer(a_enviar, tamanio_a_escribir);
		cargar_uint8_al_buffer(a_enviar, data8);
	}
	else
	{
		if (seEscribe2paginas == 1)
		{ // caso turbio que hay que escribir en 2 paginas diferentes

			uint32_t data32_reconstruido = 0;

			void *data32_parte_1 = malloc(tamanioRestantePagina);
			void *data32_parte_2 = malloc(4 - tamanioRestantePagina);

			void *data32_puntero = &data32;
			void *data32_reconstruido_puntero = &data32_reconstruido;

			// estos 2 memcpy son para dividir el uint32
			memcpy(data32_parte_1, data32_puntero, tamanioRestantePagina);
			memcpy(data32_parte_2, data32_puntero + tamanioRestantePagina, 4 - tamanioRestantePagina);

			printf("El valor de ECX completo es: %d \n", data32);
			printf("El valor de ECX antes de reconstruirlo: %d \n", data32_reconstruido);

			// estos sirven para reconstruir el puntero
			memcpy(data32_reconstruido_puntero, data32_parte_1, tamanioRestantePagina);
			memcpy(data32_reconstruido_puntero + tamanioRestantePagina, data32_parte_2, 4 - tamanioRestantePagina);

			printf("El valor de ECX despues de reconstruirlo: %d \n", data32_reconstruido);

			// Le mando el dato escrito a CPU
			cargar_int_al_buffer(a_enviar, tamanio_a_escribir);
			cargar_uint32_al_buffer(a_enviar, data32_reconstruido);

			// ahora escribo posta en la memoria solo la parte 1
			printf("Escribie en el caso turbio\n");
			printf("Primer df %d\n", direccion_fisica);
			printf("Segunda df %d\n", segundaDF);

			memcpy(memoriaPrincipal + direccion_fisica, data32_parte_1, tamanioRestantePagina);
			memcpy(memoriaPrincipal + segundaDF, data32_parte_2, 4 - tamanioRestantePagina);

			free(data32_parte_1);
			free(data32_parte_2);
		}
		else
		{
			memcpy((memoriaPrincipal + direccion_fisica), &data32, tamanio_a_escribir);

			// Le mando el dato escrito a CPU
			cargar_int_al_buffer(a_enviar, tamanio_a_escribir);
			cargar_uint32_al_buffer(a_enviar, data32);
		}

		tamanioRestantePagina = tamanioRestantePagina - 4;
	}

	int num_pag = direccion_logica / TAM_PAGINA;

	if (tamanioRestantePagina <= 0)
	{ // me quede sin espacio en la pagina y tengo que setear el marco como "no disponible"
		bitarray_set_bit(frames_ocupados_ppal, num_pag);
	}

	/* Mandamos basura, para hacer el sem_post
	t_buffer *a_enviar = crear_buffer();
	a_enviar->size = 0;
	a_enviar->stream = NULL;

	cargar_int_al_buffer(a_enviar, 1);*/

	t_paquete *un_paquete = crear_super_paquete(ESCRITURA_HECHA, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);

	terminoInstruccionMemoria();
}

bool condition_tabla_pagina(void *elemento)
{
	tablaPaginas *dato = (tablaPaginas *)elemento;
	return (dato->pid == id_global);
}

tablaPaginas *obtener_tabla_pagina(int pid)
{
	id_global = pid;

	tablaPaginas *tablaPagina = list_find(listaTablaPaginas, condition_tabla_pagina);

	return tablaPagina;
}

void terminoInstruccionMemoria()
{
	t_buffer *a_enviar = crear_buffer();
	a_enviar->size = 0;
	a_enviar->stream = NULL;

	cargar_int_al_buffer(a_enviar, 0);

	t_paquete *un_paquete = crear_super_paquete(TERMINO_INSTRUCCION_MEMORIA, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);
}

void imprimirBitsValidez(tablaPaginas p)
{

	printf("PID: %d\n", p.pid);
	for (int i = 0; i < 40; i++)
	{
		printf("Bit de validez del marco %d: %d\n", i, p.array[i].bitValidez);
		printf("El nro de marco asignado: %d:\n", p.array[i].marco);
	}
}

void imprimirBitmapMemoriaPrincipal()
{
	bool estaOcupado;

	size_t numeroMax = bitarray_get_max_bit(frames_ocupados_ppal);

	for (size_t i = 0; i < numeroMax; i++)
	{
		estaOcupado = bitarray_test_bit(frames_ocupados_ppal, i);
		if (estaOcupado)
		{
			printf("El bit: %d esta ocupado.\n", i);
		}
		else
		{
			printf("El bit: %d esta libre.\n", i);
		}
	}
}

int calcularEspacioDisponibleEnMemoria()
{
	bool estaOcupado;
	int acumulador = 0;

	size_t numeroMax = bitarray_get_max_bit(frames_ocupados_ppal);

	for (size_t i = 0; i < numeroMax; i++)
	{
		estaOcupado = bitarray_test_bit(frames_ocupados_ppal, i);

		if (!estaOcupado)
		{
			acumulador = acumulador + TAM_PAGINA;
		}
	}

	printf("El espacio total Disponible en la memoria es de %d\n", acumulador);

	return acumulador;
}

void resize(t_buffer *un_buffer)
{

	int pid = extraer_int_del_buffer(un_buffer);
	int tamanioAModificar = extraer_int_del_buffer(un_buffer);

	printf("se va a hacer un resize de: %d\n", tamanioAModificar);

	tablaPaginas *tablaPag = obtener_tabla_pagina(pid);

	printf("el pid de la tabla de paginas: %d\n", tablaPag->pid);

	int cantBitsValidez = tablaPag->cantMarcos;

	printf("la cant de bits de validez en 1: %d\n", cantBitsValidez);

	int tamanioActual = cantBitsValidez * TAM_PAGINA;

	int especioAumentar = tamanioAModificar - tamanioActual;

	int espacioDisponibleMemoria = calcularEspacioDisponibleEnMemoria();

	usleep(RETARDO_RESPUESTA * 1000);

	if (especioAumentar > espacioDisponibleMemoria)
	{
		t_buffer *buffer = crear_buffer();
		buffer->size = 0;
		buffer->stream = NULL;

		cargar_int_al_buffer(buffer, 7);

		t_paquete *paquete = crear_super_paquete(OUT_OF_MEMORY, buffer);
		enviar_paquete(paquete, fd_cpu);
		destruir_paquete(paquete);
	}
	else
	{

		printf("el tamanio actual es: %d\n", tamanioActual);
		printf("el tamanio a modificar es: %d\n", tamanioAModificar);

		if (tamanioAModificar > tamanioActual)
		{ // si necesitamos mas paginas
			printf("ENTRE AL IFFFFFFFFFFFFF\n");
			log_debug(memoria_log_debug, "PID: %d - Tamaño Actual: %d - Tamaño a Ampliar: %d", pid, tamanioActual, tamanioAModificar);

			int paginasNecesarias = ceil((double)tamanioAModificar / (double)TAM_PAGINA);

			printf("se van a solicitar: %d\n", paginasNecesarias);

			reservarFrames(tablaPag, paginasNecesarias, cantBitsValidez); // aca esta el error
		}
		else
		{
			if (tamanioAModificar <= tamanioActual)
			{ // si quiero sacar paginas, tengo que cambiar los valores del bitarray & liberar las paginas
				printf("ENTRE AL ELSEEEEEEEEE");
				log_debug(memoria_log_debug, "PID: %d - Tamaño Actual: %d - Tamaño a Reducir: %d", pid, tamanioActual, tamanioAModificar);
				int cantBytesModificar = tamanioActual - tamanioAModificar;
				int cantPaginasABorrar = ceil((double)cantBytesModificar / (double)TAM_PAGINA);
				liberarFrames(tablaPag, cantPaginasABorrar);
			}
		}

		printf("-------------------------------");

		printf("PID: %d\n", tablaPag->pid);
		for (int i = 0; i < 40; i++)
		{
			printf("Bit de validez del marco %d: %d\n", i, tablaPag->array[i].bitValidez);
			printf("El nro de marco asignado: %d\n", tablaPag->array[i].marco);
		}

		printf("-------------------------------");

		terminoInstruccionMemoria();
		imprimirBitmapMemoriaPrincipal();
	}
}

void buscarMarco(t_buffer *un_buffer)
{
	int num_pag = extraer_int_del_buffer(un_buffer);
	int pid = extraer_int_del_buffer(un_buffer);
	t_buffer *a_enviar = crear_buffer();

	usleep(RETARDO_RESPUESTA * 1000);

	a_enviar->size = 0;
	a_enviar->stream = NULL;

	tablaPaginas *tablaDePaginas = obtener_tabla_pagina(pid);
	int marco = tablaDePaginas->array[num_pag].marco;
	// Si estan pasando cosas raras con el marco que va a recibir el cpu. Puede ser que no se haya inicializado bien el array.

	log_debug(memoria_log_debug, "PID: %d - Pagina: %d - Marco: %d", pid, num_pag, marco);

	cargar_int_al_buffer(a_enviar, marco);

	t_paquete *un_paquete = crear_super_paquete(RECIBIR_MARCO, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);
}

void obtenerCantInstrucciones(int pid)
{
	pthread_mutex_t proteger_contador = PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&proteger_contador);
	id_global = pid;

	path_conID *elemento_lista = list_find(list_path_id, condition_id_igual_n);

	int cantInstrucciones = contarInstrucciones(elemento_lista->path);
	pthread_mutex_unlock(&proteger_contador);

	t_buffer *a_enviar = crear_buffer();

	a_enviar->size = 0;
	a_enviar->stream = NULL;

	cargar_int_al_buffer(a_enviar, cantInstrucciones);

	t_paquete *un_paquete = crear_super_paquete(CANT_INTRUCCIONES, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);
}

void obtenerCortesDePagina(t_list *lista, int tamanio_a_escribir, int restante_pagina)
{
	int acumulador = 0; // 70 escribir 10 restante

	while (tamanio_a_escribir > 0)
	{
		if (tamanio_a_escribir <= restante_pagina)
		{
			tamanio_a_escribir = 0; // no agrego nada a la lista y corto el while
		}
		else if (tamanio_a_escribir > restante_pagina)
		{
			int *valorAgregar = (int *)malloc(sizeof(int));
			*valorAgregar = acumulador + restante_pagina;
			list_add(lista, valorAgregar); //[10,42]
			tamanio_a_escribir = tamanio_a_escribir - restante_pagina;
			acumulador = acumulador + restante_pagina;
		}
		restante_pagina = TAM_PAGINA; // 60 32 => 28 32
	}
}

bool necesitoNuevaDF(t_list *cortesPagina, int cantIteraciones)
{
	int *numero;
	int longitud = list_size(cortesPagina);

	for (int i = 0; i < longitud; i++)
	{
		numero = (int *)list_get(cortesPagina, i); // Casteo explícito a (int*)
		if (*numero == cantIteraciones)
		{ // Dereferenciar el puntero para obtener el valor de 'numero'
			return true;
		}
	}

	return false;
}

void escribirMemoria(t_buffer *un_buffer)
{
	char *stringAEscribir = extraer_string_del_buffer(un_buffer);
	int bytes_restantes_en_pagina = extraer_int_del_buffer(un_buffer);
	int tamanioAEscribir = extraer_int_del_buffer(un_buffer);
	int cantIteraciones = 0;
	t_list *cortesPagina = list_create();

	obtenerCortesDePagina(cortesPagina, tamanioAEscribir, bytes_restantes_en_pagina); // [10,42,74]

	int longitud = list_size(cortesPagina);

	for (int i = 0; i < longitud; i++)
	{
		int *pepe = (int *)list_get(cortesPagina, i);
		printf("valor de la lista cortes: %d\n", *pepe);
	}

	printf("la longitud es: %d\n", longitud);

	int df = extraer_int_del_buffer(un_buffer);

	while (cantIteraciones < tamanioAEscribir)
	{

		if (necesitoNuevaDF(cortesPagina, cantIteraciones))
		{
			df = extraer_int_del_buffer(un_buffer);
		}

		printf("en la direccion fisica: %d\n", df);
		printf("escribo: %c\n", stringAEscribir[cantIteraciones]);

		memcpy(memoriaPrincipal + df, &stringAEscribir[cantIteraciones], 1);

		cantIteraciones++;
		df++;
	}

	printf("cantidad de iteraciones: %d\n", cantIteraciones);
}

void concat_uint8_to_string(char *str, uint8_t ch)
{
    size_t len = strlen(str); // Encuentra la longitud actual del string

    printf("el caracter es: %c\n", (char)ch);

    str[len] = (char)ch; // Añade el carácter al final del string
    str[len + 1] = '\0'; // Añade el terminador nulo
}

void leerMemoriaUnString(t_buffer *un_buffer)
{
	int bytes_restantes_en_pagina = extraer_int_del_buffer(un_buffer);
	int tamanioALeer = extraer_int_del_buffer(un_buffer);

	int cantIteraciones = 0;
	t_list *cortesPagina = list_create();

	obtenerCortesDePagina(cortesPagina, tamanioALeer, bytes_restantes_en_pagina); // [10,42,74] Cada cuantos bytes necesitas un nuevo marco (osea una nueva df)

	int longitud = list_size(cortesPagina);

	for (int i = 0; i < longitud; i++)
	{
		int *pepe = (int *)list_get(cortesPagina, i);
		printf("valor de la lista cortes: %d\n", *pepe);
	}

	printf("la longitud es: %d\n", longitud);

	int df = extraer_int_del_buffer(un_buffer);
	uint8_t infoLeida;
	char str[tamanioALeer + 1]; // Inicializa el string como vacío
    str[0] = '\0';

	while (cantIteraciones < tamanioALeer)
	{

		if (necesitoNuevaDF(cortesPagina, cantIteraciones))
		{
			df = extraer_int_del_buffer(un_buffer);
		}

		printf("en la direccion fisica: %d\n", df);
		memcpy(&infoLeida, memoriaPrincipal + df, 1);
		concat_uint8_to_string(str, infoLeida);
		cantIteraciones++;
		df++;
	}

	printf("cantidad de iteraciones: %d\n", cantIteraciones);

	printf("###################### MENSAJE LEIDO: %s \n",str);

	t_buffer *a_enviar = crear_buffer();
	a_enviar->size = 0;
	a_enviar->stream = NULL;

	cargar_string_al_buffer(a_enviar, str);
	usleep(RETARDO_RESPUESTA * 1000);

	t_paquete *un_paquete = crear_super_paquete(MANDAR_LECTURA_DE_STRING, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);

}

void leer_caracter(int df)
{
	uint8_t datoLeido;

	memcpy(&datoLeido, memoriaPrincipal + df, 1);

	t_buffer *a_enviar = crear_buffer();
	a_enviar->size = 0;
	a_enviar->stream = NULL;

	cargar_uint8_al_buffer(a_enviar, datoLeido);

	printf("se encontro el caracter %u\n", datoLeido);

	t_paquete *un_paquete = crear_super_paquete(RECIBIR_CARACTER, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);
}

//---------------------------------------------------------------------------------------------

void atender_memoria_cpu()
{
	bool control_key = 1;
	t_buffer *un_buffer;
	while (control_key)
	{
		int cod_op = recibir_operacion(fd_cpu);
		switch (cod_op)
		{
		case MENSAJE:

			break;
		case PAQUETE:

			break;
		case SOLICITUD_INSTRUCCION:
			printf("se solicito instruccion a memoria\n");
			un_buffer = recibir_todo_el_buffer(fd_cpu);
			devolver_instruccion(un_buffer);
			break;
		case EJECUTAR_RESIZE:
			un_buffer = recibir_todo_el_buffer(fd_cpu);
			resize(un_buffer);
			break;
		case DEVOLVER_TAMANIO_PAGINA:
			un_buffer = recibir_todo_el_buffer(fd_cpu);
			devolverTamanioPagina(un_buffer);
			break;
		case MANDAR_DATO_A_ESCRIBIR:
			un_buffer = recibir_todo_el_buffer(fd_cpu);
			escribirDato(un_buffer);
			break;
		case MANDAR_DATO_A_LEER:
			un_buffer = recibir_todo_el_buffer(fd_cpu);
			leerDato(un_buffer);
			break;
		case DEVOLVER_MARCO:
			un_buffer = recibir_todo_el_buffer(fd_cpu);
			buscarMarco(un_buffer);
			break;
		case CANT_INTRUCCIONES:
			un_buffer = recibir_todo_el_buffer(fd_cpu);
			int pid = extraer_int_del_buffer(un_buffer);
			obtenerCantInstrucciones(pid);
			break;
		case ESCRIBIR_MEMORIA:
			un_buffer = recibir_todo_el_buffer(fd_cpu);
			escribirMemoria(un_buffer);
			terminoInstruccionMemoria();
			break;
		case LEER_EN_MEMORIA_UN_STRING:
			un_buffer = recibir_todo_el_buffer(fd_cpu);
			leerMemoriaUnString(un_buffer);
			break;
		case LEER_CARACTER_MEMORIA:
			un_buffer = recibir_todo_el_buffer(fd_cpu);
			int df = extraer_int_del_buffer(un_buffer);
			printf("la df recibida es: %d\n");
			leer_caracter(df);
			break;
		case -1:
			log_trace(memoria_log_debug, "Desconexion de CPU - MEMORIA");
			control_key = 0;
			break;
		default:
			// log_warning(logger, "Operacion desconocida de CPU - MEMORIA");
			break;
		}
	}
}
