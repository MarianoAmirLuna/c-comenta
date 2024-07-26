#include "../include/io_kernel.h"
#include <utils/shared.h>
#include <readline/readline.h>
#include <commons/string.h>

void avisarKernelTerminoEjecutarIO(){

	t_buffer *buffer = crear_buffer();
	buffer->size = 0;
	buffer->stream = NULL;

	cargar_string_al_buffer(buffer, nombreInterACrear);

	printf("el nombre de la interfaz es: %s\n",nombreInterACrear);

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
	char* nombreArchivo;
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

			t_buffer *buffer = crear_buffer();
			buffer->size = 0;
			buffer->stream = NULL;

			cargar_string_al_buffer(buffer,nombreInterACrear); //hay que pasar esto asi desp memoria sabe a quien responderle
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

			//Se libera la interfaz en io-memoria

			break;

		case ENVIAR_IO_STDOUT_WRITE:
            un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
	        pid = extraer_int_del_buffer(un_buffer);
            int tamanioXD = extraer_int_del_buffer(un_buffer);

			t_buffer *buffer2 = crear_buffer();
			buffer2->size = 0;
			buffer2->stream = NULL;

			cargar_string_al_buffer(buffer2,nombreInterACrear); //hay que pasar esto asi desp memoria sabe a quien responderle
			cargar_int_al_buffer(buffer2,tamanioXD);

			for(int i = 0; i < tamanioXD; i++){

				numero = extraer_int_del_buffer(un_buffer);
				cargar_int_al_buffer(buffer2,numero);
			}

			t_paquete *paquete2 = crear_super_paquete(LEER_MEMORIA_PALABRA, buffer2);
			enviar_paquete(paquete2, fd_memoria);
			destruir_paquete(paquete2);

			//Se libera la interfaz en io-memoria

			break;
		case ENVIAR_IO_FS_CREATE:
		    un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
	        pid = extraer_int_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);
			printf("ejecute un create\n");

			printf("el pid es %d\n",pid);
			printf("el nombre del archivo es: %s\n",nombreArchivo);

			avisarKernelTerminoEjecutarIO();
			
			break;
		case ENVIAR_IO_FS_DELETE:
		    un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
	        pid = extraer_int_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);
			printf("ejecute un delete\n");

			printf("el pid es %d\n",pid);
			printf("el nombre del archivo es: %s\n",nombreArchivo);

			avisarKernelTerminoEjecutarIO();
			
			break;
		case ENVIAR_IO_FS_TRUNCATE:
		    un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
	        pid = extraer_int_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);
			registro_tamanio = extraer_int_del_buffer(un_buffer);
			printf("ejecute un truncate\n");

			printf("el pid es %d\n",pid);
			printf("el nombre del archivo es: %s\n",nombreArchivo);
			printf("el registro tamanio es: %d\n",registro_tamanio);

			avisarKernelTerminoEjecutarIO();
			
			break;
		case ENVIAR_IO_FS_WRITE:
		    un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
	        pid = extraer_int_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);
			int registro_puntero_write = extraer_int_del_buffer(un_buffer);
			int tamanio_write = extraer_int_del_buffer(un_buffer);

			t_buffer *buffer3 = crear_buffer();
			buffer3->size = 0;
			buffer3->stream = NULL;

			cargar_string_al_buffer(buffer3,nombreInterACrear); //hay que pasar esto asi desp memoria sabe a quien responderle
			cargar_int_al_buffer(buffer3,tamanio_write);

			for(int i = 0; i < tamanio_write; i++){

				numero = extraer_int_del_buffer(un_buffer);
				printf("el numero %d\n",numero);
				cargar_int_al_buffer(buffer3,numero);
			}

			t_paquete *paquete3 = crear_super_paquete(LEER_MEMORIA_PALABRA_DIALS_FS, buffer3);
			enviar_paquete(paquete3, fd_memoria);
			destruir_paquete(paquete3);

			printf("el pid es %d\n",pid);
			printf("el nombre del archivo es: %s\n",nombreArchivo);

			sem_wait(&esperar_palabra_memoria);

			printf("mensaje obtenido: %s\n",palabraIOWrite);

			avisarKernelTerminoEjecutarIO();
			
			break;
		case ENVIAR_IO_FS_READ:
		    /*un_buffer = recibir_todo_el_buffer(fd_entradasalida_kernel);
	        pid = extraer_int_del_buffer(un_buffer);
			nombreArchivo = extraer_string_del_buffer(un_buffer);

			printf("el pid es %d\n",pid);
			printf("el nombre del archivo es: %s\n",nombreArchivo);*/
			
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