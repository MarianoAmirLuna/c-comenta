#include <stdio.h>
#include <stdlib.h>

/*
char* leerLineaTXT(char* path,int PID,int programCounter){
    FILE *flujo = fopen(path,"rb");

    if(flujo == NULL){
        perror("Error al abrir el archivo");
        return NULL;
    }

    char caracter;

    while (feof(flujo) == 0){
        caracter = fgetc(flujo);
        printf("%c",caracter);
    }

    return NULL;
}

*/