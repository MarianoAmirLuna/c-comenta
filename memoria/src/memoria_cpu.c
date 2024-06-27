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

void devolverTamanioPagina(t_buffer *un_buffer){

	int numero = extraer_int_del_buffer(un_buffer);

	t_buffer *a_enviar = crear_buffer();

	a_enviar->size = 0;
	a_enviar->stream = NULL;

	cargar_int_al_buffer(a_enviar, TAM_PAGINA);

	t_paquete *un_paquete = crear_super_paquete(RECIBIR_TAMANIO, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);
}

void puntero_a_cadena(char *destino, size_t tam_destino, void *puntero)
{
    snprintf(destino, tam_destino, "%p", puntero);
}

void *cadena_a_puntero(const char *cadena)
{
    void *puntero = NULL;
    sscanf(cadena, "%p", &puntero);
    return puntero;
}

void escribirDato(t_buffer *un_buffer){
	//Recibimos los datos para poder hacer el memcopy
	u_int8_t data8;
	uint32_t data32;

	int direccion_logica = extraer_int_del_buffer(un_buffer);
	int direccion_fisica = extraer_int_del_buffer(un_buffer);
	int tamanio_a_escribir = extraer_int_del_buffer(un_buffer);

	if(tamanio_a_escribir  == 1){
		data8 = extraer_uint8_del_buffer(un_buffer);
	}
	else{
		data32 = extraer_uint32_del_buffer(un_buffer);
	}
	
	int seEscribe2paginas = extraer_int_del_buffer(un_buffer);
    int tamanioRestantePagina = extraer_int_del_buffer(un_buffer);

	printf("la direccion fisica: %d\n",direccion_fisica);
	printf("el tamanio a escribir: %d\n",tamanio_a_escribir);
	printf("El valor de uint8_t es: %u\n", data8);
	printf("El valor de uint32_t es: %u\n", data32);

    if(tamanio_a_escribir == 1){
		memcpy((memoriaPrincipal + direccion_fisica),&data8,tamanio_a_escribir);
	}
	else{
		if(seEscribe2paginas == 1){

			
		}
		else{
			memcpy((memoriaPrincipal + direccion_fisica),&data32,tamanio_a_escribir);
		}
	}

	//memcpy((memoriaPrincipal + direccion_fisica),escribir,tamanioAEscribir);
	
	//Mandamos basura, para hacer el sem_post
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
		//if (tamanioAModificar <= tamanioActual)
		//{ // si quiero sacar paginas
			//printf("ENTRE AL ELSEEEEEEEEE");
			//int cantBytesModificar = tamanioActual - tamanioAModificar;
			//int cantPaginasABorrar = ceil((double)cantBytesModificar / (double)TAM_PAGINA);

			// liberarFrames(tablaPag, cantPaginasABorrar);
		//}
	}

	printf("-------------------------------");
}

void buscarMarco(t_buffer* un_buffer){

	int num_pag = extraer_int_del_buffer(un_buffer);
	int pid = extraer_int_del_buffer(un_buffer);
	t_buffer *a_enviar = crear_buffer();
	
	a_enviar->size = 0;
	a_enviar->stream = NULL;

	tablaPaginas* tablaDePaginas = obtener_tabla_pagina(pid);
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
		case DEVOLVER_MARCO:
			un_buffer = recibir_todo_el_buffer(fd_cpu);
			buscarMarco(un_buffer);
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
