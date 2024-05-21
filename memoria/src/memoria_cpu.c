#include "../include/memoria_cpu.h"
#include <utils/shared.h>

#define MAX_LEN 100

char* obtenerInstruccion(char* path, int programCounter) { //devuelve la instrucción que está en la fila que indica el program counter
    FILE* archivo = fopen(path, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    char linea[MAX_LEN];
    int contador = 0;

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        contador++;
        if (contador == programCounter) {
            // Elimina el salto de línea al final de la línea
            char* nuevaLinea = strchr(linea, '\n');
            if (nuevaLinea != NULL) {
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

	printf("el path es: %s\n",elemento_lista->path);

	return elemento_lista->path;
}

void devolver_instruccion(t_buffer *un_buffer)
{
	int pid = extraer_int_del_buffer(un_buffer);
	int program_counter = extraer_int_del_buffer(un_buffer);

	//printf("el pid que fue serializado: %d\n",pid);
	//printf("el pid que fue serializado: %d\n",program_counter);

	char *path_instruccion = obtener_instruccion_lista(pid, program_counter);

	t_buffer *a_enviar = crear_buffer();

	a_enviar->size = 0;
	a_enviar->stream = NULL;

	//printf("el path de instrucciones es: %s\n",path_instruccion);

	//printf("el path instruccion es:%s\n",path_instruccion);
	//printf("el program counter es:%d\n",program_counter);

	char* instruccion = obtenerInstruccion(path_instruccion,program_counter);

	//printf("la instruccion es: %s\n",instruccion);

	cargar_string_al_buffer(a_enviar, instruccion);

	t_paquete *un_paquete = crear_super_paquete(RECIBIR_INSTRUCCION, a_enviar);
	enviar_paquete(un_paquete, fd_cpu);
	destruir_paquete(un_paquete);
}


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
		case -1:
			log_trace(memoria_log_debug,"Desconexion de CPU - MEMORIA");
			control_key = 0;
			break;
		default:
			//log_warning(logger, "Operacion desconocida de CPU - MEMORIA");
			break;
		}
	}
}


