#include "../include/memoria_cpu.h"
#include <utils/shared.h>

void enviarInstruccion(char* path, uint32_t PC){
	FILE* f = fopen(path, "wb");
	char c;
	char* instruccion = malloc(100); //asumo instrucciones siempre menores a 100 caracteres
	fread(&c, 1, 1, f);
	uint32_t num_instruccion=0, offset=0;
	while(num_instruccion!=PC){
		if(c=='\n') num_instruccion++; //cuenta las instrucciones por los saltos de linea hasta llegar a la pedida por PC
		fread(&c, 1, 1, f);
	}//ya se llegó a la instruccion del PC
	while(c!='\n'){
		instruccion[offset]=c;
		offset++;
		fread(&c, 1, 1, f);
	}
	instruccion[offset]='\0';
	void *stream = malloc(sizeof(uint32_t) + offset);
	memcpy(stream, offset, sizeof(uint32_t));
	memcpy(stream+sizeof(uint32_t),instruccion,offset);
	send(fd_cpu, stream, offset+sizeof(uint32_t), 0);

	free(instruccion);
	fclose(f);
}

void atender_memoria_cpu(){
    bool control_key = 1;
	while (control_key) {
		int cod_op = recibir_operacion(fd_cpu); 
		switch (cod_op) {
		case MENSAJE:
			//
			break;
		case PAQUETE:
            //
			break;
		case -1:
			log_trace(memoria_log_debug, "Desconexion de CPU - MEMORIA");
			control_key = 0;
			break;
		default:
			log_warning(logger,"Operacion desconocida de CPU - MEMORIA");
			break;
		}
	}
}