
/*
#include "../include/funciones_memoria.h"
#include <utils/shared.h>

int bitsToBytes(int bits){
	int bytes;
	if(bits < 8)
		bytes = 1; 
	else
	{
		double c = (double) bits;
		bytes = ceil(c/8.0);
	}
	
	return bytes;
}

char* asignarMemoriaBits(int bits)//recibe bits asigna bytes
{
	char* aux;
	int bytes;
	bytes = bitsToBytes(bits);
	//printf("BYTES: %d\n", bytes);
	aux = malloc(bytes);
	memset(aux,0,bytes);
	return aux; 
}

int iniciarPaginacion(){

    memoriaPrincipal = malloc(*TAM_MEMORIA);

    if(memoriaPrincipal == NULL){
        printf("Error en malloc");
        return 0;
    }

    int tamanioPaginas = *TAM_PAGINA;
    int tamanioMemoria = *TAM_MEMORIA;

    cant_frames_ppal = tamanioMemoria / tamanioPaginas;

    data = asignarMemoriaBits(cant_frames_ppal);
    
    if(data == NULL){
        
        perror("MALLOC FAIL!\n");
        return 0;
    }

    memset(data,0,cant_frames_ppal/8);
    frames_ocupados_ppal = bitarray_create_with_mode(data, cant_frames_ppal/8, MSB_FIRST);

	//listaTablaPaginas = list_create();

	return 1;
}

*/