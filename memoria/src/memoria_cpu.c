#include "../include/memoria_cpu.h"
#include <utils/shared.h>
#include <inttypes.h>
#include <stdint.h>


#define MAX_LEN 100

char *obtenerInstruccion(char *path, int programCounter)
{ // devuelve la instrucción que está en la fila que indica el program counter
	FILE *archivo = fopen(path, "r");
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

	// printf("el pid que fue serializado: %d\n",pid);
	// printf("el pid que fue serializado: %d\n",program_counter);

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

void solicitarDireccion(int direccion_logica){
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

void leyoPrimeraParte(){
	t_buffer *a_enviar = crear_buffer();
    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_int_al_buffer(a_enviar, 1);

    t_paquete *un_paquete = crear_super_paquete(LEYO_PRIMERA_PARTE, a_enviar);
    enviar_paquete(un_paquete, fd_cpu);
    destruir_paquete(un_paquete);
}


void leerDato(t_buffer *un_buffer){
	// Recibimos los datos para poder hacer el memcopy
	uint8_t data8;
	uint32_t data32;

	int dirFisicaDelDato = extraer_int_del_buffer(un_buffer);
	int tamanioALeer = extraer_int_del_buffer(un_buffer);
	int seEscribe2paginas = extraer_int_del_buffer(un_buffer);
	int tamanioRestantePagina = extraer_int_del_buffer(un_buffer);

	printf("Llegaron los datos a leer.\n");
	printf("la direccion fisica donde hay que leer es: %d\n", dirFisicaDelDato);
	printf("el tamanio a leer: %d\n", tamanioALeer);
	printf("el tamanio que resta de pag es: %d\n", tamanioRestantePagina);

	if (tamanioALeer == 1) // Caso donde tenemos que leer algo de 1 byte
	{
		uint8_t datoLeido8; 
		memcpy( &datoLeido8, (memoriaPrincipal + dirFisicaDelDato), tamanioALeer);
		tamanioRestantePagina = tamanioRestantePagina - 1;
		printf("############## EL DATO A LEER ES:%" PRIu8 "\n", datoLeido8);
	}
	else
	{
		if (seEscribe2paginas == 1)
		{ // caso turbio que hay que leer en 2 paginas diferentes

			//ahora leo solo la parte 1 - escribo en el registro la 1ra parte del marco
			printf("leo la primera parte\n");
			memcpy((memoriaPrincipal), (memoriaPrincipal + dirFisicaDelDato), tamanioRestantePagina);

			// Necesito la direccion fisica del 2 marco
			leyoPrimeraParte(); //le avisa a la cpu que ya puede leer la segunda parte;

			cuantoFaltaLeer = 4 - tamanioRestantePagina;
		}
		else //caso donde tenemos que leer algo de 4 bytes, pero está todo en 1 solo marco
		{
			memcpy((memoriaPrincipal), (memoriaPrincipal + dirFisicaDelDato), tamanioALeer);
		}

		tamanioRestantePagina = tamanioRestantePagina - 4;
	}

	//sacamos todo lo del bitarray también, ya que acá no hay que modificarlo

	// Mandamos basura, para hacer el sem_post
	t_buffer *a_enviar = crear_buffer();
	a_enviar->size = 0;
	a_enviar->stream = NULL;

	cargar_int_al_buffer(a_enviar, 1);

	t_paquete *un_paquete = crear_super_paquete(LECTURA_HECHA, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);

}

void escribioPrimeraParte(){
	t_buffer *a_enviar = crear_buffer();
    a_enviar->size = 0;
    a_enviar->stream = NULL;

    cargar_int_al_buffer(a_enviar, 1);

    t_paquete *un_paquete = crear_super_paquete(ESCRIBIO_PRIMERA_PARTE, a_enviar);
    enviar_paquete(un_paquete, fd_cpu);
    destruir_paquete(un_paquete);
}

void escribirDato(t_buffer *un_buffer)
{
	// Recibimos los datos para poder hacer el memcopy
	u_int8_t data8;
	uint32_t data32;

	int direccion_logica = extraer_int_del_buffer(un_buffer);
	int direccion_fisica = extraer_int_del_buffer(un_buffer);
	int tamanio_a_escribir = extraer_int_del_buffer(un_buffer);

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

	if (tamanio_a_escribir == 1)
	{
		memcpy((memoriaPrincipal + direccion_fisica), &data8, tamanio_a_escribir);
		tamanioRestantePagina = tamanioRestantePagina - 1;
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

            //estos 2 memcpy son para dividir el uint32
			memcpy(data32_parte_1, data32_puntero, tamanioRestantePagina);
			memcpy(data32_parte_2, data32_puntero + tamanioRestantePagina, 4 - tamanioRestantePagina);

			printf("El valor de ECX completo es: %d \n", data32);
			printf("El valor de ECX antes de reconstruirlo: %d \n", data32_reconstruido);

			//estos sirven para reconstruir el puntero
			memcpy(data32_reconstruido_puntero, data32_parte_1, tamanioRestantePagina);
			memcpy(data32_reconstruido_puntero + tamanioRestantePagina, data32_parte_2, 4 - tamanioRestantePagina);

			printf("El valor de ECX despues de reconstruirlo: %d \n", data32_reconstruido);

			//ahora escribo posta en la memoria solo la parte 1 
			printf("escribi la primera parte de la instruccion\n");
			memcpy(memoriaPrincipal + direccion_fisica, data32_parte_1, tamanioRestantePagina);
			escribioPrimeraParte(); //le avisa a la cpu que ya puede escribir la segunda parte;
			//memcpy(memoriaPrincipal + dir_fisica_global, data32_parte_2, 4 - tamanioRestantePagina);
			dataParte2Global = data32_parte_2; 
			cuantoFaltabaEscribir = 4 - tamanioRestantePagina;

			free(data32_parte_1);
			free(data32_parte_2);
		}
		else
		{
			memcpy((memoriaPrincipal + direccion_fisica), &data32, tamanio_a_escribir);
		}

		tamanioRestantePagina = tamanioRestantePagina - 4;
	}

	int num_pag = direccion_logica / TAM_PAGINA;

	if (tamanioRestantePagina <= 0)
	{ // me quede sin espacio en la pagina y tengo que setear el marco como "no disponible"
		bitarray_set_bit(frames_ocupados_ppal, num_pag);
	}

	// Mandamos basura, para hacer el sem_post
	t_buffer *a_enviar = crear_buffer();
	a_enviar->size = 0;
	a_enviar->stream = NULL;

	cargar_int_al_buffer(a_enviar, 1);

	t_paquete *un_paquete = crear_super_paquete(ESCRITURA_HECHA, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);
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

/*
int contarBitsValidez(tablaPaginas* tabla) {
	int contador = 0;
	for (int i = 0; i < 40; ++i) {
		if (tabla->array[i].bitValidez == 1) {
			contador++;
		}
	}
	return contador;
}

void imprimirBitsValidez(tablaPaginas p) {

	printf("PID: %d\n", p.pid);
	for(int i = 0; i < 10; i++) {
		printf("Bit de validez del marco %d: %d\n", i, p.array[i].bitValidez);
		printf("El nro de marco asignado: %d:\n", p.array[i].marco);
	}
}*/

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

	printf("el tamanio actual es: %d\n", tamanioActual);
	printf("el tamanio a modificar es: %d\n", tamanioAModificar);

	if (tamanioAModificar > tamanioActual)
	{ // si necesitamos mas paginas
		printf("ENTRE AL IFFFFFFFFFFFFF\n");

		int bytesNecesarios = tamanioAModificar - tamanioActual;
		int paginasNecesarias = ceil((double)bytesNecesarios / (double)TAM_PAGINA);

		printf("se van a solicitar: %d\n", paginasNecesarias);

		reservarFrames(tablaPag, paginasNecesarias); // aca esta el error
	}
	else
	{
		// if (tamanioAModificar <= tamanioActual)
		//{ // si quiero sacar paginas
		// printf("ENTRE AL ELSEEEEEEEEE");
		// int cantBytesModificar = tamanioActual - tamanioAModificar;
		// int cantPaginasABorrar = ceil((double)cantBytesModificar / (double)TAM_PAGINA);

		// liberarFrames(tablaPag, cantPaginasABorrar);
		//}
	}

	printf("-------------------------------");
}

void buscarMarco(t_buffer *un_buffer)
{

	int num_pag = extraer_int_del_buffer(un_buffer);
	int pid = extraer_int_del_buffer(un_buffer);
	t_buffer *a_enviar = crear_buffer();

	a_enviar->size = 0;
	a_enviar->stream = NULL;

	tablaPaginas *tablaDePaginas = obtener_tabla_pagina(pid);
	int marco = tablaDePaginas->array[num_pag].marco;
	// Si estan pasando cosas raras con el marco que va a recibir el cpu. Puede ser que no se haya inicializado bien el array.

	cargar_int_al_buffer(a_enviar, marco);

	t_paquete *un_paquete = crear_super_paquete(RECIBIR_MARCO, a_enviar);
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

		case SEGUNDA_DIRECCION:
		    un_buffer = recibir_todo_el_buffer(fd_cpu);
			dir_fisica_global = extraer_int_del_buffer(un_buffer);
			printf("ya escribi la segunda parte de la instruccion\n");
			printf("me faltaba escribir: %d\n",cuantoFaltabaEscribir);
			memcpy(memoriaPrincipal + dir_fisica_global, dataParte2Global, cuantoFaltabaEscribir);
			break;
		case SEGUNDA_DIRECCION_A_LEER:
		    un_buffer = recibir_todo_el_buffer(fd_cpu);
			int dirFisicaDel2MarcoALeer = extraer_int_del_buffer(un_buffer);
			printf("Ya lei la segunda parte.\n");
			printf("me faltaba escribir: %d\n",cuantoFaltaLeer);
			memcpy(dirFisicaDondeHayQueAlmacenarGlobal, memoriaPrincipal + dirFisicaDel2MarcoALeer, cuantoFaltaLeer);
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
