#include "../include/funciones_memoria.h"
#include <utils/shared.h>
#include "commons/string.h"

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

t_list *buscarFramesSinOcupar()
{
	t_list *framesLibres = list_create();

	int base = 0;

	// Iterar sobre todos los marcos principales
	while (base < cant_frames_ppal)
	{
		// Verificar si el marco está ocupado
		if (bitarray_test_bit(frames_ocupados_ppal, base) == 0)
		{
			// Agregar el marco libre a framesLibres
			int *marcoLibre = malloc(sizeof(int)); // Asignar memoria para el entero
			if (marcoLibre != NULL)
			{
				*marcoLibre = base; // Guardar el número de marco libre
				list_add(framesLibres, marcoLibre);
			}
			else
			{
				printf("Error: no se pudo asignar memoria para marcoLibre.\n");
				// Manejo del error si falla la asignación de memoria
				list_destroy(framesLibres); // Liberar framesLibres antes de retornar NULL
				return NULL;
			}
		}
		base++;
	}
	int marcosLibres = list_size(framesLibres);
	printf("La cantidad de marcos libres es: %d\n", marcosLibres);

	return framesLibres;
}

int calcularPaginasNecesarias(int tamanio)
{
	return ceil((double)tamanio / (double)TAM_PAGINA);
}

tablaPaginas* inicializarTablaPaginas(int pid)
{
	
	tablaPaginas* miTabla = malloc(sizeof(tablaPaginas));

	miTabla->pid = pid;
	miTabla->cantMarcos = 0;
	
	for(int i = 0; i < 40; i++) {
        miTabla->array[i].marco = 0;
        miTabla->array[i].bitValidez = false;
    }

	return miTabla;
}

int contarBitValidez(tablaPaginas *tabla)
{
	int contador = 0;
	for (int i = 0; i < 40; i++)
	{
		//printf("el marco es: %d\n",tabla->array[i].marco);
		//printf("el bitValidez es: %d\n",tabla->array[i].bitValidez);

		if (tabla->array[i].bitValidez == true)
		{
			contador++;
		}
	}
	return contador;
}


t_list *reservarFrames(tablaPaginas *tablaPags, int cantidadPaginasNecesarias, int tamanioActual)
{
	t_list *framesLibres = list_create();
	t_list *framesParaUsar = list_create();

	framesLibres = buscarFramesSinOcupar();

	if (cantidadPaginasNecesarias < list_size(framesLibres)) // verifico si hay memoria suficiente
	{

		framesParaUsar = list_take(framesLibres, cantidadPaginasNecesarias);
		int aux = 0;

		for (int i = tamanioActual; i < cantidadPaginasNecesarias; i++) 
		{
			int *frame = list_get(framesLibres, aux);

			bitarray_set_bit(frames_ocupados_ppal, *frame);

			tablaPags->array[i].marco = *frame;
			tablaPags->array[i].bitValidez = true;
			tablaPags->cantMarcos++;
			aux++;

		} // actualizo el bitarray y la tabla de paginas
	}
	else
	{
		printf("no hay memoria suficiente\n");
	}

	//safe_list_destroy_and_destroy_elements(framesLibres, safe_int_destroyer);

	return framesParaUsar;
}

void liberarFrames(tablaPaginas *tablaPags, int cantidadPaginasBorrar)
{
	int cantBitsValidez = contarBitValidez(tablaPags);							 
	int delta = cantBitsValidez - cantidadPaginasBorrar; //32 - 30 => 2
	cantBitsValidez--;

	printf("cantbits validez: %d\n",cantBitsValidez);
	printf("deta: %d\n",delta);
													
	for (int i = cantBitsValidez; i >= delta; i--)
	{

		bitarray_clean_bit(frames_ocupados_ppal, i);

		printf("el index es:%d\n",i);

		tablaPags->array[i].bitValidez = false;
		tablaPags->array[i].marco = 0;
		tablaPags->cantMarcos--;
	}
}

void iniciarPaginacion()
{
	printf("entre a iniciar paginacion\n");

	printf("tamanio de memoria es: %d\n", TAM_MEMORIA);
	printf("tamanio de pagina es: %d\n", TAM_PAGINA);

	memoriaPrincipal = malloc(TAM_MEMORIA);

	if (memoriaPrincipal == NULL)
	{
		printf("Error en malloc");
	}

	cant_frames_ppal = TAM_MEMORIA / TAM_PAGINA;

	char *data = asignarMemoriaBits(cant_frames_ppal);

	if (data == NULL)
	{

		printf("MALLOC FAIL!\n");
	}

	memset(data, 0, cant_frames_ppal / 8);

	frames_ocupados_ppal = bitarray_create_with_mode(data, cant_frames_ppal / 8, MSB_FIRST);
	
}
