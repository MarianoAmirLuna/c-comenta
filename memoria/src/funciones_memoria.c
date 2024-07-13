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

/*
t_list *buscarFramesSinOcupar()
{
	t_list *framesLibres = list_create();

	int base = 0;

	// ver si es necesario un semaforo

	while (base < cant_frames_ppal)
	{
		if (bitarray_test_bit(frames_ocupados_ppal, base) == 0)
		{
			list_add(framesLibres, base);
		}
		base++;
	}

	int marcosLibres = list_size(framesLibres);

	printf("la cantidad de marcos libres es: %d\n", marcosLibres);

	return framesLibres;
} // funca */

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

	return framesParaUsar;
}

/*
void reservarFrames(tablaPaginas *tablaPags, int cantidadPaginasNecesarias)
{ // revisar si esto anda bien
	t_list *framesLibres = buscarFramesSinOcupar();

	for (int i = 0; i < cantidadPaginasNecesarias; i++)
	{
		marcoBit* marcobit = malloc(sizeof(marcoBit));
		int *frame = list_get(framesLibres, i);

		bitarray_set_bit(frames_ocupados_ppal, *frame);

		marcobit->bitValidez = true;
		marcobit->marco = *frame;

		list_add(&(tablaPags->marco_bit), marcobit);
	}

	int tamanioReservarFrames = list_size(&(tablaPags->marco_bit));

	printf("el tamanio al reservar frames: %d\n",tamanioReservarFrames);
}*/



void liberarFrames(tablaPaginas *tablaPags, int cantidadPaginasBorrar)
{
	int cantBitsValidez = contarBitValidez(tablaPags);							 
	int delta = cantBitsValidez - cantidadPaginasBorrar; //32 - 30 => 2
	cantBitsValidez--;

	printf("cantbits validez: %d\n",cantBitsValidez);
	printf("deta: %d\n",delta);
													
	for (int i = cantBitsValidez; i > delta; i--)
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
	/*
	for (int i = 0; i < cant_frames_ppal; i++)
	{
		// Reservar memoria para la estructura de la página
		t_page *page = malloc(sizeof(t_page));
		if (page == NULL)
		{
			printf("Error al reservar memoria para la pagina");
		}

		// Asignar la parte correspondiente de memoryBytes a la página
		page->data = memoriaPrincipal + i * TAM_PAGINA;

		// Añadir la página a la lista
		list_add(listaPaginas, page);
	}
	*/
	// Utilizar la lista de páginas (por ejemplo, imprimir las direcciones)
	// for (int i = 0; i < list_size(listaPaginas); i++)
	//{
	// t_page *page = list_get(listaPaginas, i);
	// printf("Página %d en dirección %p\n", i, page->data);
	//}

	// tablaPaginas tablita = inicializarTablaPaginas(1);

	// t_list* listaxd = reservarFrames(tablita, 3);

	// int tamanioListaxd = list_size(listaxd);

	// buscarFramesSinOcupar();

	// printf("el tamanio de la listaxd es: %d\n",tamanioListaxd);

	//free(memoriaPrincipal);

	// list_destroy_and_destroy_elements(listaPaginas, free); esto se tiene que liberar desp de que se termine el programa
}
